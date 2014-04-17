/*
 * pkernel.c : This file is part of pkernel
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
 * Date:       03/2013
 * Version:
 *
 */

#include <pkernel.h>

/*!
 * \brief Create a new process. Try to allocate memory and proc space.
 * On success returns the pid of the new process, else returns 0.
 *
 * \param fptr Pointer to process function.
 * \param mem The size of process stack in bytes.
 * \param nice The nice ratio (-10 .. 10) of the process.
 * \param fit The fit (-10 .. 10) ratio of the process.
 * \return the process pid
 * \note Use enough memory space for OS ISRs to.
 *
 */
pid_t knew (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit)
{
   pid_t pid = 0;

   __os_halt_ISR();
   pid = proc_newproc(fptr, mem, nice, fit);
   if (pid != -1) // Success
      sch_add_proc (pid);

   __os_resume_ISR();
   return pid;
}

/*!
 * \brief Boot the kernel.
 * - Set priorities
 * - Set clock
 * - Set os frequency,
 * - Start allocator
 * - Create the idle process. Allocator though will
 * not give heap though before pkernel_run().
 *
 * \param __kmsize The pkernels size (aka the idle process stack size).
 * \param clk The CPU clock used by application.
 * \param os_f The OS freq requested by application.
 *
 * \return 0(proc_idle's pid) on success.
 */
int kinit (size_t kmsize, clock_t clk, clock_t os_f)
{
   pid_t pid;

   kSetPriority(kPendSV_IRQn, OS_PENDSV_PRI);
   kSetPriority(kSysTick_IRQn, OS_SYSTICK_PRI);

   set_clock (clk);     // Set kernel's knowledge for clocking and freq
   set_freq (os_f);
   alloc_init ();       // Init the Stack allocation table.

   // Make the idle proc
   pid = proc_newproc ((process_ptr_t)&proc_idle, kmsize, 0, 0);
   /*
    * \note
    * We make sure that we are outside off ANY process (cur_pid=-1)
    * so the idle's proc[0].tcb.sp remains untouched by PendSV until
    * our first context_switch from idle.
    */
   proc_set_current_pid(-1);
   return (int)pid; // Must be 0 (idle's pid)
}

/*!
 * \brief Start the kernel's scheduler and never returns.
 * - Permit malloc in the kernel's heap. This destroys the first stack.
 * - Set boot flag so new process will wait for __malloc_lock
 * - Triggers the first context_switch.
 */
void krun (void)
{
   kinit_SysTick (); // Configure and start SysTick

   /*
    * We have now SysTick, so we Permit malloc, set boot flag and trigger PendSV
    * for the first context_switch.
    */
   __malloc_unlock ();
   set_al_boot ();
   __pendsv_trig();  // PendSV request
   while (1)   // Stay here until OS starts.
      ;
   /*
    * \info
    * The kernel will not return to this thread again.
    * If there is no process to run it will force the proc_idle().
    * proc_idle() has its own stack (a.k.a kernel's stack) by
    * kinit (KERNELS_SIZE, clk, os_f) call.
    */
}
