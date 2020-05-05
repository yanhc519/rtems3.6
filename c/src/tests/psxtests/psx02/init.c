/* 
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

#define CONFIGURE_INIT
#include "system.h"
#include <signal.h>

volatile int Signal_occurred;
volatile int Signal_count;

void Signal_handler(
  int signo
)
{
  Signal_count++;
  printf(
    "Signal: %d caught by 0x%x (%d)\n",
    signo,
    pthread_self(),
    Signal_count
  );
  Signal_occurred = 1;
}

void *POSIX_Init(
  void *argument
)
{
  int               status;
  struct timespec   tv;
  struct timespec   tr;
  struct sigaction  act;
  sigset_t          mask;
  sigset_t          pending_set;

  puts( "\n\n*** POSIX TEST 2 ***" );

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  Init_id = pthread_self();
  printf( "Init's ID is 0x%08x\n", Init_id );

  /* install a signal handler */

  status = sigemptyset( &act.sa_mask );
  assert( !status );

  act.sa_handler = Signal_handler;
  act.sa_flags   = 0;
 
  sigaction( SIGUSR1, &act, NULL );

  /* simple signal to self */

  Signal_count = 0;
  Signal_occurred = 0;

  status = pthread_kill( Init_id, SIGUSR1 );
  assert( !status );

  Signal_occurred = 0;

  /* now block the signal, send it, see if it is pending, and unblock it */

  status = sigemptyset( &mask );
  assert( !status );

  status = sigaddset( &mask, SIGUSR1 );
  assert( !status );

  printf( "Init: Block SIGUSR1\n" );
  status = sigprocmask( SIG_BLOCK, &mask, NULL );
  assert( !status );

  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );
  
  
  printf( "Init: send SIGUSR1 to self\n" );
  status = pthread_kill( Init_id, SIGUSR1 );
  assert( !status );

  status = sigpending( &pending_set );
  assert( !status );
  printf( "Init: Signals pending 0x%08x\n", pending_set );
  
  printf( "Init: Unblock SIGUSR1\n" );
  status = sigprocmask( SIG_UNBLOCK, &mask, NULL );
  assert( !status );

  /* create a thread */

  status = pthread_create( &Task_id, NULL, Task_1_through_3, NULL );
  assert( !status );

  /*
   *  Loop for 5 seconds seeing how many signals we catch 
   */

  tr.tv_sec = 5;
  tr.tv_nsec = 0;
 
  do {
    tv = tr;

    Signal_occurred = 0;

    status = nanosleep ( &tv, &tr );
  
    if ( status == -1 ) {
      assert( errno == EINTR );
      assert( tr.tv_nsec || tr.tv_sec );
    } else if ( !status ) {
      assert( !tr.tv_nsec && !tr.tv_sec );
    }
     
    printf(
      "Init: signal was %sprocessed with %d:%d time remaining\n",
      (Signal_occurred) ? "" : "not ",
      (int) tr.tv_sec,
      (int) tr.tv_nsec
   );

  } while ( tr.tv_sec || tr.tv_nsec );

  /* exit this thread */

  puts( "*** END OF POSIX TEST 2 ***" );
  exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}
