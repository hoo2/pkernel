/*
 * pkernel.c : This file is part of pkernel
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

#include <pkernel.h>

kernel_var_t kernel_vars;

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
 * \brief Init the kernels' systick.
 * - Set priorities
 * - Set clock
 * - Set os frequency,
 */
void kinit_ticks (clock_t clk, clock_t os_f) {

   kSetPriority(kPendSV_IRQn, OS_PENDSV_PRI);
   kSetPriority(kSysTick_IRQn, OS_SYSTICK_PRI);

   set_clock (clk);     // Set kernel's knowledge for clocking and freq
   set_freq (os_f);

   kinit_SysTick ();
}

/*!
 * \brief Init the kernels' allocation system.
 * - Start allocator
 * - Create the idle process. Allocator though will
 * not give heap though before pkernel_run().
 *
 * \param kmsize    The pkernel's stack size for ISR and services
 *
 * \return 0(proc_idle's pid) on success.
 */

int kinit_allocation (size_t kmsize) {
    /*
    * We have now SysTick, so we initialize allocation, permit malloc, set boot flag
    * and trigger PendSV for the first context_switch.
    */
    alloc_init (kmsize); // Init the Stack allocation table.
    __malloc_unlock ();

    /*
     * \note
     * We make sure that we are outside off ANY process (cur_pid=-1)
     * so the idle's proc[0].tcb.sp remains untouched by PendSV until
     * our first context_switch.
     */
    proc_set_current_pid(-1);

    // Make the idle proc
    return (int)proc_newproc ((process_ptr_t)&proc_idle, IDLE_STACK_SIZE, 0, 0);
}

/*!
 * \brief Start the kernel's scheduler and never returns.
 * - Permit malloc in the kernel's heap. This destroys the first stack.
 * - Set boot flag so new process will wait for __malloc_lock
 * - Triggers the first context_switch.
 */
void krun (void) {

    set_al_boot ();
    kset_PSP (kget_MSP ());          // Prepare SPs
    kset_MSP ((uint32_t)&_estack);
    kernel_vars.enable = 1;          // enable pkernel SysTick handling
    __pendsv_trig();                 // PendSV request
    while (1)                        // Stay here until OS starts.
        ;
    /*
     * \info
     * The kernel will not return to this thread again.
     * If there is no process to run it will force the proc_idle().
     * proc_idle() has its own stack set by proc_newproc () call.
     */
}
