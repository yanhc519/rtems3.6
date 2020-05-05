/*
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

#define TEST_INIT
#include "system.h"

rtems_id Queue_id;

long Buffer[4];

rtems_task test_init(
  rtems_task_argument argument
);

rtems_task High_task(
  rtems_task_argument argument
);

rtems_task Low_tasks(
  rtems_task_argument argument
);


rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_id          task_id;
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 12 ***" );

  status = rtems_task_create(
    1,
    251,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &task_id
  );
  directive_failed( status, "rtems_task_create" );

  status = rtems_task_start( task_id, test_init, 0 );
  directive_failed( status, "rtems_task_start" );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

rtems_task test_init(
  rtems_task_argument argument
)
{
  rtems_unsigned32     index;
  rtems_task_entry     task_entry;
  rtems_task_priority  priority;
  rtems_id             task_id;
  rtems_status_code    status;


  status = rtems_message_queue_create(
    rtems_build_name( 'M', 'Q', '1', ' ' ),
    OPERATION_COUNT,
    16,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Queue_id
  );
  directive_failed( status, "rtems_message_queue_create" );

  priority = 250;

  for( index = 0; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      priority,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &task_id
    );
    directive_failed( status, "rtems_task_create LOOP" );

    priority--;

    if ( index==OPERATION_COUNT ) task_entry = High_task;
    else                          task_entry = Low_tasks;

    status = rtems_task_start( task_id, task_entry, 0 );
    directive_failed( status, "rtems_task_start LOOP" );
  }
}

rtems_task High_task(
  rtems_task_argument argument
)
{
  rtems_unsigned32  index;

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) Empty_function();
  overhead = Read_timer();

  Timer_initialize();
    for ( index=1 ; index <= OPERATION_COUNT ; index++ )
      (void) rtems_message_queue_send( Queue_id, (long (*)[4]) Buffer, 16 );
  end_time = Read_timer();

  put_time(
    "rtems_message_queue_send: task readied -- returns to caller",
    end_time,
    OPERATION_COUNT,
    overhead,
    CALLING_OVERHEAD_MESSAGE_QUEUE_SEND
  );

  puts( "*** END OF TEST 12 ***" );
  exit( 0 );
}

rtems_task Low_tasks(
  rtems_task_argument argument
)
{
  rtems_unsigned32 size;

  (void) rtems_message_queue_receive(
           Queue_id,
           (long (*)[4]) Buffer,
           &size,
           RTEMS_DEFAULT_OPTIONS,
           RTEMS_NO_TIMEOUT
         );
}
