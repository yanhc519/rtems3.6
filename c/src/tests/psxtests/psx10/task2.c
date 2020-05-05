/*  Task_2
 *
 *  This routine serves as a test task.
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

void *Task_2(
  void *argument
)
{
  int  status;

  Task_id = pthread_self();
  printf( "Task_2: ID is 0x%08x\n", Task_id );

  status = pthread_mutex_lock( &Mutex_id );
  assert( !status );

  puts( "Task_2: pthread_cond_wait" );
  status = pthread_cond_wait( &Cond1_id, &Mutex_id );
  assert( !status );

  puts( "Task_2: back from pthread_cond_wait release mutex" );
  status = pthread_mutex_unlock( &Mutex_id );
  assert( !status );

  puts( "Task_2: task exit" );
  pthread_exit( NULL );

  return NULL; /* just so the compiler thinks we returned something */
}
