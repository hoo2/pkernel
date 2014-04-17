/*
 * kcmsis.h : This file is part of pkernel
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

#ifndef  __kcmsis_h__
#define  __kcmsis_h__

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef __CORTEX_M
#define __CORTEX_M                (0x03)      /*!< Cortex core */
#endif

#include <stdint.h>                           /* Include standard types */

/*!
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#ifdef __cplusplus
#define     __I     volatile                /*!< defines 'read only' permissions      */
#else
#define     __I     volatile const          /*!< defines 'read only' permissions      */
#endif

#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */



/*******************************************************************************
 *                 Register Abstraction
 ******************************************************************************/

#define __kNVIC_PRIO_BITS     (4)

#define  kPendSV_IRQn         (-2)
#define  kSysTick_IRQn        (-1)


/*!
 * struct to access SCB registers
 */
typedef struct
{
  __I  uint32_t CPUID;                        /*!< Offset: 0x00  CPU ID Base Register                                  */
  __IO uint32_t ICSR;                         /*!< Offset: 0x04  Interrupt Control State Register                      */
  __IO uint32_t VTOR;                         /*!< Offset: 0x08  Vector Table Offset Register                          */
  __IO uint32_t AIRCR;                        /*!< Offset: 0x0C  Application Interrupt / Reset Control Register        */
  __IO uint32_t SCR;                          /*!< Offset: 0x10  System Control Register                               */
  __IO uint32_t CCR;                          /*!< Offset: 0x14  Configuration Control Register                        */
  __IO uint8_t  SHP[12];                      /*!< Offset: 0x18  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IO uint32_t SHCSR;                        /*!< Offset: 0x24  System Handler Control and State Register             */
  __IO uint32_t CFSR;                         /*!< Offset: 0x28  Configurable Fault Status Register                    */
  __IO uint32_t HFSR;                         /*!< Offset: 0x2C  Hard Fault Status Register                            */
  __IO uint32_t DFSR;                         /*!< Offset: 0x30  Debug Fault Status Register                           */
  __IO uint32_t MMFAR;                        /*!< Offset: 0x34  Mem Manage Address Register                           */
  __IO uint32_t BFAR;                         /*!< Offset: 0x38  Bus Fault Address Register                            */
  __IO uint32_t AFSR;                         /*!< Offset: 0x3C  Auxiliary Fault Status Register                       */
  __I  uint32_t PFR[2];                       /*!< Offset: 0x40  Processor Feature Register                            */
  __I  uint32_t DFR;                          /*!< Offset: 0x48  Debug Feature Register                                */
  __I  uint32_t ADR;                          /*!< Offset: 0x4C  Auxiliary Feature Register                            */
  __I  uint32_t MMFR[4];                      /*!< Offset: 0x50  Memory Model Feature Register                         */
  __I  uint32_t ISAR[5];                      /*!< Offset: 0x60  ISA Feature Register                                  */
} kSCB_Type;

#define kSCB_ICSR_PENDSVSET_Pos             28                                             /*!< SCB ICSR: PENDSVSET Position */
#define kSCB_ICSR_PENDSVSET_Msk             (1ul << kSCB_ICSR_PENDSVSET_Pos)                /*!< SCB ICSR: PENDSVSET Mask */

#define kSCB_SHCSR_PENDSVACT_Pos            10                                             /*!< SCB SHCSR: PENDSVACT Position */
#define kSCB_SHCSR_PENDSVACT_Msk            (1ul << kSCB_SHCSR_PENDSVACT_Pos)               /*!< SCB SHCSR: PENDSVACT Mask */

/* SCB Application Interrupt and Reset Control Register Definitions */
#define kSCB_AIRCR_VECTKEY_Pos              16                                             /*!< SCB AIRCR: VECTKEY Position */
#define kSCB_AIRCR_VECTKEY_Msk              (0xFFFFul << kSCB_AIRCR_VECTKEY_Pos)            /*!< SCB AIRCR: VECTKEY Mask */

#define kSCB_AIRCR_PRIGROUP_Pos              8                                             /*!< SCB AIRCR: PRIGROUP Position */
#define kSCB_AIRCR_PRIGROUP_Msk             (7ul << kSCB_AIRCR_PRIGROUP_Pos)                /*!< SCB AIRCR: PRIGROUP Mask */

#define kSCB_AIRCR_SYSRESETREQ_Pos           2                                             /*!< SCB AIRCR: SYSRESETREQ Position */
#define kSCB_AIRCR_SYSRESETREQ_Msk          (1ul << kSCB_AIRCR_SYSRESETREQ_Pos)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define kSCB_SHCSR_SYSTICKACT_Pos           11                                             /*!< SCB SHCSR: SYSTICKACT Position */
#define kSCB_SHCSR_SYSTICKACT_Msk           (1ul << kSCB_SHCSR_SYSTICKACT_Pos)              /*!< SCB SHCSR: SYSTICKACT Mask */


/*!
 * struct to access SysTick registers
 */
typedef struct
{
  __IO uint32_t CTRL;                         /*!< Offset: 0x00  SysTick Control and Status Register */
  __IO uint32_t LOAD;                         /*!< Offset: 0x04  SysTick Reload Value Register       */
  __IO uint32_t VAL;                          /*!< Offset: 0x08  SysTick Current Value Register      */
  __I  uint32_t CALIB;                        /*!< Offset: 0x0C  SysTick Calibration Register        */
} kSysTick_Type;

/* SysTick Control / Status Register Definitions */
#define kSysTick_CTRL_COUNTFLAG_Pos         16                                             /*!< SysTick CTRL: COUNTFLAG Position */
#define kSysTick_CTRL_COUNTFLAG_Msk         (1ul << kSysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */

#define kSysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define kSysTick_CTRL_CLKSOURCE_Msk         (1ul << kSysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define kSysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */
#define kSysTick_CTRL_TICKINT_Msk           (1ul << kSysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define kSysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define kSysTick_CTRL_ENABLE_Msk            (1ul << kSysTick_CTRL_ENABLE_Pos)               /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define kSysTick_LOAD_RELOAD_Pos             0                                             /*!< SysTick LOAD: RELOAD Position */
#define kSysTick_LOAD_RELOAD_Msk            (0xFFFFFFul << kSysTick_LOAD_RELOAD_Pos)        /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define kSysTick_VAL_CURRENT_Pos             0                                             /*!< SysTick VAL: CURRENT Position */
#define kSysTick_VAL_CURRENT_Msk            (0xFFFFFFul << kSysTick_VAL_CURRENT_Pos)        /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define kSysTick_CALIB_NOREF_Pos            31                                             /*!< SysTick CALIB: NOREF Position */
#define kSysTick_CALIB_NOREF_Msk            (1ul << kSysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define kSysTick_CALIB_SKEW_Pos             30                                             /*!< SysTick CALIB: SKEW Position */
#define kSysTick_CALIB_SKEW_Msk             (1ul << kSysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define kSysTick_CALIB_TENMS_Pos             0                                             /*!< SysTick CALIB: TENMS Position */
#define kSysTick_CALIB_TENMS_Msk            (0xFFFFFFul << kSysTick_VAL_CURRENT_Pos)        /*!< SysTick CALIB: TENMS Mask */






/* Memory mapping of Cortex-M3 Hardware */
#define kSCS_BASE           (0xE000E000)                              /*!< System Control Space Base Address */
#define kSysTick_BASE       (kSCS_BASE +  0x0010)                     /*!< SysTick Base Address              */
#define kSCB_BASE           (kSCS_BASE +  0x0D00)                     /*!< System Control Block Base Address */

#define kSCB                ((kSCB_Type *)          kSCB_BASE)        /*!< SCB configuration struct          */
#define kSysTick            ((kSysTick_Type *)      kSysTick_BASE)    /*!< SysTick configuration struct      */





/*******************************************************************************
 *                Hardware Abstraction Layer
 ******************************************************************************/

#define __kWFI()   __asm volatile ( "WFI    \n\t" )


void __kset_BASEPRI(uint32_t value)  __attribute__( ( naked ) );

uint32_t __kget_PRIMASK(void) __attribute__( ( naked ) );
void __kset_PRIMASK(uint32_t primask) __attribute__( ( naked ) );

uint32_t __kget_FAULTMASK(void) __attribute__( ( naked ) );
void __kset_FAULTMASK(uint32_t fmsk) __attribute__( ( naked ) );

uint8_t __kPrivilege(void);

/*
static __INLINE void __DSB() {
   __ASM volatile ("dsb");
}
*/

/*  ==================   NVIC functions  ================== */



/*!
 * \brief  Set the priority for an interrupt
 * Set the priority for the specified interrupt. The interrupt
 * number can be positive to specify an external (device specific)
 * interrupt, or negative to specify an internal (core) interrupt.
 *
 * Note: The priority cannot be set for every core interrupt.
 *
 * \param  IRQn      The number of the interrupt for set priority
 * \param  priority  The priority to set
 *
 */
static inline void kSetPriority(int32_t IRQn, uint32_t priority)
{
   // set Priority for Cortex-M3 System Interrupts
   kSCB->SHP[((uint32_t)(IRQn) & 0xF)-4] = ((priority << (8 - __kNVIC_PRIO_BITS)) & 0xff);
}

uint32_t __kget_MSP(void) __attribute__( ( naked ) );

/* ==================    Reset function  ================== */

/*
 * \brief  Initiate a system reset request.
 *
 * Initiate a system reset request to reset the MCU
 */
/*static __INLINE void kSystemReset(void)
{
  kSCB->AIRCR  = ((0x5FA << kSCB_AIRCR_VECTKEY_Pos)      |
                 (kSCB->AIRCR & kSCB_AIRCR_PRIGROUP_Msk) |
                 kSCB_AIRCR_SYSRESETREQ_Msk);                   // Keep priority group unchanged
  __DSB();                                                     // Ensure completion of memory access
  while(1);                                                    // wait until reset
}
*/

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef  __kcmsis_h__
