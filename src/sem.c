/*
 * sem.c : This file is part of pkernel
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

#include <sem.h>

/*!
 * \brief Open semaphore. Try to find a space for a new semaphore.
 * If there is, then allocates it and initialize the semaphore to \a v
 *
 * \param v The initial value of semaphore.
 * \return Pointer to semaphore on success, NULL if no space.
 *
 * \note
 *    This function MUST NOT called within the pkernel's Interrupts
*/
static sem_t* sopen(int v)
{
   sem_t* s;

   s = (sem_t*) malloc (sizeof (sem_t));
   if (s)
      s->val = v;
   return s;
}


/*!
 * \brief Open/Create semaphore. Try to find a space for a new semaphore.
 * If there is space in semaphore[], allocates it and initialize the
 * semaphore to 0.
 *
 * \param None
 * \return Pointer to semaphore on success, NULL if no space.
*/
inline sem_t* sem_open(void)
{
   return sopen (0);
}

/*!
 * \brief Open/Create a mutex(binary semaphore). Tryto find a space for a new mutex.
 * If there is space in semaphore[], allocates it and initialize the semaphore to 1 (unlocked).
 *
 * \param None
 * \return Pointer to mutex on success, NULL if no space.
*/
inline sem_t* mut_open (void)
{
   return sopen (1);
}

/*!
 * \brief Close semaphore. If semaphore's value don't indicate
 * a locking state(val<=0), then clears it.
 *
 * \param s Semaphore to close.
 * \return Positive on success, 0 if the semaphore is locked.
*/
int sem_close (sem_t *s)
{
   if (s->val>0)
   {
      free ((void*)s);
      return 1;
   }
   return 0;
}



