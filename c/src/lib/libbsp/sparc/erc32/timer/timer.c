/*  timer.c
 *
 *  This file implements a benchmark timer using the General Purpose Timer on
 *  the MEC.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */


#include <bsp.h>

rtems_boolean Timer_driver_Find_average_overhead;

rtems_boolean Timer_driver_Is_initialized = FALSE;

void Timer_initialize()
{
  /*
   *  Timer runs long and accurate enough not to require an interrupt.
   */

  if ( Timer_driver_Is_initialized == FALSE ) {

    /* approximately 1 us per countdown */
    ERC32_MEC.General_Purpose_Timer_Scalar  = CLOCK_SPEED - 1;
    ERC32_MEC.General_Purpose_Timer_Counter = 0xffffffff;

  } else {
    Timer_driver_Is_initialized = TRUE;
  }

  ERC32_MEC_Set_General_Purpose_Timer_Control( 
    ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING | 
      ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER
  );

  ERC32_MEC_Set_General_Purpose_Timer_Control( 
    ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING
  );

}

#define AVG_OVERHEAD      3  /* It typically takes 3.0 microseconds */
                             /*     to start/stop the timer. */
#define LEAST_VALID       2  /* Don't trust a value lower than this */

int Read_timer()
{
  rtems_unsigned32  total;

  total = ERC32_MEC.General_Purpose_Timer_Counter;

  total = 0xffffffff - total;

  if ( Timer_driver_Find_average_overhead == 1 )
    return total;          /* in one microsecond units */

  if ( total < LEAST_VALID )
    return 0;            /* below timer resolution */

  return total - AVG_OVERHEAD;
}

rtems_status_code Empty_function( void )
{
  return RTEMS_SUCCESSFUL;
}

void Set_find_average_overhead(
  rtems_boolean find_flag
)
{
  Timer_driver_Find_average_overhead = find_flag;
}
