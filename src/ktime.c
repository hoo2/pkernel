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

clock_t  volatile Ticks;               /*!< CPU time */
time_t   volatile Now;                 /*!< Time in unix secs past 1-Jan-70 */

static kclock_t  volatile kclock;      /*!< The kernel's "knowledge" of cpu clock */
static kclock_t  volatile os_freq;     /*!< The kernels frequency */



/*!
 * Initialize and Start the SysTick with the kclock and os_freq
 */
void kinit_SysTick (void)
{
   // Time base configuration and enable
   kSysTick->LOAD = (kclock / 8) / os_freq;
   kSysTick->CTRL |= kSysTick_CTRL_ENABLE_Msk + kSysTick_CTRL_TICKINT_Msk;
}

/*!
 * \brief Get the pkernel's knowledge of os frequency.
 * \return OS Frequency
 */
__INLINE kclock_t kget_os_freq (void){
   return os_freq;
}

/*!
 * \brief Set the pkernel's os frequency (Update his knowledge).
 * \param f OS Frequency.
 */
__INLINE void kset_os_freq (kclock_t f){
   os_freq = f;
}

/*!
 * \brief Get the pkernel's knowledge of CPU frequency.
 * \return CPU Frequency
 */
__INLINE kclock_t kget_clock (void){
   return kclock;
}

/*!
 * \brief Set the pkernel's knowledge of CPU freq.
 * \param f CPU Frequency.
 */
__INLINE void kset_clock (kclock_t clk){
   kclock = clk;
}

/*!
 * \brief Reconfigure the SysTick with the kclock and os_freq
 * \warning Use kset_os_freq() and kset_clock() before.
 */
void kupdate_SysTick (void)
{
   /* Time base configuration */
   kSysTick->LOAD = (kclock / 8) / os_freq;
}


/*!
 * \brief Determines the processor time used.
 * Returns the implementation's best approximation to the processor time
 * used by the program since program invocation. The time in
 * seconds is the value returned divided by the value of the macro
 * CLK_TCK or CLOCKS_PER_SEC
 *
 * \param None
 * \return Current value of Ticks \sa Ticks
 */
__INLINE clock_t clock (void)
{
   return (clock_t) Ticks;
}

/*!
 * \brief Determines the current calendar time. The encoding of the value is
 * in unix secs past 1-Jan-70. Returns the implementations best approximation
 * to the current calendar time. If timer is not a null pointer, the return value
 * is also assigned to the object it points to.
 *
 * \param timer Pointer to time_t struct
 * \return Current value of Now \sa Now
 */
time_t time (time_t *timer)
{
   if (timer)
      *timer = (time_t)Now;
   return (time_t)Now;
}

