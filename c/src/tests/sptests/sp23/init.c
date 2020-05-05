/*  Init
 *
 *  This routine is the initialization task for this test program.
 *  It is a user initialization task and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  puts( "\n\n*** TEST 23 ***" );

  Task_name[ 1 ] =  rtems_build_name( 'T', 'A', '1', ' ' );

  status = rtems_task_create(
    Task_name[ 1 ],
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id[ 1 ]
  );
  directive_failed( status, "rtems_task_create of TA1" );

  status = rtems_task_start( Task_id[ 1 ], Task_1, 0 );
  directive_failed( status, "rtems_task_start of TA1" );

  Port_name[ 1 ] = rtems_build_name( 'D', 'P', '1', ' ' );

  status = rtems_port_create(
    Port_name[ 1 ],
    Internal_port_area,
    External_port_area,
    0xff,
    &Port_id[ 1 ]
  );
  directive_failed( status, "rtems_port_create of DP1" );

  printf( "INIT - rtems_port_create - DP1 - int = %p   ext = %p\n",
          Internal_port_area,
          External_port_area
  );

  status = rtems_task_delete( RTEMS_SELF );
  directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
}
