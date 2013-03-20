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

typedef volatile union
{
   struct
   {
      uint8_t  os       :1;
      uint8_t  suspend  :1;
      uint8_t  res      :6;
   }flag;
   uint8_t  flags;
}os_command_t;

typedef enum
{
   OS_TRIG=0, OS_SUSPEND
}os_command_enum_t;

/*
 * XXX: Do not change these values. OS runs in the
 * lowest priority level and PendSV is 15, so it always
 * run last and call BX 0xFFFFFFF9 to return in process.
 * NOT in ISR.
 */
#define OS_PENDSV_PRI      (0x0F)
#define OS_SYSTICK_PRI     (0x0E)

#define OS_HALT_ISR()      (__kset_BASEPRI (OS_SYSTICK_PRI))
#define OS_RESUME_ISR()    (__kset_BASEPRI (OS_SYSTICK_PRI))

void SysTick_Handler(void);// __attribute__( ( naked ) );
void PendSV_Handler(void) __attribute__( ( naked ) );
void init_timer (void);

void OS_Call (process_t *p, os_command_enum_t cmd);

void sleep (clock_t t);
void wait (sem_t *s);
void signal (sem_t *s);
void lock (sem_t *s);
void unlock (sem_t *m);

#endif //#ifndef __os_h__

