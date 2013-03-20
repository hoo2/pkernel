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
#include "stdlib.h"

static al_t al[ALLOC_SIZE];
static uint8_t malock = 0;

/*=============  Interrupt Service Routine  ====================*/

void __malloc_lock (void)
{
   while (malock == 1)
      ;
   malock = 1;
}

void __malloc_unlock (void)
{
   malock = 0;
}

static void al_zeropad (uint8_t *p, size_t s)
{
   uint8_t *zp;
   for (zp = p; zp - p < s * sizeof(size_t); ++zp)
      *zp = 0;
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

   top = ALLOC_SIZE - 1;
   do
   {
      for (ok = 1, i = 0; i < top; ++i)
      {
         if ((al[i].flag > al[i + 1].flag)
               || (al[i].flag == al[i + 1].flag
                     && al[i].mem_ptr > al[i + 1].mem_ptr))
         {
            ok = 0;
            ttop = i;
            swap_al (&al[i], &al[i + 1]);
         }
      }
      top = ttop;
   }
   while (!ok);
}

void *m_al (size_t sz, al_mem_type_t mt)
{
   int8_t i, slot;
   size_t min, asz;
   void* mptr = NULL;

   /*
    * First check for available space in al[] array
    * and store the available slot
    */
   for (i = 0; i < ALLOC_SIZE && al[i].flag != MA_UNUSED; ++i)
      ;
   if (i >= ALLOC_SIZE)
      return mptr;
   else
      slot = i;

   /*
    * Dispatch in two methods.
    * Stack: Return the tip+1 address of the best match.
    * Heap:  Return the base address of the best match.
    */
   for (i=0, min = (size_t) -1;
		   al[i+1].flag!=MA_UNUSED && i<ALLOC_SIZE-1; ++i)
   {
      /*
       * Best match:
       * Calculate the space between the blocks.
       * The next non empty place is the next place in al[]
       * AND Keep track of the min acceptable space.
       */
      asz = (al[i + 1].mem_ptr - al[i].mem_ptr) / sizeof(size_t) - al[i].sz;

      if (sz <= asz && asz < min)
      {
         min = asz;
         mptr = (void*) ((uint32_t) al[i].mem_ptr + al[i].sz * sizeof(size_t));
      }
   }
   if (mptr)   // Place found
   {
      if (mt == AL_STACK)
      {
         al[slot].mem_ptr = mptr + (min - sz) * sizeof(size_t);
         al[slot].sz = sz;
         mptr = (void*) (al[slot].mem_ptr + sz * sizeof(size_t));
      }
      else // mt == AL_HEAP
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
   int s = incr/sizeof(size_t);
   int r = incr%sizeof(size_t);

   if (r)
      ++s;
   return (caddr_t) m_al (s, AL_HEAP);
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
   void *p;

   __malloc_lock ();
   p = m_al (N * __size, AL_HEAP);
   al_zeropad ((uint8_t*) p, N * __size);
   __malloc_unlock ();
   return p;
}

void *realloc (void * __r, size_t __size)
{
   free (__r);
   return malloc (__size);
}
/*
void *_malloc_r  (struct _reent *, size_t)
{

}
void *_calloc_r  (struct _reent *, size_t, size_t)
{

}
void _free_r  (struct _reent *, void *)
{

}
void *_realloc_r  (struct _reent *, void *, size_t)
{

}
*/
\
void alloc_init (void)
{
   uint8_t i;

   /*
    * As you can see in LabBook page 38,
    * pkernel's stack lives in
    *
    *    ----------------                 ---
    *   |                | <- _eram        ^
    *   |                |                 | Stack
    *   |                |                 |
    *          ...
    *   |                |             pkernel's
    *   |                |              stack
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
   uint32_t ps = (uint32_t) & pulStack;
   uint32_t ed = (uint32_t) & _eram;

   /*
    *
    */
   al[0].mem_ptr = (void*) ps;
   al[0].sz = 0;
   al[0].flag = MA_BOTTOM;
   al[1].mem_ptr = (void*) ed;
   al[1].sz = 0;
   al[1].flag = MA_TOP;

   // Init the rest al table to NULL
   for (i=2; i<ALLOC_SIZE; ++i)
   {
      al[i].mem_ptr = 0;
      al[i].flag = MA_UNUSED;
   }
   __malloc_unlock ();
}

