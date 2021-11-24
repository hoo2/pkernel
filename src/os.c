/*
 * os.c : This file is part of pkernel
 *
 * Copyright (C) 2013 Choutouridis Christos <houtouridis.ch@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:     Choutouridis Christos <houtouridis.ch@gmail.com>
 * Date:       03/2013
 * Version:
 *
 */

#include <os.h>

clock_t  volatile Ticks = 0;           /* cpu time */
time_t   volatile Now = 0;             /* time in unix secs past 1-Jan-70 */

static os_command_t os_command;

/*=============  Interrupt Service Routines  ====================*/

/*!
 * \brief This ISR handles SysTick. We update Ticks and Now.
 * We also consume time from the active process and trigger PendSV.
 *
 * \param  None
 * \retval None
 */
void SysTick_Handler(void)
{
   // Trigger PendSV
   __pendsv_trig ();

   /*
    * Update Exported Counters and
    * call cron and micron.
    */
   ++Ticks;
   services ();
   if ( !(Ticks % get_freq ()))
   {
      ++Now;
      cron ();
   }
   // In case of cron stretching call cron() continuously.
   if (cron_stretching())
      cron ();

   // If we have process in runq consume time of it.
   if ( !sch_runq_empty () )
      proc_dec_ticks (proc_get_current_pid());
   // Return to PendSV
   //__asm volatile ("BX %0" : : "r" (0xFFFFFFF1));
}

/*!
 * \brief This ISR handles PendSV.
 *
 * Actions:
 * - Save the active process's stack.
 * - Get the "correct" pid from scheduler
 * - If needed, context switching
 * - Load the stack for the "correct" process
 * - Return to Process.
 *
 * \note This means that the PendSV must handled last by the NVIC
 *
 * \param  None
 * \retval None
 */
void PendSV_Handler (void)
{
   uint32_t reg = proc_save_ctx ();
   /*^
    * \note reg is static so it lives between context switches
    *
    * At this point NVIC has pushed the hw_stack_frame
    * We push lr and sw_stack_frame, and save the sp of it to
    * proc[].tcb.sp
    */

   proc_store_stack_pointer (reg);

   if (os_command.flags)         // Clear wait flags
      os_command.flags = 0;
   /*
    * Get from scheduler the pid to run and check
    * if we have to change stack.
    */
   if ((reg = proc_sel_stack (schedule ())) != 0)
       kset_PSP (reg);
   /*
    * Load the sw_stack_frame of the process to run.
    * The NVIC will load the hw_stack_frame.
    */
   proc_load_ctx ();
   // Add custom epilogue to return from ISR
   // 0xFFFFFFF9 : MSP priviledged
   // 0xFFFFFFFD : PSP un-priviledged
   __asm volatile ("MOV lr, #0xFFFFFFFD \n\t"
                   "BX lr               \n\t" );
}

/*!
 * \brief Provide a functionality based on the os_command_enum_t
 * from pkernel.
 *
 * \param p The corresponding process.
 * \param cmd The command to apply.
 * \note Thread safe, not reentrant.
 *
 * \warning Don't use directly this function. Use it through exit(), sleep() wait()
 *
 */
void OS_Call (process_t *p, os_command_enum_t cmd)
{
   /*
    * We have to wait any other OS action to finish.
    * So we wait SysTick and PendSV.
    */
   while ( __pendsv_act() || __systick_act() )
      ;
   __os_halt_ISR();
   switch (cmd)
   {
      case OS_EXIT:
         sch_exit (p);
         proc_exit (p);
         break;
      case OS_SUSPEND:
         sch_susp_proc (p);
         break;
      default:
         return;
      /*
       * XXX: I know its not safe to make runq, susq write operations.
       * But for now we assume we are just after SysTick.
       * In the near future OS_Call will be discarded and SVC will take over.
       */
   }
   __os_resume_ISR();
   // Set flags, trigger PendSV and wait.
   os_command.flags |= 0x1 << cmd;
   __pendsv_trig ();
   while (os_command.flags && (0x1 << cmd))
      ;
   return;
}

/*!
 * \brief This function triggers OS_Call to terminate the process.
 * Called when a process returns. Can also called from a process
 * to terminate the process. This function never returns.
 *
 * \param  status is discarded. We don't use it
 * \note Thread safe, not reentrant.
 */
void exit (int status)
{
   (void) status;
   process_t *p = proc_get_current_proc();
   OS_Call (p, OS_EXIT);
   while(1);   // for compatibility
   /*
    * \Note status is discarded. We don't use it.
    */
}

/*!
 * \brief This function triggers OS_Call to suspend the process.
 * A call to this function cause pkernel to suspend the process
 * immediate without the need for waiting the SysTick. This function
 * returns when the process resumes / wakes up.
 *
 * \param alarm The number of ticks in 1/CLOCKS_PER_SEC before
 * the process wakes up.
 * \note Thread safe, not reentrant.
 */
void sleep (clock_t alarm)
{
   process_t *p = proc_get_current_proc ();

   p->alarm = Ticks + alarm;
   OS_Call (p, OS_SUSPEND);
}


/*!
 * \brief This function waits for a semaphore. If the semaphore
 *  is positive decreases it, if 0 then suspends the process.
 *
 * \param  s    Pointer to semaphore used
 * \return None
 * \note Thread safe, not reentrant.
 */
void sem_wait (sem_t *s) {
   process_t *p;

   if (s->val <= 0) {
       p = proc_get_current_proc ();
       p->sem = s;
       OS_Call (p, OS_SUSPEND);
   }
   // here s->val is always positive
   --s->val;
}

/*!
 * \brief Increase the semaphores value, but leave
 * the OS to awake the process.
 */
void sem_post (sem_t *s) {
   ++s->val;
   /*
    * \Note we leave next tick's scheduler to awake the related process.
    */
}

/*!
 * \brief  This function waits for a mutex. If the mutex is
 * positive(1) decreases it, if 0 then suspends the process.
 * \param  m pointer to mutex used
 * \return None
 * \note Thread safe, not reentrant.
 */
inline void mut_lock (sem_t *m) {
   sem_wait (m);
}


/*!
 * Unlock (by setting high) the semaphore, but leave
 * the OS to awake the process.
*/
void mut_unlock (sem_t *m) {
    m->val =1;
   /*
    * \Note we leave next tick's scheduler to awake the related process.
    */
}
