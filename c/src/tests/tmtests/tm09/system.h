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

#include <timesys.h>

/* configuration information */
 
#define CONFIGURE_TMTEST
 
#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES     1
#define CONFIGURE_TICKS_PER_TIMESLICE        0
 
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_EXTRA_TASK_STACKS         (1 * RTEMS_MINIMUM_STACK_SIZE)

#include <confdefs.h>

/* end of include file */
