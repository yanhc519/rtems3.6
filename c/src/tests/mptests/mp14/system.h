/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
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

#include <tmacros.h>

/* functions */

rtems_timer_service_routine Delayed_send_event(
  rtems_id  timer_id,
  void     *ignored_address
);

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Test_task(
  rtems_task_argument argument
);

rtems_task Delayed_events_task(
  rtems_task_argument argument
);

rtems_task Message_queue_task(
  rtems_task_argument index
);

rtems_task Partition_task(
  rtems_task_argument argument
);

rtems_task Semaphore_task(
  rtems_task_argument argument
);

void Exit_test( void );

/* configuration information */
 
#define CONFIGURE_MPTEST
 
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
 
#define CONFIGURE_TICKS_PER_TIMESLICE   1
#define CONFIGURE_MICROSECONDS_PER_TICK RTEMS_MILLISECONDS_TO_MICROSECONDS(1)

#if ( NODE_NUMBER == 1 )
#define CONFIGURE_MAXIMUM_TIMERS              12
#define CONFIGURE_MAXIMUM_SEMAPHORES          1
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES      1
#define CONFIGURE_MAXIMUM_PARTITIONS          1
#elif ( NODE_NUMBER == 2 )
#define CONFIGURE_MAXIMUM_TIMERS              2
#endif

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <confdefs.h>

/* variables */

TEST_EXTERN rtems_id   Event_task_id[ 2 ];       /* event task ids */

TEST_EXTERN rtems_id   Semaphore_task_id[ 2 ];   /* semaphore task ids */
TEST_EXTERN rtems_name Semaphore_task_name[ 2 ]; /* semaphore task names */

TEST_EXTERN rtems_id   Queue_task_id[ 3 ];       /* message queue task ids */
TEST_EXTERN rtems_name Queue_task_name[ 3 ];     /* message queue task names */

TEST_EXTERN rtems_id   Partition_task_id[ 4 ];   /* partition task ids */
TEST_EXTERN rtems_name Partition_task_name[ 4 ]; /* partition task names */

TEST_EXTERN rtems_id   Partition_id[ 3 ];    /* partition ids */
TEST_EXTERN rtems_name Partition_name[ 3 ];  /* partition names */

TEST_EXTERN rtems_id   Semaphore_id[ 3 ];    /* semaphore ids */
TEST_EXTERN rtems_name Semaphore_name[ 3 ];  /* semaphore names */

TEST_EXTERN rtems_id   Queue_id[ 3 ];        /* message queue ids */
TEST_EXTERN rtems_name Queue_name[ 3 ];      /* message queue names */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* remote event task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* remote event task names */

TEST_EXTERN rtems_id   Timer_id[ 4 ];         /* event timer ids */
TEST_EXTERN rtems_name Timer_name[ 4 ];       /* event timer names */

TEST_EXTERN rtems_unsigned32 Msg_buffer[ 4 ][ 4 ];

extern rtems_multiprocessing_table Multiprocessing_configuration;

TEST_EXTERN volatile rtems_boolean Stop_Test;
TEST_EXTERN rtems_id timer_id;

#define EVENT_TASK_DOT_COUNT        100
#define EVENT_SEND_DOT_COUNT        100
#define DELAYED_EVENT_DOT_COUNT     1000
#define MESSAGE_DOT_COUNT           200
#define PARTITION_DOT_COUNT         200
#define SEMAPHORE_DOT_COUNT         200

/* end of include file */
