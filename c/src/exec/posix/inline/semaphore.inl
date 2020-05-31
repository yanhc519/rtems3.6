/*  rtems/posix/semaphore.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX Semaphores.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_SEMAPHORE_inl
#define __RTEMS_POSIX_SEMAPHORE_inl
 
/*PAGE
 *
 *  _POSIX_Semaphore_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void )
{
  return (POSIX_Semaphore_Control *)
    _Objects_Allocate( &_POSIX_Semaphore_Information );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Get
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  Objects_Id        *id,
  Objects_Locations *location
)
{
  return (POSIX_Semaphore_Control *)
    _Objects_Get( &_POSIX_Semaphore_Information, *id, location );
}
 
/*PAGE
 *
 *  _POSIX_Semaphore_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Semaphore_Is_null (
  POSIX_Semaphore_Control *the_semaphore
)
{
  return !the_semaphore;
}

#endif
/*  end of include file */

