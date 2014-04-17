/*
 * pmcs.c : This file is part of pkernel
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

#include <pmcs.h>

static callbacks_t callbacks;

/*!
 * \brief
 *    API function to put the CPU to sleep. When called from a privilege
 *    process will also call presleep() and postsleep() callback functions.
 *
 *    This function will called from pkernel, at idle if the kernel variable
 *    \sa idle_mode is set to \arg IDLE_SLEEP. \sa kernel_var_t
 *
 * \param none.
 * \return none.
 */
void sleepmode (void)
{
   uint32_t fm, pm;   // faultmask and primask

   if (callbacks.presleep)     // Call-back
      callbacks.presleep();
   if (callbacks.postsleep && __kPrivilege())
   {
      /*
       * If postsleep call-back exist we must configure interrupts
       * to execute after postsleep().
       * Only for privilege mode.
       */
      fm = __kget_FAULTMASK ();
      pm = __kget_PRIMASK ();
      __kset_FAULTMASK (0);
      __kset_PRIMASK (1);
   }
   __kWFI();
   if (callbacks.postsleep && __kPrivilege())
   {
      // Run call-back if any and release PRIMASK, FAULTMASK
      callbacks.postsleep ();
      __kset_FAULTMASK (fm);
      __kset_PRIMASK (pm);
   }
}

/*!
 * \brief
 *    API function to put the CPU to stop mode and will also call
 *    prestop() and poststop() call-back functions. This function will
 *    called from pkernel, at idle if the kernel variable
 *    \sa idle_mode is set to \arg IDLE_STOP. \sa kernel_var_t
 * \note
 *    This function will work only when called in privilege mode.
 *
 * \param none.
 * \return none.
 */
void stopmode (void)
{
   uint32_t fm, pm;   // faultmask and primask

   if (! __kPrivilege())      // No privilege, Aboard!
      return;
   if (callbacks.prestop)     // Call-back
      callbacks.prestop();
   if (callbacks.poststop)
   {
      /*
       * If poststop call-back exist we must configure interrupts
       * to execute after poststop().
       */
      fm = __kget_FAULTMASK ();
      pm = __kget_PRIMASK ();
      __kset_FAULTMASK (0);
      __kset_PRIMASK (1);
   }
   // Set SLEEPDEEP flag and go to sleep
   kSCB->SCR |= kSCB_SCR_SLEEPDEEP_Msk;
   __kWFI();

   // Hey, I'm awake, clear SLEEPDEEP flag
   kSCB->SCR &= ~kSCB_SCR_SLEEPDEEP_Msk;
   if (callbacks.poststop)
   {
      // Run call-back if any and release PRIMASK, FAULTMASK
      callbacks.poststop ();
      __kset_FAULTMASK (fm);
      __kset_PRIMASK (pm);
   }
}

/*!
 * \brief
 *    API function to put the CPU to service mode. This mode is for service-driven
 *    applications and prevents returning to main application code (Processes). CPU
 *    will run only the registered services. \sa service_add()
 *    It will also call presleep() and postsleep() call-back functions.
 * \note
 *    CPU will run any other properly configured interrupts also.
 * \note
 *    This function will work only when called in privilege mode.
 * \note
 *    To return from service mode call \see applicationmode() from within
 *    a service.
 *
 * \param none.
 * \return none.
 */
void servicemode (void)
{
   if (! __kPrivilege())      // No privilege, Aboard!
      return;
   if (callbacks.presleep)    // Call-back
      callbacks.presleep();

   // Set SLEEPONEXIT flag and go to sleep
   kSCB->SCR |= kSCB_SCR_SLEEPONEXIT_Msk;
   __kWFI();

   if (callbacks.postsleep)   // Call-back
      callbacks.postsleep ();
}

/*!
 * \brief
 *    API function to put the CPU back to application mode(normal mode), after service
 *    mode.
 * \note
 *    This function will work only when called in privilege mode.
 *
 * \param none.
 * \return none.
 */
void applicationmode (void)
{
   if (! __kPrivilege())   // No privilege, Aboard!
      return;

   // Clear SLEEPONEXIT flag
   kSCB->SCR &= ~kSCB_SCR_SLEEPONEXIT_Msk;
}

/*!
 * \brief
 *    Set call back function to execute before entering sleep mode.
 *    If this is not set, then no presleep call will occur.
 *    This function will called for presleep operation foe every process and
 *    service (Global scope).
 *
 * \param fptr The call-back function
 * \return none.
 *
 * \warning
 *    This function will called ONLY if the process is running in
 *    privilege mode.
 */
void set_presleep (callback_t fptr)
{
   callbacks.presleep = fptr;
}

/*!
 * \brief
 *    Set call back function to execute after exiting sleep mode.
 *    If this is not set, then no postsleep call will occur.
 *    This function will called for postsleep operation foe every process and
 *    service (Global scope).
 *
 * \param fptr The call-back function.
 * \return none.
 *
 * \warning
 *    This function will called ONLY if the process is running in
 *    privilege mode.
 */
void set_postsleep (callback_t fptr)
{
   callbacks.postsleep = fptr;
}

/*!
 * \brief
 *    Set call back function to execute before entering stop mode.
 *    If this is not set, then no prestop call will occur.
 *    This function will called for prestop operation foe every process and
 *    service (Global scope).
 *
 * \param fptr The call-back function
 * \return none.
 *
 * \warning
 *    This function will called ONLY if the process is running in
 *    privilege mode.
 */
void set_prestop (callback_t fptr)
{
   callbacks.prestop = fptr;
}

/*!
 * \brief
 *    Set call back function to execute after exiting stop mode.
 *    If this is not set, then no poststop call will occur.
 *    This function will called for poststop operation foe every process and
 *    service (Global scope).
 *
 * \param fptr The call-back function
 * \return none.
 *
 * \warning
 *    This function will called ONLY if the process is running in
 *    privilege mode.
 */
void set_poststop (callback_t fptr)
{
   callbacks.poststop = fptr;
}


