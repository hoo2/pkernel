/*
 * cron.c : This file is part of pkernel
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
 * Date:       06/2013
 * Version:
 *
 */

#include <cron.h>

static cron_list_t     cronl;
static micron_list_t   micronl;

static uint8_t cron_stretch;
static uint8_t micron_stretch;
/*!
 * Return if a list is empty
 */
static inline int micron_empty_list (micron_list_t *l)
{
   return (l->head) ? 0 : 1;
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
void microntab (micronfun_t fptr, clock_t every)
{
   micron_t *m = (micron_t *) malloc (sizeof (micron_t));

   if (!m)  // check for free space
      return;

   // Add the micron node to the micron list
   micron_stretch = 1;
   /*
    * Micron stretching for list manipulation.
    * It's OK to loose "some" ticks when we configure micron.
    */
   if(micron_empty_list(&micronl))
      micronl.head = m;
   else
      micronl.tail->next = m;

   m->next = 0;
   m->prev = micronl.tail;
   micronl.tail = m;

   // Fill micron values
   m->every = every;
   m->fptr = fptr;
   micron_stretch = 0;
}

/*!
 * \brief   Remove a function from Micron
 * \param   fptr Pointer to function
 */
void microntab_r (micronfun_t fptr)
{
   micron_t *m = micronl.head;

   // Find node or return
   for ( ; m && (m->fptr!=fptr) ; m=m->next)
      ;
   if (!m)
      return;

   // Remove it
   micron_stretch = 1;
   /*
    * Micron stretching for list manipulation.
    * It's OK to loose "some" ticks when we configure micron.
    */
   if(m->prev)
      m->prev->next = m->next;
   if(m->next)
      m->next->prev = m->prev;
   if(micronl.head == m)
      micronl.head = m->next;
   if(micronl.tail == m)
      micronl.tail = m->prev;
   micron_stretch = 0;
   free (m);
}

/*!
 * \brief
 *    Micron service. This function is called from SysTick
 *    handler every tick.
 */
void micron (void)
{
   micron_t *m = micronl.head;

   if (!m || micron_stretch)
      return;     // Micron stretching, or no micron list. Aboard!

   // Find node to call
   do {
      if (!(Ticks % m->every))
         m->fptr ();
      m=m->next;
   } while (m);
}

/*!
 * Return if a list is empty
 */
static inline int cron_empty_list (cron_list_t *l)
{
   return (l->head) ? 0 : 1;
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
   cron_stretch = 1;
   /*
    * Cron stretching for list manipulation.
    * It's OK to loose "some" ticks when we configure cron.
    */
   if(cron_empty_list(&cronl))
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
   cron_stretch = 0;
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
   cron_stretch = 1;
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
   cron_stretch = 0;
   free (m);
}

/*!
 * \brief
 *    Returns the cron_postponed flag
 */
inline uint8_t cron_stretching(void) {
   return cron_stretch;
}

/*!
 * \brief
 *    Cron service. This function is called from SysTick
 *    handler every sec.
 */
void cron (void)
{
   cron_t *m = cronl.head;

   if (!m)     // No Cron list aboard
      return;

   if (__malloc_state())   // Malloc is locked. Aboard with flag
   {
      cron_stretch = 1;
      return;
   }
   // Find process to create
   do {
      if ((Now == m->at) || (!((Now-m->at) % m->every)))
      {
         // Call knew() if the process does not exist
         if (proc_search_pid (m->fptr) == -1)
            knew (m->fptr, m->ms, m->nice, m->fit);
         cron_stretch = 0;
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
