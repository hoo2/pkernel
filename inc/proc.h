/*
 * proc.h : This file is part of pkernel
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


#ifndef __proc_h__
#define __proc_h__

#include "pkdefs.h"
#include "alloc.h"

#define IDLE_PROC_ID       (0)
#define TIMESLICE_TICKS    (10)


/*
 * Exported to lib
 */
// Process Manipulation
void  proc_set_current_pid(int pid);
int   proc_get_current_pid(void);
process_t *proc_get_current_proc(void);
process_t *proc_get_process(int pid);

// API to os.c/h
void     context_switch(uint32_t s) __attribute__( ( naked ) );
uint32_t proc_load_ctx(void) __attribute__( ( naked ) );
uint32_t proc_save_ctx(void) __attribute__( ( naked ) );
uint32_t proc_sel_stack (int pid);
void     proc_store_stack_pointer (uint32_t sp);

// API to sched.c/h
exit_t   proc_init (void);
int      proc_newproc (process_ptr_t fptr, int8_t nice, int8_t fit, size_t mem);
int      proc_delproc (void);
void     proc_rst_ticks (int pid);
void     proc_dec_ticks (int pid);

#endif //#ifndef __proc_h__


