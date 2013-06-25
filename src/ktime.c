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

#include <ktime.h>

static clock_t  volatile kcpu_clk;     /*!< The kernel's "knowledge" of cpu clock */
static clock_t  volatile kfreq;        /*!< The kernels frequency */


/*!
 * Initialize and Start the SysTick with the kcpu_clk and kfreq
 */
void kinit_SysTick (void)
{
   // Time base configuration and enable
   kSysTick->LOAD = (kcpu_clk / 8) / kfreq;
   kSysTick->CTRL |= kSysTick_CTRL_ENABLE_Msk + kSysTick_CTRL_TICKINT_Msk;
}

/*!
 * \brief Get the pkernel's knowledge of os frequency.
 * \return OS Frequency
 */
inline clock_t get_freq (void) {
   return kfreq;
}

/*!
 * \brief Set the pkernel's knowledge of os frequency.
 * \param   f OS Frequency
 *
 * \note
 *    This function does not update theSusTick. Use \sa update_freq() instead.
 */
inline void set_freq (clock_t f) {
   kfreq = f;
}

/*!
 * \brief
 *    Set the pkernel's os frequency (Update his knowledge).
 *    Also reconfigure the SysTick with the new kfreq.
 * \param   f  OS Frequency.
 */
void update_freq (clock_t f)
{
   if (kfreq != f)
   {
      kfreq = f;
      kSysTick->LOAD = (kcpu_clk / 8) / kfreq;
   }
}

/*!
 * \brief Get the pkernel's knowledge of CPU frequency.
 * \return CPU Frequency
 */
inline clock_t get_clock (void) {
   return kcpu_clk;
}

/*!
 * \brief Set the pkernel's knowledge of os frequency.
 * \param   f OS Frequency
 *
 * \note
 *    This function does not update theSusTick. Use \sa update_freq() instead.
 */
inline void set_clock (clock_t clk) {
   kcpu_clk = clk;
}

/*!
 * \brief
 *    Set the pkernel's knowledge of CPU freq.
 *    Also reconfigure the SysTick with the new kcpu_clk.
 * \param f CPU Frequency.
 */
void update_clock (clock_t clk)
{
   if (kcpu_clk != clk)
   {
      kcpu_clk = clk;
      kSysTick->LOAD = (kcpu_clk / 8) / kfreq;
   }
}

/*!
 * \brief
 *  determines the processor time used.
 * \return
 *  the implementation's best approximation to the processor time
 *  used by the program since program invocation. The time in
 *  seconds is the value returned divided by the value of the macro
 *  CLK_TCK or CLOCKS_PER_SEC
 */
inline clock_t clock (void)
{
   return (clock_t) Ticks;
}

/*!
 * \brief
 *  determines the current calendar time. The encoding of the value is
 *  unspecified.
 * \return
 *  The implementations best approximation to the current calendar
 *  time. If timer is not a null pointer, the return value
 *  is also assigned to the object it points to.
 */
time_t time (time_t *timer)
{
   if (timer)
      *timer = (time_t)Now;
   return (time_t)Now;
}

