/*  rtems/posix/mqueue.h
 *
 *  This include file contains all the private support information for
 *  POSIX Message Queues.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_MESSAGE_QUEUE_h
#define __RTEMS_POSIX_MESSAGE_QUEUE_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coremsg.h>
#include <rtems/score/object.h>

/*
 *  Data Structure used to manage a POSIX message queue
 */
 
typedef struct {
   Objects_Control             Object;
   int                         process_shared;
   int                         flags;
   boolean                     named;
   boolean                     linked;
   boolean                     blocking;
   unsigned32                  open_count;
   CORE_message_queue_Control  Message_queue;
   struct sigevent             notification;
}  POSIX_Message_queue_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Message_queue_Information;
 
/*
 *  _POSIX_Message_queue_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Message_queue_Manager_initialization(
  unsigned32 maximum_message_queues
);
 
/*
 *
 *  _POSIX_Message_queue_Create_support
 *
 *  DESCRIPTION:
 *
 *  This routine performs the creation of a message queue utilizing the
 *  core message queue.
 */
 
int _POSIX_Message_queue_Create_support(
  const char                    *name,
  int                            pshared,
  unsigned int                   oflag,
  struct mq_attr                *attr,
  POSIX_Message_queue_Control  **message_queue
);

/*
 *
 *  _POSIX_Message_queue_Send_support
 *
 *  DESCRIPTION:
 *
 *  This routine posts a message to a specified message queue.
 */

int _POSIX_Message_queue_Send_support(
  mqd_t               mqdes,
  const char         *msg_ptr,
  unsigned32          msg_len,
  Priority_Control    msg_prio,
  Watchdog_Interval   timeout
);

/*
 *  _POSIX_Message_queue_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a message queue control block from
 *  the inactive chain of free message queue control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Allocate( void );
 
/*
 *  _POSIX_Message_queue_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a message queue control block to the
 *  inactive chain of free message queue control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Message_queue_Free (
  POSIX_Message_queue_Control *the_mq
);
 
/*
 *  _POSIX_Message_queue_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps message queue IDs to message queue control blocks.
 *  If ID corresponds to a local message queue, then it returns
 *  the_mq control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the message queue ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_message queue is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_mq is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Message_queue_Control *_POSIX_Message_queue_Get (
  Objects_Id         id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Message_queue_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_message_queue is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Message_queue_Is_null (
  POSIX_Message_queue_Control *the_mq
);

/*
 *  _POSIX_Message_queue_Name_to_id
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

int _POSIX_Message_queue_Name_to_id(
  const char          *name,
  Objects_Id          *id
);

/*
 *  _POSIX_Message_queue_Priority_to_core
 * 
 *  DESCRIPTION:
 *
 *  XXX
 */
 
RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Message_queue_Priority_to_core(
  unsigned int priority
);

#include <rtems/posix/mqueue.inl>
#include <rtems/posix/mqueuemp.h>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

