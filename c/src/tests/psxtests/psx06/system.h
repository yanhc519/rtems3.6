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

/* functions */

#include <pmacros.h>

void *POSIX_Init(
  void *argument
);

void *Task_1(
  void *argument
);

void *Task_2(
  void *argument
);

/* configuration information */

#define CONFIGURE_SPTEST

#define CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_TEST_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_MAXIMUM_POSIX_KEYS     1

#include <confdefs.h>

/* global variables */

#ifdef CONFIGURE_INIT
#define TEST_EXTERN
#else
#define TEST_EXTERN extern
#endif

TEST_EXTERN pthread_t        Init_id;
TEST_EXTERN pthread_t        Task_id;
TEST_EXTERN pthread_t        Task2_id;
TEST_EXTERN pthread_key_t    Key_id;
TEST_EXTERN rtems_unsigned32 Data_array[ CONFIGURE_MAXIMUM_POSIX_THREADS ];
TEST_EXTERN rtems_unsigned32 Destructor_invoked;

/* end of include file */
