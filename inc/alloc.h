/*
 * alloc.h : This file is part of pkernel
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


#ifndef __alloc_h__
#define __alloc_h__

#include <pkdefs.h>
#include <sem.h>
#include <kcmsis.h>
#include <stddef.h>

extern unsigned long _estack;           /*!< End address for ram provided by ld script. */
extern unsigned long _ebss;             /*!< Ending address of the bss section provided by ld script. */

typedef  char *caddr_t;                   /*!< same as stdlib */

/*!
 * Allocation table flag type
 * Mark the type of al[] block.
 */
typedef enum
{
   MA_BOTTOM=-1,  /*!< Indicate a starting address block. */
   MA_BLOCK,      /*!< Indicate a used memory block of heap or stack. */
   MA_TOP,        /*!< Indicate a anding address block. */
   MA_UNUSED      /*!< Indicate a free al slot. */
}al_flag_t;

/*!
 * Allocation Table item
 */
typedef volatile struct
{
   void*       mem_ptr;    /*!< Pointer to memory block in ram. */
   size_t      sz;         /*!< Size of memory in bytes. */
   al_flag_t   flag;       /*!< The blocks type. \sa al_flag_t */
}al_t;

/*!
 * Enumerator to indicate if a memory is going to be for heap or stack
 */
typedef enum
{
   AL_STACK=0, AL_HEAP
}al_mem_type_t;

void *m_al (size_t sz, al_mem_type_t mt);
void m_fr (void* p);
void alloc_init (size_t self);

void __malloc_lock (void);    // Spin locks
void __malloc_unlock (void);
uint8_t __malloc_state (void);

uint8_t al_boot (void);
void set_al_boot (void);

caddr_t _sbrk ( int incr );
void *malloc (size_t __size);
void free (void* p);
void *calloc(size_t N, size_t __size);
void *realloc (void * __r, size_t __size);




#endif //#ifndef __alloc_h__

