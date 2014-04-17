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

#include <kcmsis.h>

/* ===============  Compiler specific Intrinsics  =============== */

/*!
 * \brief Set the Base Priority value.
 * \param value  BasePriority
 */
void __kset_BASEPRI(uint32_t value)
{
  __asm volatile (
        "MSR basepri, %0   \n\t"
        "BX lr             \n\t" : : "r" (value) );
}

/*!
 * \brief  Get the priority mask bit in the priority mask register
 * \return PRIMASK value
 */
uint32_t __kget_PRIMASK(void)
{
  uint32_t res=0;
  __asm volatile ("MRS %0, primask  \n\t"
                  "MOV r0, %0       \n\t"
                  "BX  lr           \n\t" : "=r" (res) );
  return res;
}

/*!
 * \brief  Set the priority mask bit in the priority mask register
 * \param  pmsk  PRIMASK value
 */
void __kset_PRIMASK(uint32_t pmsk)
{
   __asm volatile ("MSR primask, %0 \n\t"
                   "BX  lr          \n\t" : : "r" (pmsk) );
}

/*!
 * \brief  Return the content of the fault mask register
 * \return FaultMask
 */
uint32_t __kget_FAULTMASK(void)
{
  uint32_t res=0;

  __asm volatile ("MRS %0, faultmask \n\t"
                  "MOV r0, %0        \n\t"
                  "BX  lr            \n\t" : "=r" (res) );
  return res;
}

/*!
 * \brief  Set the fault mask register
 * \param  fmsk  faultMask value
 */
void __kset_FAULTMASK(uint32_t fmsk)
{
   __asm volatile ("MSR faultmask, %0 \n\t"
                   "BX  lr            \n\t" : : "r" (fmsk) );
}

/*!
 * \brief  Returns true if we are in privilege mode
 */
uint8_t __kPrivilege(void)
{
   uint32_t  r;

   __asm volatile ("MRS %0, control \n\t" : "=r" (r) );
   r &= 0x01;
   return (!r);
}

/**
 * @brief  Return the Main Stack Pointer
 *
 * @return Main Stack Pointer
 *
 * Return the current value of the MSP (main stack pointer)
 * Cortex processor register
 */

uint32_t __kget_MSP(void)
{
  uint32_t result=0;

  __asm volatile ("MRS %0, msp\n\t"
                  "MOV r0, %0 \n\t"
                  "BX  lr     \n\t"  : "=r" (result) );
  return(result);
}
