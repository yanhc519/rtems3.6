/*  Screen4
 *
 *  This routine generates error screen 4 for test 9.
 *
 *  Input parameters:  NONE
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

void Screen4()
{
  rtems_event_set   event_out;
  rtems_time_of_day time;
  rtems_status_code status;

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ALL)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( all conditions )" );

  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_EVENT_ANY | RTEMS_NO_WAIT,
    RTEMS_NO_TIMEOUT,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_UNSATISFIED,
    "rtems_event_receive unsatisfied (ANY)"
  );
  puts( "TA1 - rtems_event_receive - RTEMS_UNSATISFIED ( any condition )" );

  puts( "TA1 - rtems_event_receive - timeout in 3 seconds" );
  status = rtems_event_receive(
    RTEMS_EVENT_16,
    RTEMS_DEFAULT_OPTIONS,
    3 * TICKS_PER_SECOND,
    &event_out
  );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_event_receive"
  );
  puts( "TA1 - rtems_event_receive - woke up with RTEMS_TIMEOUT" );

  status = rtems_event_send( 100, RTEMS_EVENT_16 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_event_send with illegal id"
  );
  puts( "TA1 - rtems_event_send - RTEMS_INVALID_ID" );

  puts( "TA1 - rtems_task_wake_after - sleep 1 second - RTEMS_SUCCESSFUL" );
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed( status, "rtems_task_wake_after" );

  build_time( &time, 2, 5, 1988, 8, 30, 45, 0 );
  print_time( "TA1 - rtems_clock_set - ", &time, "" );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );
  puts( " - RTEMS_SUCCESSFUL" );
}
