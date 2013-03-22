/*
 * proc.c : This file is part of pkernel
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


#include "proc.h"

static process_t proc[MAX_PROC];

/* cur_pid:  pid of the currently executing process.
 *           when we're in the idle process cur_pid will be 0.
 * last_pid: pid of the last real process that was running, this should
 *           never become 0. Essentially this defines the active kernel stack.
 */
static int cur_pid, last_pid;


uint32_t proc_save_ctx(void)
{
	uint32_t result=0;

   __asm volatile (
   "MRS r0, msp         \n\t"
   "STMDB r0!, {r4-r11} \n\t"
   "MSR msp, r0         \n\t"
   "MOV %0, r0          \n\t"
   "BX  lr              \n\t"
      : "=r" (result) );
	return result;
}

uint32_t proc_load_ctx(void)
{
	uint32_t result=0;
   __asm volatile (
   "MRS r0, msp         \n\t"
   "LDMFD r0!, {r4-r11} \n\t"
   "MSR msp, r0         \n\t"
   "MOV %0, r0          \n\t"
   "BX  lr              \n\t"
      : "=r" (result) );
   return result;
}


void context_switch(uint32_t s)
{
   __asm volatile (
   "MSR msp, %0  \n\t"
	"BX  lr       \n\t" : : "r" (s) );
}

uint32_t proc_sel_stack (int pid)
{
   static process_t *new;
   
   new = proc + pid;
   if(last_pid != pid)
   {
      proc_set_current_pid (new->id);
      return new->tcb.sp;
   }
   else
   {
      proc_set_current_pid(new->id);
      return 0;
   }
}

void proc_store_stack_pointer (uint32_t sp)
{
   process_t   *cp;

   cp = proc_get_current_proc ();   
   if (cp)
      cp->tcb.sp = sp;
}


__INLINE void proc_set_current_pid(int pid){
   cur_pid = last_pid = pid;
}
 
__INLINE int proc_get_current_pid(void){
   return cur_pid;
}
 
process_t *proc_get_current_proc(void)
{
   return cur_pid>=0 ? &proc[cur_pid]: 0;
}
 
process_t *proc_get_process(int pid)
{
   process_t *p = proc + pid;
   if(p->id != pid)
      return 0;
   else
      return p;
}

void proc_rst_ticks (int pid)
{
   process_t *p = proc + pid;

   if (p->fit >= 0)
      p->ticks_left = (0.1*p->fit + 1)*TIMESLICE_TICKS;
   else
      p->ticks_left = (0.05*p->fit + 1)*TIMESLICE_TICKS;

   if (p->ticks_left<1)
      p->ticks_left = 1;
}

void proc_dec_ticks (int pid)
{
   process_t *p = proc + pid;
   p->ticks_left--;
}



void proc_idle(void)
{
   while (1)
      ;
}

int proc_newproc (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit)
{
   int8_t  i, pid=-1;
   uint32_t* pm = NULL;
   hw_stack_frame_t  *pfrm;

   /* Find an empty slot in proc table */
   for (i=0 ; i<MAX_PROC ; ++i)
      if (!proc[i].is)
         break;

   /* Find available space in memory */
   if ( !al_boot () )
      pm = (uint32_t*)m_al (mem, AL_STACK);
   else
   {
      __malloc_lock ();
      pm = (uint32_t*)m_al (mem, AL_STACK);
      __malloc_unlock ();
   }

   if (i>=MAX_PROC || pm == NULL)
      return pid;
   else
      pid = i;

   /* prepare the process before put it into runq */
   proc[pid].tcb.sp_tip = (uint32_t)pm;
   proc[pid].id = pid;
   proc[pid].is = 1;
   proc[pid].nice = nice;
   proc[pid].fit = fit;
   proc[pid].alarm = 0;
   proc[pid].sem = (void*)0;
   proc_rst_ticks (pid);

   pfrm = (hw_stack_frame_t *)(proc[pid].tcb.sp_tip - sizeof(hw_stack_frame_t));
   // Create the return frame in stack
   pfrm->r0 = pfrm->r1 = pfrm->r2 = pfrm->r3 = pfrm->r12 = 0;
   pfrm->pc = (uint32_t) fptr;
   pfrm->lr = (uint32_t)proc_delproc;
   pfrm->psr = 0x21000000; //default PSR value

   // Save the SP of the process
   proc[pid].tcb.sp = (uint32_t)pfrm - sizeof(sw_stack_frame_t);

   return pid;
}

int proc_delproc (void)
{
   proc[cur_pid].is = 0;


   return 1;
}



