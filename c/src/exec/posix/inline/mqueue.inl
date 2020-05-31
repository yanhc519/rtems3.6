/*  rtems/posix/mqueue.inl
 *
 *  This include file contains the static inline implementation of the private 
 *  inlined routines for POSIX Message Queue.
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
 
#ifndef __RTEMS_POSIX_MESSAGE_QUEUE_inl
#define __RTEMS_POSIX_MESSAGE_QUEUE_inl
 
/*PAGE
 *
 *  _POSIX_Message_queue_Allocate
 */
 
RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Allocate( void )
{
  return (POSIX_Message_queue_Control *)
    _Objects_Allocate( &_POSIX_Message_queue_Information );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Free
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Message_queue_Free (
  POSIX_Message_queue_Control *the_mq
)
{
  _Objects_Free( &_POSIX_Message_queue_Information, &the_mq->Object );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Get
 */
 
RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
)
{
  return (POSIX_Message_queue_Control *)
    _Objects_Get( &_POSIX_Message_queue_Information, id, location );
}
 
/*PAGE
 *
 *  _POSIX_Message_queue_Is_null
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Message_queue_Is_null (
  POSIX_Message_queue_Control *the_mq
)
{
  return !the_mq;
}

/*PAGE
 *
 *  _POSIX_Message_queue_Priority_to_core
 */
 
RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Message_queue_Priority_to_core(
  unsigned int priority
)
{
  return priority;
}

#endif
/*  end of include file */

