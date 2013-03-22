/*
 * ktime.c : This file is part of pkernel
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

#include "ktime.h"

clock_t  volatile Ticks;               /* cpu time */
time_t   volatile Now;                 /* time in unix secs past 1-Jan-70 */

static kclock_t  volatile kclock;      /* the kernel's "knowledge" of cpu clock */
static kclock_t  volatile os_freq;     /* the kernels frequency */

__INLINE kclock_t kget_os_freq (void){
   return os_freq;
}

__INLINE void kset_os_freq (kclock_t f){
   os_freq = f;
}

__INLINE kclock_t kget_clock (void){
   return kclock;
}

__INLINE void kset_clock (kclock_t clk){
   kclock = clk;
}

void kinit_SysTick (void)
{
   /* Time base configuration and enable */
   kSysTick->LOAD = (kclock / 8) / os_freq;
   kSysTick->CTRL |= kSysTick_CTRL_ENABLE_Msk + kSysTick_CTRL_TICKINT_Msk;
}

void kupdate_SysTick (void)
{
   /* Time base configuration */
   kSysTick->LOAD = (kclock / 8) / os_freq;
}


/* Determines the processor time used.
 * Returns: the implementation's best approximation to the processor time
 *          used by the program since program invocation. The time in
 *          seconds is the value returned divided by the value of the macro
 *          CLK_TCK or CLOCKS_PER_SEC
 */

__INLINE clock_t clock (void)
{
   return (clock_t) Ticks;
}

/*
 * Determines the current calendar time. The encoding of the value is
 * in unix secs past 1-Jan-70.
 * Returns: the implementations best approximation to the current calendar
 *          time. If timer is not a null pointer, the return value
 *          is also assigned to the object it points to.
 */
time_t time (time_t * timer)
{
   if (timer)
      *timer = (time_t)Now;
   return (time_t)Now;
}

