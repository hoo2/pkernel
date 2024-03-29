/*
 * sem.h : This file is part of pkernel
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


#ifndef __sem_h__
#define __sem_h__

#include <sched.h>

void sem_init (sem_t* s, int v);
int  sem_close (sem_t *s);
int  sem_getvalue (sem_t *s);
int  sem_check (sem_t *s);

void mut_init (sem_t* m);
int  mut_close (sem_t *m);
int  mut_trylock (sem_t *m);

#endif //#ifndef __sem_h__

