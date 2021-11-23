/*
 * cron.c : This file is part of pkernel
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
 * Date:       06/2013
 * Version:
 *
 */

#include <cron.h>

static service_list_t  servl;
static cron_list_t     cronl;

/*!
 * \brief   Adds a function to Service list
 * \param   pfun  Pointer to function
 * \param   every Tick period. pkernel will run the \a fptr every \b every Ticks
 *
 * \note
 *    All the entries will run in privileged mode and will use the main
 *    stack.
 */
void service_add (service_t fptr, clock_t every)
{
   service_item_t *m = (service_item_t *) malloc (sizeof (service_item_t));

   if (!m)  // check for free space
      return;

   // Add the service node to the service list
   kernel_vars.service_lock = 1;
   /*
    * Service lock for list manipulation.
    * It's OK to loose "some" ticks when we configure services.
    */
   if(!servl.head)
      servl.head = m;
   else
      servl.tail->next = m;

   m->next = 0;
   m->prev = servl.tail;
   servl.tail = m;

   // Fill micron values
   m->every = every;
   m->fptr = fptr;
   kernel_vars.service_lock = 0;
}

/*!
 * \brief   Remove a function from Service list
 * \param   fptr Pointer to function
 */
void service_rem (service_t fptr)
{
   service_item_t *m = servl.head;

   // Find node or return
   for ( ; m && (m->fptr!=fptr) ; m=m->next)
      ;
   if (!m)
      return;

   // Remove it
   kernel_vars.service_lock = 1;
   /*
    * Service lock for list manipulation.
    * It's OK to loose "some" ticks when we configure services.
    */
   if(m->prev)
      m->prev->next = m->next;
   if(m->next)
      m->next->prev = m->prev;
   if(servl.head == m)
      servl.head = m->next;
   if(servl.tail == m)
      servl.tail = m->prev;
   kernel_vars.service_lock = 0;
   free (m);
}

/*!
 * \brief
 *    Micron service. This function is called from SysTick
 *    handler every tick.
 */
void services (void)
{
   service_item_t *m = servl.head;

   if (!m || kernel_vars.service_lock)
      return;     // Service lock, or no service list. Aboard!

   // Find node to call
   do {
      if (!(Ticks % m->every))
         m->fptr ();
      m=m->next;
   } while (m);
}

/*!
 * \brief   Adds a function to micron list
 * \param   pfun  Pointer to function
 * \param   every Tick period. Micron will run the \a fptr every \b every Ticks
 *
 * \note
 *    All the entries will run in privileged mode and will use the main
 *    stack.
 */
void crontab ( process_ptr_t fptr, size_t ms,
               int8_t nice, int8_t fit, uint8_t pr,
               time_t at, time_t every)
{
   cron_t *m = (cron_t *) malloc (sizeof (cron_t));

   if (!m)  // check for free space
      return;

   // Add the cron node to the cron list
   kernel_vars.cron_stretch = 1;
   /*
    * Cron stretching for list manipulation.
    * It's OK to loose "some" ticks when we configure cron.
    */
   if(!cronl.head)
      cronl.head = m;
   else
      cronl.tail->next = m;

   m->next = 0;
   m->prev = cronl.tail;
   cronl.tail = m;

   // Fill cron values
   m->fptr = fptr;
   m->ms = ms;
   m->nice = nice;
   m->fit = fit;
   m->pr = pr;
   m->at = at;
   m->every = every;
   kernel_vars.cron_stretch = 0;
}

/*!
 * \brief   Remove a function from Micron
 * \param   fptr Pointer to function
 */
void crontab_r (process_ptr_t fptr)
{
   cron_t *m = cronl.head;

   // Find node or return
   for ( ; m && (m->fptr!=fptr) ; m=m->next)
      ;
   if (!m)
      return;

   // Remove it
   kernel_vars.cron_stretch = 1;
   /*
    * Cron stretching for list manipulation.
    * It's OK to loose "some" ticks when we configure cron.
    */
   if(m->prev)
      m->prev->next = m->next;
   if(m->next)
      m->next->prev = m->prev;
   if(cronl.head == m)
      cronl.head = m->next;
   if(cronl.tail == m)
      cronl.tail = m->prev;
   kernel_vars.cron_stretch = 0;
   free (m);
}

/*!
 * \brief
 *    Returns the cron_postponed flag
 */
inline uint8_t cron_stretching(void) {
   return kernel_vars.cron_stretch;
}

/*!
 * \brief
 *    Cron service. This function is called from SysTick
 *    handler every sec.
 */
void cron (void)
{
   cron_t *m = cronl.head;

   if (!m)     // No Cron list. Aboart!
      return;

   if (__malloc_state() || __proc_state())
   {
      /*
       * malloc or proc[] is locked.
       * Aboart with cron stretching.
       */
       kernel_vars.cron_stretch = 1;
      return;
   }
   // Find process to create
   do {
      if ((Now == m->at) || (!((Now-m->at) % m->every)))
      {
         // Call knew() if the process does not exist
         if (proc_search_pid (m->fptr) == -1)
            knew (m->fptr, m->ms, m->nice, m->fit);
         kernel_vars.cron_stretch = 0;
         /*!
          * \note
          * Yes! We can call knew(). But only if no one else has locked
          * malloc. We wait until malloc_unlock() (Cron stretching). Then we
          * clear cron_strech flag So the SysTick ISR will not call cron() again.
          *
          * *** So yes we call knew() and we don't have reentrant struct! Faaaact! ***
          */
      }
      m=m->next;
   } while (m);
}
