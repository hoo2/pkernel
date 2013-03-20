/*
 * alloc.h : This file is part of pkernel
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


#ifndef __alloc_h__
#define __alloc_h__

#include "pkdefs.h"
#include "sem.h"

extern void _eram;                        /* End address for ram */
extern unsigned long pulStack[];

typedef  char *   caddr_t;

typedef enum
{
   MA_BOTTOM=-1, MA_BLOCK, MA_TOP, MA_UNUSED
}al_flag_t;

typedef
volatile struct
{
   void*       mem_ptr;
   size_t      sz;
   al_flag_t   flag;
}al_t;

typedef enum
{
   AL_STACK=0, AL_HEAP
}al_mem_type_t;

void* m_al (size_t sz, al_mem_type_t mt);
void alloc_init (void);

void __malloc_lock (void);    // Spin locks
void __malloc_unlock (void);

caddr_t _sbrk ( int incr );
void *malloc (size_t __size);
void free (void* p);
void *calloc(size_t N, size_t __size);
void *realloc (void * __r, size_t __size);




#endif //#ifndef __alloc_h__

