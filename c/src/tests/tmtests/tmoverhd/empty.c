/*
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

rtems_timer_service_routine Timer_handler(
  rtems_id argument
)
{
}

rtems_asr Isr_handler(
  rtems_signal_set signals
)
{
}

rtems_asr Asr_handler(
  rtems_signal_set signals
)
{
}

rtems_task task_func() {}

void null_func() {}

rtems_status_code Empty_directive()
{
  return( RTEMS_SUCCESSFUL );
}
