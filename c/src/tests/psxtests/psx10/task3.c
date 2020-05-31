/*  Task_3
 *
 *  This routine serves as a test task.
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

void *Task_3(
  void *argument
)
{
  int  status;

  Task_id = pthread_self();
  printf( "Task_3: ID is 0x%08x\n", Task_id );

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

  puts( "Task_3: pthread_cond_wait" );
  status = pthread_cond_wait( &Cond1_id, &Mutex_id );
  if ( status != EINVAL )
    printf( "status = %d\n", status );
  assert( status == EINVAL );
  puts( "Task_3: pthread_cond_wait - EINVAL (mutex not locked after signal)");

  puts( "Task_3: task exit" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
