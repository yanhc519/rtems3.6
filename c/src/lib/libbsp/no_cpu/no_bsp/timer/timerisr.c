/*  timerisr.s
 *
 *  If required this ISR is used to bump a count of interval "overflow"
 *  interrupts which have occurred since the timer was started.  The
 *  number of overflows is taken into account in the Read_timer()
 *  routine if necessary.
 *
 *  To reduce overhead this is best to be the "rawest" hardware interupt
 *  handler you can write.  This should be the only interrupt which can
 *  occur during the measured time period.
 *
 *  NOTE:  This file is USUALLY in assembly and is LEAN AND MEAN.
 *         Any code in this isr is pure overhead which can perturb
 *         the accuracy of the Timing Test Suite.
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

#include <rtems.h>

extern rtems_unsigned32 _Timer_interrupts;

void timerisr( void )
{
  /*
   *  _Timer_interrupts += TIMER_BETWEEN_OVERFLOWS  (usually in microseconds)
   *  return from interrupt
   */
}
