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

/*=============  Interrupt Service Routine  ====================*/

/**
  * @brief  This function handles SysTick.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
   int pid;

   ++Ticks;     //Update Exported Counters
	if ( !(Ticks % kget_os_freq ()) )
	   ++Now;

	/* If we have process in runq and the pkernel is up
	 * consume time of it.
	 */
   if (!sch_runq_empty () && (pid=proc_get_current_pid () ))
      proc_dec_ticks (pid);

   // Pend OS handler
   OS_Call ((void*)0, OS_TRIG);
}
/**
  * @brief  This function handles PendSV.
  * @param  None
  * @retval None
  */
void PendSV_Handler (void)
{
	uint32_t reg;
   static uint32_t _lr=0;

	__asm volatile ("MOV %0, lr" : "=r" (_lr));

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
	__asm volatile ("BX %0" : : "r" (_lr));
}


void OS_Call (process_t *p, os_command_enum_t cmd)
{
   // Dispatch command
   switch (cmd)
   {
      case OS_TRIG:
         kSCB->ICSR |= kSCB_ICSR_PENDSVSET_Msk;
         return;
      case OS_SUSPEND:
         /*
          * We have to wait any other OS action to finish.
          * So we wait SysTick and PendSV.
          */
         while (kSCB->SHCSR & kSCB_SHCSR_PENDSVACT_Msk
                || kSCB->SHCSR & kSCB_SHCSR_SYSTICKACT_Msk)
            ;
         sch_susp_proc (p);
         os_command.flags |= 0x1 << cmd;
         kSCB->ICSR |= kSCB_ICSR_PENDSVSET_Msk;
         while (os_command.flags && (0x1<<cmd))   // Wait PendSV here.
            ;
         return;
      default:
         return;
   }
}

void sleep (clock_t alarm)
{
   process_t *p = proc_get_current_proc ();

   p->alarm = Ticks + alarm;
   OS_Call (p, OS_SUSPEND);
}

/*
   void wait (sem_t *s)

   If the semaphore is positive decreases it,
   if 0 then suspends the process.

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

/*
 * void signal (sem_t *s)
 *
 * Increases the semaphores value, but leave
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

/*
 *  void lock (sem_t *s)
 *
 * If the mutex is positive clears it,
 * if 0 then suspends the process.
 */
inline void lock (sem_t *s)   // same as wait
{
   return wait(s);
}


/*
 * void unlock (sem_t *m)
 *
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
