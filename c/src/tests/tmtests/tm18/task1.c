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

rtems_unsigned32 taskcount;
rtems_task_priority taskpri;

rtems_task First_task(
  rtems_task_argument argument
);

rtems_task Middle_tasks(
  rtems_task_argument argument
);

rtems_task Last_task(
  rtems_task_argument argument
);


void test_init();

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Print_Warning();

  puts( "\n\n*** TIME TEST 18 ***" );

  test_init();

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}

void test_init()
{
  rtems_id          id;
  rtems_task_entry  task_entry;
  rtems_unsigned32  index;
  rtems_status_code status;

  for ( index = 0 ; index <= OPERATION_COUNT ; index++ ) {
    status = rtems_task_create(
      rtems_build_name( 'T', 'I', 'M', 'E' ),
      128,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &id
    );
    directive_failed( status, "rtems_task_create loop" );

    if ( index == OPERATION_COUNT ) task_entry = Last_task;
    else if ( index == 0 )          task_entry = First_task;
    else                            task_entry = Middle_tasks;


    status = rtems_task_start( id, task_entry, 0 );
    directive_failed( status, "rtems_task_start loop" );
  }

}

rtems_task First_task(
  rtems_task_argument argument
)
{
  Timer_initialize();

  (void) rtems_task_delete( RTEMS_SELF );
}

rtems_task Middle_tasks(
  rtems_task_argument argument
)
{
  (void) rtems_task_delete( RTEMS_SELF );
}

rtems_task Last_task(
  rtems_task_argument argument
)
{
  end_time = Read_timer();

  put_time(
    "rtems_task_delete: calling task",
    end_time,
    OPERATION_COUNT,
    0,
    CALLING_OVERHEAD_TASK_DELETE
  );

  puts( "*** END OF TEST 18 ***" );
  exit( 0 );
}