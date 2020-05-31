/*  system.h
 *
 *  This include file is used by all tests in the Time Suite.
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


#include <coverhd.h>
#include <tmacros.h>

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_timer_service_routine Timer_handler(
  rtems_id argument
);
 
rtems_asr Isr_handler(
  rtems_signal_set signals
);
 
rtems_asr Asr_handler(
  rtems_signal_set signals
);
 
rtems_task task_func();
 
void null_func();
 
rtems_status_code Empty_directive();

#include <timesys.h>

/* configuration information */
 
#define CONFIGURE_TMTEST
 
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              2
#define CONFIGURE_TICKS_PER_TIMESLICE        0
 
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <confdefs.h>

/* end of include file */
