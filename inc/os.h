/*
 * os.h : This file is part of pkernel
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


#ifndef __os_h__
#define __os_h__


#include "pkdefs.h"
#include "sched.h"
#include "kcmsis.h"
#include "ktime.h"

/*!
 * Data structure used when a OS_Call is waiting the PendSV.
 *
 * XXX: We can "see" the request type when we are inside the PendSV
 * but we don't do anything for that yet.
 */
typedef volatile union
{
   struct
   {
      uint8_t  exit     :1;
      uint8_t  suspend  :1;
      uint8_t  res      :6;
   }flag;
   uint8_t  flags;
}os_command_t;

/*!
 * OS command used in OS_Call to pass the requested functionality
 * This provides a OS Call List.
 *
 * \sa OS_Call
 */
typedef enum
{
   OS_EXIT=0,     /*!< Indicates termination request of the current process. */
   OS_SUSPEND,    /*!< Indicates suspend request of the current process. */
}os_command_enum_t;

/*
 * XXX: Do not change these values. OS runs in the
 * lowest priority level and PendSV is 15, so it always
 * run last and call BX 0xFFFFFFF9 to return in process,
 * NOT in ISR.
 */
#define OS_PENDSV_PRI      (0x0F)
#define OS_SYSTICK_PRI     (0x0E)

#define __os_halt_ISR()    (__kset_BASEPRI (OS_SYSTICK_PRI))
#define __os_resume_ISR()  (__kset_BASEPRI (0))

#define __pendsv_act()     (kSCB->SHCSR & kSCB_SHCSR_PENDSVACT_Msk)
#define __systick_act()    (kSCB->SHCSR & kSCB_SHCSR_SYSTICKACT_Msk)

#define __pendsv_trig()    (kSCB->ICSR |= kSCB_ICSR_PENDSVSET_Msk)

/*
 * Exported Functions for inner use.
 */
void SysTick_Handler(void);// __attribute__( ( naked ) );
void PendSV_Handler(void) __attribute__( ( naked ) );
void init_timer (void);

void OS_Call (process_t *p, os_command_enum_t cmd);

/*
 * Exported Functions for userland
 */
void exit (int status);
void sleep (clock_t t);
void wait (sem_t *s);
void signal (sem_t *s);
void lock (sem_t *s);
void unlock (sem_t *m);

#endif //#ifndef __os_h__

