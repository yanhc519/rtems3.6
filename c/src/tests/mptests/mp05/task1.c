/*  Test_task
 *
 *  This task initializes the signal catcher, sends the first signal
 *  if running on the first node, and loops while waiting for signals.
 *
 *  NOTE: The signal catcher is not reentrant and hence RTEMS_NO_ASR must
 *        be a part of its execution mode.
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

#define SIGNALS_PER_DOT 15

rtems_timer_service_routine Stop_Test_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Stop_Test = TRUE;
}

rtems_task Test_task(
  rtems_task_argument argument
)
{
  rtems_status_code status;

  Stop_Test = FALSE;

  signal_caught = 0;
  signal_count  = 0;

  puts( "rtems_signal_catch: initializing signal catcher" );
  status = rtems_signal_catch( Process_asr, RTEMS_NO_ASR|RTEMS_NO_PREEMPT );
  directive_failed( status, "rtems_signal_catch" );

  if (Multiprocessing_configuration.node == 1) {
     remote_node = 2;
     remote_signal  = RTEMS_SIGNAL_18;
     expected_signal = RTEMS_SIGNAL_17;
  }
  else {
     remote_node = 1;
     remote_signal  = RTEMS_SIGNAL_17;
     expected_signal = RTEMS_SIGNAL_18;
  }
  puts_nocr( "Remote task's name is : " );
  put_name( Task_name[ remote_node ], TRUE );

  puts( "Getting TID of remote task" );
  do {
      status = rtems_task_ident(
          Task_name[ remote_node ],
          RTEMS_SEARCH_ALL_NODES,
          &remote_tid
          );
  } while ( status != RTEMS_SUCCESSFUL );
  directive_failed( status, "rtems_task_ident" );

  status = rtems_timer_fire_after(
    Timer_id[ 1 ],
    3 * TICKS_PER_SECOND,
    Stop_Test_TSR,
    NULL
  );
  directive_failed( status, "rtems_timer_fire_after" );

  if ( Multiprocessing_configuration.node == 1 ) {
    puts( "Sending signal to remote task" );
    do {
      status = rtems_signal_send( remote_tid, remote_signal );
      if ( status == RTEMS_NOT_DEFINED )
        continue;
    } while ( status != RTEMS_SUCCESSFUL );
    directive_failed( status, "rtems_signal_send" );
  }

  while ( Stop_Test == FALSE ) {
    if ( signal_caught ) {
      signal_caught = 0;
      if ( ++signal_count >= SIGNALS_PER_DOT ) {
        signal_count = 0;
        put_dot( '.' );
      }
      status = rtems_signal_send( remote_tid, remote_signal );
      directive_failed( status, "rtems_signal_send" );
    }
  }
  puts( "\n*** END OF TEST 5 ***" );
  exit( 0 );
}
