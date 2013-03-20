/*
 * pkdefs.h : This file is part of pkernel
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

#ifndef	__pkdefs_h__
#define	__pkdefs_h__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdlib.h>
#include <stdint.h>
#include "ktime.h"

/* =================== User Defines ===================== */

#define  SYSTEM_RTT_MAXSIZE               (5)   // Maximum number or real time tasks
#define  MAX_PROC                         (8)
#define  MAX_SEMAPHORES                   (6)
#define  MAX_HEAP_ALLOCS                  (8)

/* ================     General Defines       ======================*/
    
#define  ALLOC_SIZE                       (MAX_HEAP_ALLOCS+MAX_PROC)





/* =================== Data types ===================== */

typedef enum
{
   EXIT_OK,
   EXIT_ERROR
}exit_t;

typedef volatile struct
{
   int val;
   uint8_t en:1;
}sem_t;

typedef struct
{
   uint32_t r0;
   uint32_t r1;
   uint32_t r2;
   uint32_t r3;
   uint32_t r12;
   uint32_t lr;
   uint32_t pc;
   uint32_t psr;
}hw_stack_frame_t;

typedef struct
{
   uint32_t r4;
   uint32_t r5;
   uint32_t r6;
   uint32_t r7;
   uint32_t r8;
   uint32_t r9;
   uint32_t r10;
   uint32_t r11;
}sw_stack_frame_t;


typedef struct
{
   uint32_t sp_tip;
   uint32_t sp;
}proc_tcb_t;

typedef struct process
{
   int            id;      // the process id
   int8_t         is;      // Used as flag that process exists.
   int            exit_status;

   int            ticks_left;
   int8_t         nice;    // gives priority level -10..10
   int8_t         fit;     // gives time slice level -10..10

   clock_t        alarm;   // If suspend this is the alarm
   sem_t          *sem;    // If suspend this is the semaphore
   proc_tcb_t     tcb;
   
   struct process *next, *prev;  // Used for both runq and susq
}process_t;

typedef int (*process_ptr_t) (void);

typedef struct proc_list
{
   process_t *head;
   process_t *tail;
}proc_list_t;

#ifdef __cplusplus
}
#endif

#endif //#ifndef	__pkdefs_h__

