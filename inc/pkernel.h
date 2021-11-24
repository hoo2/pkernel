/*
 * pkernel.h : This file is part of pkernel
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
 *          kinit ((size_t)320, CLOCK, TICK_FREQ);
 *          knew (&pr_1, (size_t)320, 1, 0);
 *          knew (&pr_2, (size_t)320, 1, 0);
 *          krun ();
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

#include <os.h>

pid_t knew (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit);
int   kinit (size_t kmsize, clock_t clk, clock_t os_f);
void  krun (void);

extern clock_t clock (void);
extern time_t time (time_t *timer);

extern clock_t get_clock (void);
extern void    set_clock (clock_t clk);
extern void    update_clock (clock_t clk);
extern clock_t get_freq (void);
extern void    set_freq (clock_t f);
extern void    update_freq (clock_t f);

extern void sem_init (sem_t* s, int v);
extern int  sem_close (sem_t *s);
extern int  sem_getvalue (sem_t *s);
extern int  sem_check (sem_t *s);
extern void mut_init (sem_t* m);
extern int  mut_close (sem_t *m);
extern int  mut_trylock (sem_t *m);

extern void exit (int status);
extern void sleep (clock_t t);
extern void sem_wait (sem_t *s);
extern void sem_post (sem_t *s);
extern void mut_lock (sem_t *s);
extern void mut_unlock (sem_t *m);

extern void *malloc (size_t __size);
extern void free (void* p);
extern void *calloc (size_t N, size_t __size);
extern void *realloc (void * __r, size_t __size);

extern void service_add (service_t fptr, clock_t every);
extern void service_rem (service_t fptr);
extern void crontab (process_ptr_t fptr, size_t ms, int8_t nice, int8_t fit, uint8_t pr, time_t at, time_t every);
extern void crontab_r (process_ptr_t fptr);

extern void sleepmode (void);
extern void stopmode (void);
extern void servicemode (void);
extern void applicationmode (void);

extern void set_presleep (callback_t fptr);
extern void set_postsleep (callback_t fptr);
extern void set_prestop (callback_t fptr);
extern void set_poststop (callback_t fptr);

#ifdef __cplusplus
}
#endif

#endif //#ifndef	__pkernel_h__

