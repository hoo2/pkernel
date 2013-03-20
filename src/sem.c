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

#include "sem.h"

static sem_t semaphore[MAX_SEMAPHORES];

static sem_t* sopen(int v)
{
   int i;

   for (i=0 ; i<MAX_SEMAPHORES ; ++i)
      if (!semaphore[i].en)
      {
         semaphore[i].en = 1;
         semaphore[i].val = v;
         return &semaphore[i];
      }
   return (sem_t*)0; 
}
/*
   sem_t* sem_open(void)

   Open semaphore. It tries to find a space for a new semaphore.
   If there is, then is allocates it and init the semaphore to 0

   Arguments none
   Return    sem_t*  pointer to semaphore on success
                     NULL if no space
*/
sem_t* sem_open(void)
{
   return sopen (0); 
}

/*
   sem_t* mut_open(void)

   Open mutex (Binary semaphore). It tries to find a space for a new mutex.
   If there is, then is allocates it and init the mutex to 1 (unlocked)

   Arguments none
   Return    sem_t*  pointer to mutex on success
                     NULL if no space
*/
sem_t* mut_open (void)
{
   return sopen (1);
}

/*
   int sem_close (sem_t *s)

   If semaphore is not locking anything, the clears it.

   Return >0 on success
          0 if locked
*/
int sem_close (sem_t *s)
{
   if (s->val>0)
      s->en = 0;
   return s->val;      
}



