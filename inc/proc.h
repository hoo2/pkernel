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

#include <pkdefs.h>
#include <alloc.h>
#include <stddef.h>

#define IDLE_PROC_ID       (0)
#define TIMESLICE_TICKS    (10)

extern void exit (int status);

/*
 * Exported to lib
 */

// Spin locks
void __proc_lock (void);
 void __proc_unlock (void);
uint8_t __proc_state (void);

// Process Manipulation
void  proc_set_current_pid(pid_t pid);
pid_t proc_get_current_pid(void);
pid_t proc_search_pid (process_ptr_t fptr);
process_t *proc_get_current_proc(void);
process_t *proc_get_process(pid_t pid);

// API to os.c/h
void     context_switch(uint32_t s) __attribute__( ( naked ) );
void     proc_load_ctx(void) __attribute__( ( naked ) );
uint32_t proc_save_ctx(void) __attribute__( ( naked ) );
uint32_t proc_sel_stack (pid_t pid);
void     proc_store_stack_pointer (uint32_t sp);

// API to sched.c/h
void     proc_idle(void);
pid_t    proc_newproc (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit);
void     proc_exit (process_t *p);
void     proc_rst_ticks (pid_t pid);
void     proc_dec_ticks (pid_t pid);

#endif //#ifndef __proc_h__


