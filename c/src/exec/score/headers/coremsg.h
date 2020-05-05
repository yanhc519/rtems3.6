/*  coremsg.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Message queue Handler.
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
 
#ifndef __RTEMS_CORE_MESSAGE_QUEUE_h
#define __RTEMS_CORE_MESSAGE_QUEUE_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/priority.h>
#include <rtems/score/watchdog.h>
 
/*
 *  The following type defines the callout which the API provides
 *  to support global/multiprocessor operations on message_queues.
 */
 
typedef void ( *CORE_message_queue_API_mp_support_callout )(
                 Thread_Control *,
                 Objects_Id
             );

/*
 *  The following defines the data types needed to manipulate
 *  the contents of message buffers.
 *
 *  NOTE:  The buffer field is normally longer than a single unsigned32.
 *         but since messages are variable length we just make a ptr to 1.  
 */
 
typedef struct {
    unsigned32  size;
    unsigned32  buffer[1];
} CORE_message_queue_Buffer;
 
/*
 *  The following records define the organization of a message
 *  buffer.
 */
 
typedef struct {
  Chain_Node                 Node;
  CORE_message_queue_Buffer  Contents;
}   CORE_message_queue_Buffer_control;

/*
 *  Blocking disciplines for a message_queue.
 */

typedef enum {
  CORE_MESSAGE_QUEUE_DISCIPLINES_FIFO,
  CORE_MESSAGE_QUEUE_DISCIPLINES_PRIORITY
}   CORE_message_queue_Disciplines;

/*
 *  The following enumerated type details the modes in which a message
 *  may be submitted to a message queue.  The message may be posted
 *  in a send or urgent fashion.
 */
 
typedef enum {
  CORE_MESSAGE_QUEUE_SEND_REQUEST   = 0,
  CORE_MESSAGE_QUEUE_URGENT_REQUEST = 1
}  CORE_message_queue_Submit_types;

/*
 *  Core Message queue handler return statuses.
 */
 
typedef enum {
  CORE_MESSAGE_QUEUE_STATUS_SUCCESSFUL,
  CORE_MESSAGE_QUEUE_STATUS_INVALID_SIZE,
  CORE_MESSAGE_QUEUE_STATUS_TOO_MANY,
  CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED,
  CORE_MESSAGE_QUEUE_STATUS_UNSATISFIED_NOWAIT,
  CORE_MESSAGE_QUEUE_STATUS_WAS_DELETED,
  CORE_MESSAGE_QUEUE_STATUS_TIMEOUT
}   CORE_message_queue_Status;

/*
 *  The following defines the control block used to manage the 
 *  attributes of each message queue.
 */

typedef struct {
  CORE_message_queue_Disciplines  discipline;
}   CORE_message_queue_Attributes;
 
/*
 *  The following defines the type for a Notification handler.  A notification
 *  handler is invoked when the message queue makes a 0->1 transition on
 *  pending messages.
 */

typedef void (*CORE_message_queue_Notify_Handler)( void * );

/*
 *  The following defines the control block used to manage each 
 *  counting message_queue.
 */
 
typedef struct {
  Thread_queue_Control               Wait_queue;
  CORE_message_queue_Attributes      Attributes;
  unsigned32                         maximum_pending_messages;
  unsigned32                         number_of_pending_messages;
  unsigned32                         maximum_message_size;
  Chain_Control                      Pending_messages;
  CORE_message_queue_Buffer         *message_buffers;
  CORE_message_queue_Notify_Handler  notify_handler;
  void                              *notify_argument;
  Chain_Control                      Inactive_messages;
}   CORE_message_queue_Control;

/*
 *  _CORE_message_queue_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the message_queue based on the parameters passed.
 */

boolean _CORE_message_queue_Initialize(
  CORE_message_queue_Control    *the_message_queue,
  Objects_Classes                the_class,
  CORE_message_queue_Attributes *the_message_queue_attributes,
  unsigned32                     maximum_pending_messages,
  unsigned32                     maximum_message_size,
  Thread_queue_Extract_callout   proxy_extract_callout
);
 
/*
 *  _CORE_message_queue_Close
 *
 *  DESCRIPTION:
 *
 *  This function closes a message by returning all allocated space and
 *  flushing the message_queue's task wait queue.
 */
 
void _CORE_message_queue_Close(
  CORE_message_queue_Control *the_message_queue,
  Thread_queue_Flush_callout  remote_extract_callout,
  unsigned32                  status
);

/*
 *
 *  _CORE_message_queue_Flush
 *
 *  DESCRIPTION:
 *
 *  This function flushes the message_queue's task wait queue.  The number
 *  messages flushed from the queue is returned.
 *
 */

unsigned32 _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue
);

/*
 *  _CORE_message_queue_Flush_support
 *
 *  DESCRIPTION:
 *
 *  This routine flushes all outstanding messages and returns
 *  them to the inactive message chain.
 */
 
unsigned32 _CORE_message_queue_Flush_support(
  CORE_message_queue_Control *the_message_queue
);
 
/*
 *
 *  _CORE_message_queue_Broadcast
 *
 *  DESCRIPTION:
 *
 *  This function sends a message for every thread waiting on the queue and
 *  returns the number of threads made ready by the message.
 *
 */
 
CORE_message_queue_Status _CORE_message_queue_Broadcast(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  unsigned32                                *count
);

/*
 *
 *  _CORE_message_queue_Submit
 *
 *  DESCRIPTION:
 *
 *  This routine implements the send and urgent message functions. It
 *  processes a message that is to be submitted to the designated
 *  message queue.  The message will either be processed as a
 *  send message which it will be inserted at the rear of the queue
 *  or it will be processed as an urgent message which will be inserted
 *  at the front of the queue.
 *
 */
 
CORE_message_queue_Status _CORE_message_queue_Submit(
  CORE_message_queue_Control                *the_message_queue,
  void                                      *buffer,
  unsigned32                                 size,
  Objects_Id                                 id,
  CORE_message_queue_API_mp_support_callout  api_message_queue_mp_support,
  CORE_message_queue_Submit_types            submit_type
);

/*
 *
 *  _CORE_message_queue_Seize
 *
 *  DESCRIPTION:
 *
 *  This kernel routine dequeues a message, copies the message buffer to
 *  a given destination buffer, and frees the message buffer to the
 *  inactive message pool.  The thread will be blocked if wait is TRUE,
 *  otherwise an error will be given to the thread if no messages are available.
 *
 */
 
void _CORE_message_queue_Seize(
  CORE_message_queue_Control *the_message_queue,
  Objects_Id                  id,
  void                       *buffer,
  unsigned32                 *size,
  boolean                     wait,
  Watchdog_Interval           timeout
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/coremsg.inl>
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

