/*
 * pkdefs.h : This file is part of pkernel
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

#ifndef	__pkdefs_h__
#define	__pkdefs_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <ktime.h>
#include <stddef.h>

#ifndef __cplusplus
#include <stdatomic.h>
#define pkernel_atomic  _Atomic
#else
#define pkernel_atomic
//^ We leave it blank to prevent C++ compilation errors when the header is in scope.
//
//  All the pkernel files should compile with gcc. If C++ compilation units include
//  this header they see pkernel_atomic blank. The type remains binary compatible
//  so no extra effort is required.
#endif

/* =================== User Defines ===================== */

#define  MAX_PROC                (0x10)   /*!< The maximum number of Process supported by pkernel. */
#define  MAX_HEAP_ALLOCS         (0x20)   /*!< The maximum number of Heap allocations supported by pkernel.*/





/* ================     General Defines       ======================*/
#define  ALLOC_SIZE                       (MAX_HEAP_ALLOCS+MAX_PROC)
#define  IDLE_STACK_SIZE                  (96)  // [bytes]



/* =================== Data types ===================== */

/*!
 * Semaphore data type
 */
typedef struct sem {
    pkernel_atomic int val;   /*!< Semaphore value. */
}sem_t;

/*!
 * Hardware stack frame.
 * This is a clone of the stack frame used by NVIC
 */
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

/*!
 * Software stack frame.
 * This is a clone of the stack frame used by pkernel.
 */
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


/*!
 * Task Control Block
 */
typedef struct
{
   uint32_t sp_tip;     /*!< Memory pointer returned by allocator. */
   uint32_t sp;         /*!< Current SP. */
}proc_tcb_t;

typedef int pid_t;      /*!< Process ID type. */

typedef void (*process_ptr_t) (void);  /*!< The Process Function type. */

/*!
 * Process data type
 */
typedef struct process
{
   pid_t          id;      /*!< The process id. */
   process_ptr_t  fptr;    /*!< The function where the process came from */

   int8_t         is;      /*!< process exists flag. */
   uint8_t        pr;      /*!< Privilege flag */

   int            time_slice;
   int8_t         nice;    /*!< Gives priority level -10..10. */
   int8_t         fit;     /*!< Gives time slice level -10..10. */

   clock_t        alarm;   /*!< If suspend this is the alarm. */
   sem_t          *sem;    /*!< If suspend this is the semaphore. */
   proc_tcb_t     tcb;

   struct process *next, *prev;  /*!< Used by runq and susq lists. */
}process_t;

/*!
 * Linked list type
 */
typedef struct proc_list
{
   process_t *head;
   process_t *tail;
}proc_list_t;

typedef void (*service_t) (void);
   /*!< Pointer to void function (void) to use as service */

/*!
 * Type for service item
 */
typedef struct service_item
{
   service_t      fptr;
   clock_t        every;
   struct service_item  *prev, *next;
}service_item_t;


typedef struct service_list
{
   service_item_t *head;
   service_item_t *tail;
}service_list_t;

/*!
 * Type for cron item
 */
typedef struct cron
{
   process_ptr_t  fptr;
   size_t         ms;
   int8_t         nice, fit;
   uint8_t        pr;
   time_t         at;
   time_t         every;
   struct cron    *prev, *next;
}cron_t;

typedef struct cron_list
{
   cron_t *head;
   cron_t *tail;
}cron_list_t;

typedef enum
{
   IDLE_RUN=0,
   IDLE_SLEEP,
   IDLE_STOP
}idle_mode_en;


typedef struct kernel_vars {
    idle_mode_en    idle_mode;
    int             def_time_slice;
    pid_t           cur_pid;    /*!< pid of the currently executing process. The idle process's cur_pid is 0.*/
    pid_t           last_pid;   /*!< pid of the last real process that was running, this should never become 0. */
    volatile uint8_t prock;     /*!< proc lock flag. Set proc table is used. */
    volatile uint8_t cron_stretch;
    volatile uint8_t service_lock;
    volatile uint8_t enable;    /*!< pkernel enable flag */
}kernel_var_t;
extern kernel_var_t   kernel_vars;

typedef void (*callback_t) (void);

typedef struct callbacks
{
   callback_t  presleep;
   callback_t  postsleep;
   callback_t  prestop;
   callback_t  poststop;
}callbacks_t;


#ifdef __cplusplus
}
#endif

#endif //#ifndef	__pkdefs_h__

