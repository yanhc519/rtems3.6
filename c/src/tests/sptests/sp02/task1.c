/*  Task_1
 *
 *  This routine serves as a test task.  It verifies the task manager.
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

#include "system.h"

rtems_task Task_1(
  rtems_task_argument argument
)
{
  rtems_id          tid2;
  rtems_id          tid3;
  rtems_status_code status;
  rtems_unsigned32  previous_priority;

  puts( "TA1 - rtems_task_wake_after - sleep 1 second" );
  status = rtems_task_wake_after( 1*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  status = rtems_task_ident( Task_name[ 2 ], RTEMS_SEARCH_ALL_NODES, &tid2 );
  directive_failed( status, "rtems_task_ident of TA2" );

  printf( "TA1 - rtems_task_ident - tid of TA2 (0x%.8x)\n", tid2 );

  status = rtems_task_ident( Task_name[ 3 ], RTEMS_SEARCH_ALL_NODES, &tid3 );
  directive_failed( status, "rtems_task_ident of TA3" );

  printf( "TA1 - rtems_task_ident - tid of TA3 (0x%.8x)\n", tid3 );

  status = rtems_task_set_priority( tid3, 2, &previous_priority );
  directive_failed( status, "rtems_task_set_priority" );

  puts( "TA1 - rtems_task_set_priority - set TA3's priority to 2" );


  puts( "TA1 - rtems_task_suspend - suspend TA2" );
  status = rtems_task_suspend( tid2 );
  directive_failed( status, "rtems_task_suspend of TA2" );

  puts( "TA1 - rtems_task_delete - delete TA2" );
  status = rtems_task_delete( tid2 );
  directive_failed( status, "rtems_task_delete of TA2" );

  puts( "TA1 - rtems_task_wake_after - sleep for 5 seconds" );
  status = rtems_task_wake_after( 5*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "*** END OF TEST 2 ***" );
  exit( 0 );
}
