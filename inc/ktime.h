/*
 * ktime.h : This file is part of pkernel
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

#ifndef __ktime_h__
#define __ktime_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include "kcmsis.h"

/* =================== Data types ===================== */

/*
 * Also defined in types.h
 */
#ifndef  _CLOCK_T_
#define  _CLOCK_T_   unsigned long     /* clock() */
#endif
#ifndef _TIME_T_
#define  _TIME_T_ long                 /* time() */
#endif

typedef _CLOCK_T_ clock_t;             /*!< CPU time type */
typedef _TIME_T_ time_t;               /*!< date/time in unix secs past 1-Jan-70 type for 68 years*/

typedef  _CLOCK_T_ kclock_t;           /*!< The clock freq known by the kernel */

extern clock_t  volatile Ticks;        /*!< CPU time */
extern time_t   volatile Now;          /*!< time in unix secs past 1-Jan-70 */

/* =================== Exported Functions ===================== */

kclock_t kget_clock (void);
void kset_clock (kclock_t clk);
kclock_t kget_os_freq (void);
void kset_os_freq (kclock_t f);
void kinit_SysTick (void);
void kupdate_SysTick (void);


clock_t clock (void);
time_t time(time_t * /*timer*/);


#ifdef __cplusplus
}
#endif

#endif

