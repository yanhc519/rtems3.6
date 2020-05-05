/*  Task_1
 *
 *  This routine serves as a test task.  It verifies intertask communication
 *  using task notepads and verifies the the user extensions (tcreate, etc).
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
  rtems_status_code   status;
  rtems_task_priority the_priority;
  rtems_task_priority previous_priority;

  rtems_test_pause();

  status = rtems_task_set_priority(
    RTEMS_SELF,
    RTEMS_CURRENT_PRIORITY,
    &the_priority
  );
  directive_failed( status, "rtems_task_set_priority" );
  printf(
    "TA1 - rtems_task_set_priority - get initial priority of self: %02d\n",
    the_priority
  );

  while( FOREVER ) {
    status = rtems_task_get_note( RTEMS_SELF, RTEMS_NOTEPAD_8, &the_priority );
    directive_failed( status, "rtems_task_get_note" );
    printf(
"TA1 - rtems_task_get_note - get RTEMS_NOTEPAD_8 - current priority: %02d\n",
       the_priority
    );

    if ( --the_priority == 0 ) {
      puts( "TA1 - rtems_task_suspend - suspend TA2" );
      status = rtems_task_suspend( Task_id[ 2 ] );
      directive_failed( status, "rtems_task_suspend" );

      puts( "TA1 - rtems_task_set_priority - set priority of TA2 ( blocked )" );
      status = rtems_task_set_priority( Task_id[ 2 ], 5, &previous_priority );
      directive_failed( status, "rtems_task_set_priority" );

      status = rtems_task_delete( Task_id[ 2 ] );
      directive_failed( status, "rtems_task_delete of TA2" );

      status = rtems_task_delete( RTEMS_SELF );
      directive_failed( status, "rtems_task_delete of RTEMS_SELF" );
    }

    printf( "TA1 - rtems_task_set_note - set TA2's RTEMS_NOTEPAD_8: %02d\n",
            the_priority
    );
    status = rtems_task_set_note( Task_id[ 2 ], RTEMS_NOTEPAD_8, the_priority );
    directive_failed( status, "rtems_task_set_note" );

    printf( "TA1 - rtems_task_set_priority - set TA2's priority: %02d\n",
            the_priority
    );
    status = rtems_task_set_priority(
               Task_id[ 2 ],
               the_priority,
               &previous_priority
    );
    directive_failed( status, "rtems_task_set_priority" );
  }
}
