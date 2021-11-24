/*
 * alloc.c : This file is part of pkernel
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

#include <alloc.h>


static al_t al[ALLOC_SIZE];      /*!< Allocation table to hold the allocated ram blocks for stack or heap. */
static uint8_t malock = 0;       /*!< permit malloc before pkernel boot */
static uint8_t al_boot_f = 0;    /*!< Flag to permit stack allocation before pkernel_run */

/*=============  Static Functions ====================*/

/*!
 * Fill with zero size @a s after address @a p
 */
static void al_zeropad (char *p, size_t s)
{
   char *zp;
   for (zp=p; (size_t)(zp-p)<s ; ++zp)
      *zp=0;
}

/*!
 * Swap al[] entries pointed by @a a1 @a a2
 */
static void swap_al (al_t *a1, al_t *a2)
{
   al_t r = *a1;
   *a1 = *a2;
   *a2 = r;
}

/*!
 * Short al[] so it will be like this: BOTTOM < BLOCK < TOP < UNUSED
 * The BLOCKs are shorted based on address at ram.
 *
 * This helps m_al to calculate empty spaces. Every next block is
 * the flash-neighbor block
 */
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





/*===================  Exported Functions ====================*/

/*!
 * \breif Spin lock to make thread safe the malloc functionality.
 * \param None
 * \return None
 */
void __malloc_lock (void)
{
   while (malock == 1)
      ;
   malock = 1;
}

/*!
 * \breif malloc spin lock's unlock function.
 * \param None
 * \return None
 */
inline void __malloc_unlock (void){
   malock = 0;
}

/*!
 * \breif Reads malloc lock state
 * \return True if its locked
 */
inline uint8_t __malloc_state (void) {
   return malock;
}

/*!
 * Get al_boot flag.
 * al_boot_f = 0 -> pkernel is not yet running.
 * al_boot_f = 1 -> pkernel is running.
 */
inline uint8_t al_boot (void){
   return al_boot_f;
}

/*!
 * Set al_boot flag to 1 to indicate a running pkernel.
 */
inline void set_al_boot (void){
   al_boot_f = 1;
}

/*!
 * \brief Try to find a empty space in both the al[] and the ram, to
 * allocate the requested memory. It use the smaller available block.
 * - The memory is aligned in size_t.
 * - For stack, the return block is placed in the upper memory space
 * of the available block.
 * - For heap it is placed at the bottom.
 *
 * \param sz Size in bytes to allocate.
 * \param mt AL_STACK, AL_HEAP
 * \return Pointer (void*) to allocated memory, or NULL for failure.
 */
void *m_al (size_t sz, al_mem_type_t mt)
{
   int8_t i, slot, r;
   size_t min, asz;
   void* mptr = NULL;

   if (!sz)  return (void*)0;       // Who calls m_al with zero size?
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
    * Stack: Return the base address of the best match
    *        and the block is placed on the upper limit.
    * Heap:  Return the base address of the best match
    *        and the block is placed in the bottom limit.
    */
   if (mptr)
   {
      if (mt == AL_STACK)        // Swift stack in the upper space
         mptr = mptr + (min - sz);

      al[slot].mem_ptr = mptr;
      al[slot].sz = sz;
      al[slot].flag = MA_BLOCK;
      al_short ();
   }
   return mptr;
}

/*!
 * \brief Free up the memory pointed by @a p.
 *
 * \param p pointer to memory.
 * \return None.
 */
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
   /*
    * XXX: free must leave the al[] shorted.
    */
}

/*!
 * \brief Tailoring _malloc_r and _free_r.
 * Allocate or free memory from pkernel using m_al,
 * as requested by _malloc_r or _free_r.
 *
 * \param incr the memory size in bytes.
 * If incr is positive we allocate memory
 * if it is negative we free memory.
 * \return Pointer to allocated memory or NULL for free
 * \note We don't free up memory from _malloc_r calls yet.
 */
caddr_t _sbrk ( int incr )
{
   if (incr>0)
      return (caddr_t) m_al (incr, AL_HEAP);
   else
      return (caddr_t)0;
   /*
    * XXX: I don't free up the memory from
    * malloc_r free_r yet.
    */
}

/*!
 * \brief Allocate a size @a __size memory in bytes and return
 * the address.
 *
 * \param __size of the requested memory in bytes.
 * \return Pointer to allocated memory or NULL on failure.
 * \note Thread safe, not reentrant.
 */
void *malloc (size_t __size)
{
   void * p;
   __malloc_lock ();
   p = m_al (__size, AL_HEAP);
   __malloc_unlock ();
   return p;
}

/*!
 * \brief Free up the memory pointed by @a p.
 *
 * \param p pointer to memory.
 * \return None.
 * \note Thread safe, not reentrant.
 */
void free (void* p)
{
   __malloc_lock ();
   m_fr (p);
   __malloc_unlock ();
}

/*!
 * \brief Allocate a number @a N of size @a __size memory in bytes,
 * clears it and returns the address.
 *
 * \param N number of @a __size blocks to allocate.
 * \param __size of each requested block in bytes.
 * \return Pointer to allocated memory or NULL on failure.
 * \note Thread safe, not reentrant.
 */
void *calloc (size_t N, size_t __size)
{
   char *p;

   __malloc_lock ();
   p = m_al (N*__size, AL_HEAP);
   al_zeropad (p, N*__size);
   __malloc_unlock ();
   return p;
}


/*!
 * \brief Reallocates the memory pointed by @a __r in to a new size
 * @a __size in bytes and returns the new address.
 *
 * \param __r pointer to previous allocates memory.
 * \param __size the new requested size in bytes.
 * \return Pointer to allocated memory or NULL on failure.
 * \note Thread safe, not reentrant.
 */
void *realloc (void *__r, size_t __size)
{
   free (__r);
   return malloc (__size);
}

/*!
 * \brief Initialize pkernel's stack and heap.
 * This functions reads the _estack and _ebss entries
 * in ld script and initialize the allocator.
 *
 *                          <- _estack
 *        ----------------
 *       |      self      |
 *        ----------------                 ---
 *       |                |                 ^
 *       |                |                 | Stack
 *            ...                           |
 *       |                |                 |
 *       |                |             pkernel's RAM
 *       |                |                 |
 *       |                |                 | Heap
 *       |                | <- _ebss       \/
 *        ----------------                 ---
 *       |                |
 *       |     .bss       |
 *       |                | <- _sbss / _edata
 *        ----------------
 *       |                |
 *       |    .data       |
 *       |                | <- _sdata
 *       -----------------
 *
 * \param  self     Memory for pkernels MSP [bytes]
 * \retval None
 *
 */
void alloc_init (size_t self)
{
   unsigned long* btm = (unsigned long*)&_ebss;
   unsigned long* top = (unsigned long*)&_estack;
   top -= self / sizeof(top);

   al[0].mem_ptr = (void*)btm;
   al[0].sz = 0;
   al[0].flag = MA_BOTTOM;

   al[1].mem_ptr = (void*)top;
   al[1].sz = 0;
   al[1].flag = MA_TOP;

   // Init the rest al[] table to NULL
   for (uint8_t i=2; i<ALLOC_SIZE; ++i) {
      al[i].mem_ptr = 0;
      al[i].flag = MA_UNUSED;
   }
}

