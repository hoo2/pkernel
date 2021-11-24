/*
 * sem.c : This file is part of pkernel
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

#include <sem.h>

/*!
 * \brief
 *    Open/Initialize semaphore.
 *
 * \param s,   Pointer to semaphore to initialize
 * \param v    The initial value of semaphore.
 */
static void _sinit (sem_t *s, int v) {
   if (s) {
      s->val = v;
   }
}


/*!
 * \brief
 *    Open/Initialize semaphore.
 * \note
 *    The usual init value of a semaphore is 0.
 *
 * \param s,   Pointer to semaphore to initialize
 * \param v    The initial value of semaphore.
 */
inline void sem_init (sem_t* s, int v) {
   _sinit (s, v);
}

/*!
 * \brief
 *    Close/De-Initialize a semaphore.
 *
 * \param   s,   Pointer to semaphore to close
 * \return  0
 */
inline int sem_close (sem_t *s) {
   return s->val = 0;
}

/*!
 * \brief
 *    Get semaphore's value without any interaction to it
 *
 * \param  s pointer to semaphore used
 * \return The semaphore value
 */
int sem_getvalue (sem_t *s) {
    return s->val;
}

/*!
 * \brief
 *    This function checks for a semaphore value. If the semaphore
 *    is positive decreases it and return true. Else return false
 *
 * \param  s        Pointer to semaphore used
 * \return true     For positive semaphore value.
 *
 * \note Thread safe, not reentrant.
 */
int sem_check (sem_t *s) {
    if (s->val > 0) {
        --s->val;
        return 1;
    }
    else
        return 0;
}

/*!
 * \brief
 *    Open/Initialize mutex which is a binary semaphore for this implementation
 * \note
 *    The init value of a mutex is 1 (unlocked)
 * \param m    Pointer to mutex to initialize
 */
inline void mut_init (sem_t* m) {
    _sinit (m, 1);
}

/*!
* \brief
*    Close/De-Initialize a mutex.
*
* \param   m    Pointer to mutex to close
* \return  0
*/
int mut_close (sem_t *m) {
    return m->val = 1;
}

/*!
* \brief
*    This function checks for a mutex.
*    If its 1 (unlocked) decreases it and return true.
*    Else return false (already locked)
*
* \param  s     Pointer to mutex used
* \return the status of the operation
*    \arg  0  Fail to lock, mutex already locked
*    \arg  1  Success, mutex is locked by the function
*
* \note Thread safe, not reentrant.
*/
int mut_trylock (sem_t *m) {
    if (m->val > 0) {
        m->val = 0;
        return 1;
    }
    else
        return 0;
}



