/*
 * cron.h : This file is part of pkernel
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
 * Date:       06/2013
 * Version:
 *
 */

#ifndef __cron_h__
#define __cron_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <pkdefs.h>
#include <alloc.h>
#include <stddef.h>

void services (void);
void cron (void);
uint8_t cron_stretching(void);

void service_add (service_t fptr, clock_t every);
void service_rem (service_t fptr);
void crontab (process_ptr_t fptr, size_t ms, int8_t nice, int8_t fit, uint8_t pr, time_t at, time_t every);
void crontab_r (process_ptr_t fptr);

extern pid_t knew (process_ptr_t fptr, size_t mem, int8_t nice, int8_t fit);

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __cron_h__
