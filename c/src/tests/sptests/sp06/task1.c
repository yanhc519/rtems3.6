/*  Task_1
 *
 *  This task serves as a test task.  Verifies that a task can restart
 *  both itself and other tasks.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

#include "system.h"

rtems_task Task_1( argument )
rtems_unsigned32 argument;
{
  rtems_unsigned32  pass;
  rtems_status_code status;

  puts( "TA1 - is beginning to run" );

  pass = argument + 1;

  if ( pass == 5 ) {
    puts( "*** END OF TEST 6 ***" );
    exit( 0 );
  }

  if ( pass == 1 ) {
    puts( "TA1 - restarting TA3" );
    status = rtems_task_restart( Task_id[ 3 ], Restart_argument );
    directive_failed( status, "rtems_task_restart of TA3" );

    status = rtems_task_wake_after( 2 * TICKS_PER_SECOND );
    directive_failed( status, "rtems_task_wake_after" );
  }

  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting TA2" );
  status = rtems_task_restart( Task_id[ 2 ], Restart_argument );
  directive_failed( status, "rtems_task_restart TA2" );

  status = rtems_task_wake_after( 1*TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  puts( "TA1 - rtems_task_restart - restarting self" );
  status = rtems_task_restart( Task_id[ 1 ], pass );
  directive_failed( status, "rtems_task_restart of RTEMS_SELF" );
}
