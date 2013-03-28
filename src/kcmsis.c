/*
 * kcmsis.c : This file is part of pkernel
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

#include "kcmsis.h"

/* ===============  Compiler specific Intrinsics  =============== */


#if defined ( __CC_ARM   ) /*------------------RealView Compiler -----------------*/
/* ARM armcc specific functions */

/*!
 * \brief  Set the Base Priority value
 * \param  basePri  BasePriority
 */
__ASM void __kset_BASEPRI(uint32_t basePri)
{
  msr basepri, r0
  bx lr
}

#elif (defined (__ICCARM__)) /*------------------ ICC Compiler -------------------*/
/* IAR iccarm specific functions */
#pragma diag_suppress=Pe940

/*!
 * \brief  Set the Base Priority value
 * \param  basePri  BasePriority
 */
__ASM void __kset_BASEPRI(uint32_t basePri)
{
  msr basepri, r0
  bx lr
}

#pragma diag_default=Pe940

#elif (defined (__GNUC__)) /*------------------ GNU Compiler ---------------------*/
/* GNU gcc specific functions */

/*!
 * \brief Set the Base Priority value.
 * \param value  BasePriority
 */
void __kset_BASEPRI(uint32_t value)
{
  __ASM volatile (
        "MSR basepri, %0   \n\t"
        "BX lr             \n\t" : : "r" (value) );
}

#elif (defined (__TASKING__)) /*------------------ TASKING Compiler ---------------------*/
/* TASKING carm specific functions */

/*
 * The CMSIS functions have been implemented as intrinsics in the compiler.
 * Please use "carm -?i" to get an up to date list of all instrinsics,
 * Including the CMSIS ones.
 */

#endif
