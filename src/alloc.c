/*
 * alloc.c : This file is part of pkernel
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

#include "alloc.h"

static al_t al[ALLOC_SIZE];
static uint8_t malock = 1;    // Don't permit malloc before pkernel boot
static uint8_t al_boot_f = 0;   // Flag to permit stack allocation before pkernel_run

/*=============  Interrupt Service Routine  ====================*/

static void al_zeropad (char *p, size_t s)
{
   char *zp;
   for (zp=p; zp-p<s ; ++zp)
      *zp=0;
}

static void swap_al (al_t *a1, al_t *a2)
{
   al_t r = *a1;
   *a1 = *a2;
   *a2 = r;
}

static void al_short (void)
{
   uint8_t i;
   uint8_t ok, top, ttop;

   top = ttop = ALLOC_SIZE - 1;
   do
   {
      for (ok=1, i=0; i<top; ++i)
      {
         if ((al[i].flag > al[i+1].flag)
               || (al[i].flag == al[i+1].flag
                     && al[i].mem_ptr > al[i+1].mem_ptr))
         {
            ok = 0;
            ttop = i;
            swap_al (&al[i], &al[i+1]);
         }
      }
      top = ttop;
   }
   while (!ok);
}

void __malloc_lock (void)
{
   while (malock == 1)
      ;
   malock = 1;
}

void __malloc_unlock (void){
   malock = 0;
}

__INLINE uint8_t al_boot (void){
   return al_boot_f;
}

__INLINE void set_al_boot (void){
   al_boot_f = 1;
}

void *m_al (size_t sz, al_mem_type_t mt)
{
   int8_t i, slot, r;
   size_t min, asz;
   void* mptr = NULL;

   if ((r=sz%sizeof(size_t)) != 0)  // Alignment to size_t
      sz += sizeof(size_t) - r;

   /*
    * First check for available space in al[] array
    * and store the available slot
    */
   for (i=0; al[i].flag != MA_UNUSED && i<ALLOC_SIZE ; ++i)
      ;
   if (i >= ALLOC_SIZE)
      return mptr;
   else
      slot = i;

   /*
    * Best match:
    * - Calculate the space between the blocks.
    * - The next non empty place is the next place in al[]
    * - Keep track of the min acceptable space.
    */
   for (i=0, min=(size_t)-1;
		   al[i+1].flag!=MA_UNUSED && i<ALLOC_SIZE-1; ++i)
   {
      asz = (al[i+1].mem_ptr - al[i].mem_ptr) - al[i].sz;

      if (sz<=asz && asz<min)
      {
         min = asz;
         mptr = (void*) ((uint32_t)al[i].mem_ptr + al[i].sz);
      }
   }

   /*
    * Dispatch in two methods.
    * Stack: Return the tip+1 address of the best match.
    * Heap:  Return the base address of the best match.
    */
   if (mptr)
   {
      if (mt == AL_STACK)
      {
         al[slot].mem_ptr = mptr + (min - sz);
         al[slot].sz = sz;
         mptr = (void*) (al[slot].mem_ptr + sz);
      }
      else  // AL_HEAP
      {
         al[slot].mem_ptr = mptr;
         al[slot].sz = sz;
      }
      al[slot].flag = MA_BLOCK;
      al_short ();
   }
   return mptr;
}

void m_fr (void* p)
{
   int8_t i;
   for (i = 0; i < ALLOC_SIZE; ++i)
      if (p == al[i].mem_ptr && al[i].flag == MA_BLOCK)
      {
         al[i].mem_ptr = NULL;
         al[i].sz = 0;
         al[i].flag = MA_UNUSED;
         break;
      }
   al_short ();
}

caddr_t _sbrk ( int incr )
{
   if (incr>0)
      return (caddr_t) m_al (incr, AL_HEAP);
   else
      return (caddr_t)0;
   /*
    * XXX: I dont free up the memory from
    * malloc_r free_r yet.
    */
}

void *malloc (size_t __size)
{
   void * p;
   __malloc_lock ();
   p = m_al (__size, AL_HEAP);
   __malloc_unlock ();
   return p;
}

void free (void* p)
{
   __malloc_lock ();
   m_fr (p);
   __malloc_unlock ();
}

void *calloc (size_t N, size_t __size)
{
   char *p;

   __malloc_lock ();
   p = m_al (N*__size, AL_HEAP);
   al_zeropad (p, N*__size);
   __malloc_unlock ();
   return p;
}

void *realloc (void *__r, size_t __size)
{
   free (__r);
   return malloc (__size);
}


void alloc_init (void)
{
   uint8_t i;

   /*
    * pkernel's stack and heap lives in
    *
    *    ----------------                 ---
    *   |                | <- _eram        ^
    *   |                |                 | Stack
    *   |                |                 |
    *          ...
    *   |                |             pkernel's
    *   |                |
    *    -  -  -  -  -  -                  |
    *   |    Startup's   |                 | Heap
    *   |     stack      | <- pulStack     ,
    *    ----------------                 ---
    *   |                | <- _ebss
    *   |                |
    *   |                | <- _sbss / _edata
    *    ----------------
    *   |                |
    *   |                |
    *   |                | <- _sdata
    *   -----------------
    */
   al[0].mem_ptr = (void*) &pulStack;
   al[0].sz = 0;
   al[0].flag = MA_BOTTOM;

   al[1].mem_ptr = (void*) &_eram;
   al[1].sz = 0;
   al[1].flag = MA_TOP;

   // Init the rest al table to NULL
   for (i=2; i<ALLOC_SIZE; ++i)
   {
      al[i].mem_ptr = 0;
      al[i].flag = MA_UNUSED;
   }
}

