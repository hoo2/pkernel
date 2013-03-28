/*
 * pkernel.h : This file is part of pkernel
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

/*!
 * \mainpage pkernel
 *
 * \section desc_sec Description
 * pkernel is a very small multitasking kernel (RTOS) for Cortex M3/M4
 * microcontrollers.
 *
 * \section example_sec Example
 *
 *       #include "pkernel.h"
 *       void pr_1 (void)
 *       {
 *          // process 1 code
 *          while (1)
 *          {
 *             // usually inside a loop
 *          }
 *       }
 *       void pr_2 (void)
 *       {
 *          // process 2 code
 *          while (1)
 *          {
 *             // usually inside a loop
 *          }
 *       }
 *
 *       int main (void)
 *       {
 *          pkernel_boot ((size_t)320, CLOCK, TICK_FREQ);
 *          pkernel_newprocess (&pr_1, (size_t)320, 1, 0);
 *          pkernel_newprocess (&pr_2, (size_t)320, 1, 0);
 *          pkernel_run ();
 *          while (1);  // Unreachable.
 *       }
 *
 *
 */

#ifndef	__pkernel_h__
#define	__pkernel_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include "os.h"


pid_t pkernel_newprocess (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit);
int   pkernel_boot (size_t __kmsize, kclock_t clk, kclock_t os_f);
void  pkernel_run (void);


extern kclock_t kget_clock (void);
extern void     kset_clock (kclock_t clk);
extern kclock_t kget_os_freq (void);
extern void     kset_os_freq (kclock_t f);
extern void     kupdate_SysTick (void);

extern sem_t* sem_open(void);
extern sem_t* mut_open (void);
extern int    sem_close (sem_t *s);

extern void exit (int status);
extern void sleep (clock_t t);
extern void wait (sem_t *s);
extern void signal (sem_t *s);
extern void lock (sem_t *s);
extern void unlock (sem_t *m);

extern void *malloc (size_t __size);
extern void free (void* p);
extern void *calloc(size_t N, size_t __size);
extern void *realloc (void * __r, size_t __size);

#ifdef __cplusplus
}
#endif

#endif //#ifndef	__pkernel_h__

