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

#include "pkernel.h"


exit_t pkernel_newprocess (process_ptr_t fptr, int8_t nice, int8_t fit, uint16_t mem)
{
   int pid;
   OS_HALT_ISR();
   pid = proc_newproc(fptr, nice, fit, mem);
   if (pid != -1)
   {
      sch_add_proc (pid);
      OS_RESUME_ISR();
      return EXIT_OK;
   }
   OS_RESUME_ISR();
   return EXIT_ERROR;
}

exit_t  pkernel_boot (size_t __kmsize, kclock_t clk, kclock_t os_f)
{
   kSetPriority(PendSV_IRQn, OS_PENDSV_PRI);
   kSetPriority(SysTick_IRQn, OS_SYSTICK_PRI);

   kset_clock (clk);
   kset_os_freq (os_f);
   proc_init ();  // Init the Stack allocation table.
   proc_newproc ((process_ptr_t)&sch_idle_proc, 0, 0, __kmsize); // Make the idle proc
   /*
    * We make sure that we are outside off ANY process (pid = -1)
    * so the idle's proc[0].ctx.sp remains untouched by
    * OS's stack switching mechanism proc_store_stack_pointer().
    */
   proc_set_current_pid(-1);
   return EXIT_OK;
}

void pkernel_run (void)
{
   kinit_SysTick (); // Conf and start SysTick
   OS_Call ((void*)0, OS_TRIG);  // And jump to runq process

   while (1);  // Stay here until OS starts.
   /*
    * XXX: The kernel never returns to this thread again.
    * If there is no process it will force the sch_idle_proc()
    */
}
