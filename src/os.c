/*
 * os.c : This file is part of pkernel
 *
 * Copyright (C) 2013 Houtouridis Christos <houtouridis.ch@gmail.com>
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
 * Author:     Houtouridis Christos <houtouridis.ch@gmail.com>
 * Date:       03/013
 * Version:
 *
 */

#include "os.h"

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
   //Update Exported Counters
   ++Ticks;
	if ( !(Ticks % kget_os_freq ()) )
	   ++Now;

	// If we have process in runq consume time of it.
   if ( !sch_runq_empty () )
      proc_dec_ticks (proc_get_current_pid());

   // Trigger PendSV
   __pendsv_trig ();
}

/*!
 * \brief This ISR handles PendSV. We:
 * - Save the active process's stack.
 * - Get the "correct" pid from scheduler
 * - If needed we do context switching
 * - Load the stack for the "correct" process
 * - Return to Process.
 * (*This mean that the PendSV must handled last by the NVIC*)
 *
 * \param  None
 * \retval None
 */
void PendSV_Handler (void)
{
   static uint32_t linkreg=0;
	uint32_t reg;
   /*
    * XXX: linkreg is static so it lives between context switches
    */

	__asm volatile ("MOV %0, lr" : "=r" (linkreg));

	if (os_command.flags)         // Clear wait flags
	   os_command.flags = 0;
	/*
	 * At this point NVIC has pushed the hw_stack_frame
	 * We push lr and sw_stack_frame, and save the sp of it to
	 * proc[].tcb.sp
	 */
	reg = proc_save_ctx ();
	proc_store_stack_pointer (reg);
	/*
	 * Get from scheduler the pid to run and check
	 * if we have to change stack.
	 */
	reg = schedule ();
	reg = proc_sel_stack (reg);
	if (reg)
		context_switch (reg);
	/*
	 * Load the sw_stack_fram of the process to run.
	 * The NVIC will load the hw_stack_frame.
	 */
	proc_load_ctx ();
	// Add custom epilogue to return from ISR
	__asm volatile ("BX %0" : : "r" (linkreg));
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
   }
   // Set flags, trigger PendSV and wait.
   os_command.flags |= 0x1 << cmd;
   __pendsv_trig ();
   while (os_command.flags && (0x1<<cmd))
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
   process_t *p = proc_get_current_proc();
   OS_Call (p, OS_EXIT);
   while(1);   // for compatibility
   /*
    * XXX: status is discarded. We don't use it.
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
 * \param  s pointer to semaphore used
 * \return None
 * \note Thread safe, not reentrant.
 */
void wait (sem_t *s)
{
   process_t *p;

   if (s->val <=0)
   {
      p = proc_get_current_proc ();
      p->sem = s;
      OS_Call (p, OS_SUSPEND);
   }
   else
      s->val--;
}

/*!
 * \brief Increase the semaphores value, but leave
 * the OS to awake the process.
 */
void signal (sem_t *s)
{
   ++s->val;
   /*
    * XXX:If value is positive we leave next tick's
    * scheduler to awake the related process.
    */
}

/*!
 * \brief  This function waits for a mutex. If the mutex is
 * positive(1) decreases it, if 0 then suspends the process.
 * \param  s pointer to mutex used
 * \return None
 * \note Thread safe, not reentrant.
 */
__INLINE void lock (sem_t *s){
   return wait(s);
}


/*!
 * Unlock (by setting high) the semaphore, but leave
 * the OS to awake the process.
*/
void unlock (sem_t *m)
{
   if (++m->val > 1) // Binary semaphore
      m->val=1;
   /*
    * XXX:If value is positive we leave next tick's
    * scheduler to awake the related process.
    */
}
