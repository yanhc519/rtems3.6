/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the test set.
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

#include <tmacros.h>

/* macros */

/* functions */

rtems_task Init(
  rtems_task_argument argument
);

rtems_task Task_1_through_5(
  rtems_task_argument argument
);

void blow_stack( void );

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define TASK_STACK_SIZE (RTEMS_MINIMUM_STACK_SIZE*3)

#define CONFIGURE_EXTRA_TASK_STACKS (10 * RTEMS_MINIMUM_STACK_SIZE)

#include <confdefs.h>

/* global variables */

TEST_EXTERN rtems_id   Task_id[ 4 ];         /* array of task ids */
TEST_EXTERN rtems_name Task_name[ 4 ];       /* array of task names */

/* end of include file */
