/*  put_error
 *
 *  This routine verifies that the given error is the expected error.
 *
 *  Input parameters:
 *    error    - actual error code
 *    expected - expected error code
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

char *Errors[] = {
  "RTEMS_SUCCESSFUL",               /* successful completion */
  "RTEMS_TASK_EXITTED",             /* returned from a task */
  "RTEMS_MP_NOT_CONFIGURED",        /* multiprocessing not configured */
  "RTEMS_INVALID_NAME",             /* invalid object name */
  "RTEMS_INVALID_ID",               /* invalid object id */
  "RTEMS_TOO_MANY",                 /* too many */
  "RTEMS_TIMEOUT",                  /* timed out waiting */
  "RTEMS_OBJECT_WAS_DELETED",       /* object was deleted while waiting */
  "RTEMS_INVALID_SIZE",             /* specified size was invalid */
  "RTEMS_INVALID_ADDRESS",          /* address specified is invalid */
  "RTEMS_INVALID_NUMBER",           /* number was invalid */
  "RTEMS_NOT_DEFINED",              /* item has not been initialized */
  "RTEMS_RESOURCE_IN_USE",          /* resources still outstanding */
  "RTEMS_UNSATISFIED",              /* request not satisfied */
  "RTEMS_INCORRECT_STATE",          /* task is in wrong state */
  "RTEMS_ALREADY_SUSPENDED",        /* task already in state */
  "RTEMS_ILLEGAL_ON_SELF",          /* illegal operation on calling task */
  "RTEMS_ILLEGAL_ON_REMOTE_OBJECT", /* illegal operation for remote object */
  "RTEMS_CALLED_FROM_ISR",          /* called from ISR */
  "RTEMS_INVALID_PRIORITY",         /* invalid task priority */
  "RTEMS_INVALID_CLOCK",            /* invalid date/time */
  "RTEMS_INVALID_NODE",             /* invalid node id */
  "RTEMS_NOT_OWNER_OF_RESOURCE",    /* not owner of resource */
  "RTEMS_NOT_CONFIGURED",           /* directive not configured */
  "RTEMS_NOT_IMPLEMENTED"           /* directive not implemented */
};

/* Task states */

void put_error(
  rtems_unsigned32  error,
  rtems_status_code expected
)
{

  if ( error <= RTEMS_NOT_IMPLEMENTED )
    printf( "EXPECTED FATAL - error code is correctly %s\n", Errors[ error ] );
  else
    printf( "ERROR - out of range error code is %d\n", error );

  if ( error != expected ) {
    printf( "ERROR - did not get expected code of %d\n", expected );
  }
}
