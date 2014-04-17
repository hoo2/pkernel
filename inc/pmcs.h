/*
 * pmcs.h : This file is part of pkernel
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
#ifndef __pmcs_h__
#define __pmcs_h__

#ifdef __cplusplus
 extern "C" {
#endif

#include <pkdefs.h>
#include <kcmsis.h>

void sleepmode (void);
void stopmode (void);
void servicemode (void);
void applicationmode (void);

void set_presleep (callback_t fptr);
void set_postsleep (callback_t fptr);
void set_prestop (callback_t fptr);
void set_poststop (callback_t fptr);

#ifdef __cplusplus
 }
#endif

#endif   //#ifndef __pmcs_h__
