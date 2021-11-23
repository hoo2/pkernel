/*
 * proc.c : This file is part of pkernel
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

#include <proc.h>

/*!
 * Hold the processes. pkernel creates a process in the first available slot in proc[].
 * The @runq and @susq are pointing to this array. \sa runq, susq
 */
static process_t proc[MAX_PROC];

//static pid_t cur_pid;   /*!< pid of the currently executing process. The idle process's cur_pid is 0.*/
//static pid_t last_pid;  /*!< pid of the last real process that was running, this should never become 0. */
//static uint8_t prock;   /*!< proc lock flag. Set proc table is used. */

/*!
 * \breif Spin lock to make thread safe the malloc functionality.
 * \param None
 * \return None
 */
void __proc_lock (void)
{
   while (kernel_vars.prock == 1)
      ;
   kernel_vars.prock = 1;
}

/*!
 * \breif malloc spin lock's unlock function.
 * \param None
 * \return None
 */
inline void __proc_unlock (void){
    kernel_vars.prock = 0;
}

/*!
 * \breif Reads malloc lock state
 * \return True if its locked
 */
inline uint8_t __proc_state (void) {
   return kernel_vars.prock;
}

/*!
 * \brief Push R4-R11 registers to MSP stack and returns the new SP
 *
 * \return The stack pointer
 */
uint32_t pproc_save_ctx (void) {
   uint32_t result;

   __asm volatile (
         "MRS r0, MSP         \n\t"
         "STMDB r0!, {r4-r11} \n\t"
         "MSR MSP, r0         \n\t"
         "MOV %0, r0          \n\t"
         "BX  lr              \n\t" : "=r" (result) );
   return result;
}

/*!
 * \brief Pop R4-R11 registers from MSP stack and update the new SP
 */
void pproc_load_ctx (void) {
   __asm volatile (
         "MRS r0, MSP         \n\t"
         "LDMFD r0!, {r4-r11} \n\t"
         "MSR MSP, r0         \n\t"
         "BX  lr              \n\t" );
}

/*!
 * \brief Push R4-R11 registers to stack and returns the new SP
 *
 * \return The stack pointer
 */
uint32_t proc_save_ctx (void)
{
   uint32_t result;

   __asm volatile (
         "MRS r0, PSP         \n\t"
         "STMDB r0!, {r4-r11} \n\t"
         "MSR PSP, r0         \n\t"
         "MOV %0, r0          \n\t"
         "BX  lr              \n\t" : "=r" (result) );
   return result;
}

/*!
 * \brief Pop R4-R11 registers from stack and update the new SP
 */
void proc_load_ctx (void)
{
   __asm volatile (
         "MRS r0, PSP         \n\t"
         "LDMFD r0!, {r4-r11} \n\t"
         "MSR PSP, r0         \n\t"
         "BX  lr              \n\t" );
}

uint32_t kget_MSP (void) {
    uint32_t r;

    __asm volatile (
        "MRS %0, MSP    \n\t"
        "MOV r0, %0     \n\t"
        "BX  lr         \n\t" : "=r" (r) );
    return r;
}

uint32_t kget_PSP (void) {
    uint32_t r;

    __asm volatile (
        "MRS %0, PSP    \n\t"
        "MOV r0, %0     \n\t"
        "BX  lr         \n\t" : "=r" (r) );
    return r;
}

void kset_MSP (uint32_t s) {
   __asm volatile (
         "MSR MSP, %0   \n\t"
         "BX  lr        \n\t" : : "r" (s) );
}

void kset_PSP (uint32_t s) {
   __asm volatile (
         "MSR PSP, %0   \n\t"
         "BX  lr        \n\t" : : "r" (s) );
}

/*!
 * \brief Return a SP if we have to do context switch.
 * \return The stack pointer of the new process, or 0
 */
uint32_t proc_sel_stack (pid_t pid)
{
   uint32_t stck = 0;
   process_t *new = proc + pid;

   if (kernel_vars.last_pid != pid)
      stck = new->tcb.sp;
   proc_set_current_pid (new->id);

   return stck;
}

/*!
 * \brief Save SP in the corresponding process tcb.
 * \param The SP.
 */
void proc_store_stack_pointer (uint32_t sp)
{
   process_t *cp;

   cp = proc_get_current_proc ();
   if (cp)
      cp->tcb.sp = sp;
}

/*!
 * \brief Set current pid.
 * \param The pid to set.
 */
inline void proc_set_current_pid (pid_t pid){
    kernel_vars.cur_pid = kernel_vars.last_pid = pid;
}

/*!
 * \brief Get current pid.
 * \return The current pid.
 */
inline pid_t proc_get_current_pid (void){
   return kernel_vars.cur_pid;
}

/*!
 * \brief Search for a process with the function pointer.
 * \return pid or -1 if there is none.
 */
pid_t proc_search_pid (process_ptr_t fptr)
{
   int i;

   for (i=0 ; i<MAX_PROC ; ++i)
      if (proc[i].is && proc[i].fptr == fptr)
         return proc[i].id;
   return -1;
}

/*!
 * \brief Get current process.
 * \return Pointer to the current process in proc[].
 */
process_t *proc_get_current_proc (void)
{
   return kernel_vars.cur_pid >= 0 ? &proc[kernel_vars.cur_pid] : 0;
}

/*!
 * \brief Get process from a pid. If the pid does not
 * exist, return 0.
 * \return Pointer to the process in proc[] or 0 on failure.
 */
process_t *proc_get_process (pid_t pid)
{
   process_t *p = proc + pid;
   if (p->id != pid)
      return 0;
   else
      return p;
}

/*!
 * \brief Reset time_slice parameter of the process with pid @a pid.
 * \param pid The pid of the process.
 */
void proc_rst_ticks (pid_t pid)
{
   process_t *p = proc + pid;

   if (p->fit >= 0)
      p->time_slice = (0.1*p->fit+1) * TIMESLICE_TICKS;
   else
      p->time_slice = (0.05*p->fit+1) * TIMESLICE_TICKS;

   if (p->time_slice < 1)
      p->time_slice = 1;
}

/*!
 * \brief Consume time of a process.
 * \param pid The pid of the process.
 */
void proc_dec_ticks (pid_t pid)
{
   process_t *p = proc + pid;
   p->time_slice--;
}

/*!
 * \brief The idle process. This process is forced from pkernel
 * if there is no other process in runq.
 */
void proc_idle (void)
{
   while (1)
      switch (kernel_vars.idle_mode)
      {
         default:
         case IDLE_RUN:
            break;
         case IDLE_SLEEP:
            sleepmode();
            break;
         case IDLE_STOP:
            stopmode();
            break;
      }
}

/*!
 * \brief Create new process.
 * - Allocate memory for process
 * - Prepare the proc[] table
 * - Prepare the stack frame so when the pkernel switches
 *   to process the POP functionality will set the CPU to the
 *   correct state.
 *
 * \param fptr Pointer to process function.
 * \param mem The size of process stack in bytes.
 * \param nice The nice ratio (-10 .. 10) of the process.
 * \param fit The fit (-10 .. 10) ratio of the process.
 * \return the process pid
 *
 */
pid_t proc_newproc (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit)
{
   int8_t i;
   pid_t pid = -1;
   uint32_t* pm = NULL;
   hw_stack_frame_t *pfrm;

   /* Find an empty slot in proc table */
   __proc_lock ();
   for (i = 0; i < MAX_PROC; ++i)
      if (!proc[i].is)
         break;

   /* Find available space in memory */
   if (!al_boot ())
      pm = (uint32_t*) m_al (mem, AL_STACK);
   else
   {
      __malloc_lock ();
      pm = (uint32_t*) m_al (mem, AL_STACK);
      __malloc_unlock ();
   }

   if (i >= MAX_PROC || pm == NULL)
      return pid;
   else
      pid = i;

   /* prepare the process before put it into runq */
   proc[pid].tcb.sp_tip = (uint32_t) pm;
   proc[pid].id = pid;
   proc[pid].fptr = fptr;
   proc[pid].is = 1;
   proc[pid].nice = nice;
   proc[pid].fit = fit;
   proc[pid].alarm = 0;
   proc[pid].sem = (void*) 0;
   proc_rst_ticks (pid);

   pfrm = (hw_stack_frame_t *) (proc[pid].tcb.sp_tip + mem - sizeof(hw_stack_frame_t));
   // Create the return frame in stack
   pfrm->r0 = pfrm->r1 = pfrm->r2 = pfrm->r3 = pfrm->r12 = 0;
   pfrm->pc = (uint32_t) fptr;
   pfrm->lr = (uint32_t) exit;
   pfrm->psr = 0x21000000; //default PSR value

   // Save the SP of the process
   proc[pid].tcb.sp = (uint32_t) pfrm - sizeof(sw_stack_frame_t);
   __proc_unlock ();
   return pid;
}

/*!
 * \brief Clear process from proc[] table and free up the stack memory
 * \param p Pointer to the process
 */
void proc_exit (process_t *p)
{
   p->is = 0;
   __malloc_lock ();
      m_fr ((void *) p->tcb.sp_tip);
   __malloc_unlock ();
}

