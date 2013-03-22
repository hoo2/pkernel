/*
 * sched.h : This file is part of pkernel
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


#ifndef __sched_h__
#define __sched_h__

#include "proc.h"

int schedule(void);
process_t* sch_alarm (void);
void sch_add_proc(int pid);
void sch_remove_proc (int pid);

/* list operations */
void sch_list_ins_back(proc_list_t *list, process_t *proc);
void sch_list_ins_front(proc_list_t *list, process_t *proc);
void sch_list_remove(proc_list_t *list, process_t *proc);
void sch_susp_proc (process_t *p);
int sch_runq_empty (void);
int sch_susq_empty (void);
int sch_empty_list (proc_list_t *l);

#endif //#ifndef __sched_h__

