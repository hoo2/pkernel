/*
 * ktime.h : This file is part of pkernel
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

#ifndef __ktime_h__
#define __ktime_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <kcmsis.h>
#include <time.h>

/* =================== Data types ===================== */

/*
 * Also defined in types.h
 */
//#ifndef  _CLOCK_T_
//#define  _CLOCK_T_   unsigned long     /* clock() */
//#endif
//#ifndef _TIME_T_
//#define  _TIME_T_ long                 /* time() */
//#endif
//
//typedef _CLOCK_T_ clock_t;             /*!< CPU time type */
//typedef _TIME_T_ time_t;               /*!< date/time in unix secs past 1-Jan-70 type for 68 years*/

extern clock_t  volatile Ticks;        /*!< CPU time */
extern time_t   volatile Now;          /*!< time in unix secs past 1-Jan-70 */

typedef time_t (*ext_time_ft) (time_t *);    /*!< Pointer type for External time function. */
typedef int (*ext_settime_ft) (const time_t *); /*!< Pointer type for External set time function. */

extern ext_time_ft _ext_time;          //!< Pointer to External time callback function
extern ext_settime_ft _ext_settime;    //!< Pointer to External set time callback function

/*
 * ========= Set Functions ============
 */
void kset_rtc_time (ext_time_ft f);
void kset_rtc_settime (ext_settime_ft f);

/* =================== Exported Functions ===================== */

void kinit_SysTick (void);

clock_t get_clock (void);
void set_clock (clock_t clk);
void update_clock (clock_t clk);

clock_t get_freq (void);
void set_freq (clock_t f);
void update_freq (clock_t f);


clock_t clock (void);
time_t time(time_t * /*timer*/);
int settime (const time_t *t);

#ifdef __cplusplus
}
#endif

#endif

