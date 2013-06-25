/*
 * sched.c : This file is part of pkernel
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

#include <sched.h>

/*!
 * A list that holds all the active/running processes. The list does not
 * have nodes of it own. It use the items in proc[] for nodes.
 */
static proc_list_t    runq;

/*!
 * A list that holds all the suspended processes. The list does not
 * have nodes of it own. It use the items in proc[] for nodes.
 */
static proc_list_t    susq;

/*!
 * \brief pkernel's Scheduler.
 * This is a round robin scheduler with some extras. When called:
 * - Check if we have a suspended process that needs to wake up in susq.
 * - Check priorities between the suspended process and current process
 * - Wake up by inserting the process to the "correct" place in runq and
 * remove it from susq
 * - Round the runq if needed. (ticks_left is 0).
 *
 * \return the pid of the selected process to run.
 * \note Schedule is called from PendSV only.
 */
pid_t schedule(void)
{
   process_t *p, *wp = sch_alarm ();
   pid_t pid;
   uint8_t  ins=0;

   /*
    * If we have a awakened process, we have to put
    * it somewhere. So we look the niceness.
    */
   if (wp)
   {
      sch_list_remove (&susq, wp);  // Remove it from susq

      if (sch_runq_empty ())        // No others
         sch_list_ins_front (&runq, wp);
      else                          // There is others, check niceness.
      {
         /*
          * Find first spot in front of a non zero ticks_left process
          * with bigger niceness than wp.
          * \note
          *    We leave zero ticks_left process out, because the rest of
          *    the scheduler will roll them.
          */
         p = runq.head;
         ins = 1;
         while (ins && p)
         {
            if (p->ticks_left && (wp->nice < p->nice))
            {
               sch_list_ins (&runq, wp, p);
               ins = 0;
            }
            p = p->next;
         }
         if (ins)                   // All items was meaner than wp
            sch_list_ins_back (&runq, wp);
      }
   }

   // If we still have no processes, switch to idle
   if (sch_runq_empty ())
      pid = 0;
   // If we are here check ticks to roll the process
   else if (runq.head->ticks_left <= 0)
   {
      // start a new timeslice
      proc_rst_ticks (runq.head->id);

      if(runq.head->next)
      {
         // If we have something else to run roll the runq
         p = runq.head;
         sch_list_remove(&runq, p);
         sch_list_ins_back(&runq, p);
      }
      pid = runq.head->id;
   }
   else
      pid = runq.head->id;

   // return to the selected process.
   return pid;
}

/*!
 * \brief Check if there is a process in susq needs to awake.
 * - Check if a process, suspended by sleep() has expire it's sleep time.
 * - Check if a process, suspended by wait() / lock(), has a positive semaphore value now.
 *
 * \return Pointer to process that has to wake up or NULL there is none.
 */
process_t* sch_alarm (void)
{
   process_t *p=(process_t*)0;
   uint8_t  wu = 0;   // wakeup flag

   if (sch_susq_empty())
      return p;

   do {
      if (!p)
         p = susq.head;
      else
         p =p->next;
      /*
       * If Alarm       then alarm <= Ticks
       * If Semaphore   then Value > 0
       */
      wu = 0;
      if (!p->alarm || p->alarm <= Ticks)
         ++wu;
      if (!p->sem || (p->sem && p->sem->val>0))
         ++wu;
      if (wu>1)
      {
         // Release the process from shackles
         p->alarm = 0;
         p->sem = (void*)0;
         return p;
      }

   } while (p->next);

   return (process_t*)0;
}

/*!
 * \brief Add a process at the end of the runq
 */
void sch_add_proc(pid_t pid)
{
   process_t *proc;

   proc = proc_get_process(pid);
   sch_list_ins_back(&runq, proc);
}

/*!
 * \brief Remove a process from the runq
 */
void sch_remove_proc (pid_t pid)
{
   process_t *proc;

   proc = proc_get_process(pid);
   sch_list_remove (&runq, proc);
}



/* ================== list operations ================== */

/*!
 * \brief Add a process at the end of the @a list.
 * \param list Pointer to list
 * \param proc Pointer to process
 */
void sch_list_ins_back(proc_list_t *list, process_t *proc)
{
   if(sch_empty_list(list))
      list->head = proc;
   else
      list->tail->next = proc;

   proc->next = 0;
   proc->prev = list->tail;
   list->tail = proc;
}

/*!
 * \brief Add a process in front of the \a list.
 * \param list Pointer to list
 * \param proc Pointer to process
 */
void sch_list_ins_front(proc_list_t *list, process_t *proc)
{
   if(sch_empty_list(list))
      list->tail = proc;
   else
      list->head->prev = proc;

   proc->next = list->head;
   proc->prev = 0;
   list->head = proc;
}

/*!
 * \brief Add a process in the \a list before \b before.
 * \param list Pointer to list
 * \param proc Pointer to process
 */
void sch_list_ins (proc_list_t *list, process_t *proc, process_t *before)
{
   process_t *pp;

   if (list->head == before)
      sch_list_ins_front(list, proc);
   else
   {
      pp = before->prev;
      pp->next = proc;
      before->prev = proc;

      proc->next = before;
      proc->prev = pp;
   }
}

/*!
 * \brief Remove a process from a @a list.
 * \param list Pointer to list
 * \param proc Pointer to process
 */
void sch_list_remove(proc_list_t *list, process_t *proc)
{
   if(proc->prev)
      proc->prev->next = proc->next;
   if(proc->next)
      proc->next->prev = proc->prev;
   if(list->head == proc)
      list->head = proc->next;
   if(list->tail == proc)
      list->tail = proc->prev;

}

/*!
 * \brief Remove a process from runq and insert at the end of the susq.
 * \param proc Pointer to process
 */
void sch_susp_proc (process_t *p)
{
   sch_list_remove (&runq, p);
   sch_list_ins_back (&susq, p);
}

/*!
 * \brief Remove a process from scheduler lists.
 * \param proc Pointer to process
 */
void sch_exit (process_t *p)
{
   sch_list_remove (&runq, p);
   /*
    * We don't need to remove proc from susq, because
    * proc is running
    */
}

/*!
 * Return if a list is empty
 */
inline int sch_empty_list (proc_list_t *l)
{
   return (l->head) ? 0 : 1;
}

/*!
 * Return if runq is empty
 */
inline int sch_runq_empty (void)
{
   return (runq.head) ? 0 : 1;
}

/*!
 * Return if susq is empty
 */
inline int sch_susq_empty (void)
{
   return (susq.head) ? 0 : 1;
}

