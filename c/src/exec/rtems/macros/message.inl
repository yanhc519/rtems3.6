/*  message.inl
 *
 *  This include file contains the macro implementation of all
 *  inlined routines in the Message Manager.
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

#ifndef __MESSAGE_QUEUE_inl
#define __MESSAGE_QUEUE_inl

/*PAGE
 *
 *  _Message_queue_Is_null
 *
 */

#define _Message_queue_Is_null( _the_message_queue ) \
   ( (_the_message_queue) == NULL  )

/*PAGE
 *
 *  _Message_queue_Free
 *
 */

#define _Message_queue_Free( _the_message_queue ) \
  _Objects_Free( &_Message_queue_Information, &(_the_message_queue)->Object )

/*PAGE
 *
 *  _Message_queue_Get
 *
 */

#define _Message_queue_Get( _id, _location ) \
  (Message_queue_Control *) \
     _Objects_Get( &_Message_queue_Information, (_id), (_location) )

#endif
/* end of include file */
