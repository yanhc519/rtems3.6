/*  Screen5
 *
 *  This routine generates error screen 5 for test 9.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

void Screen5()
{
  rtems_status_code status;

  status = rtems_semaphore_create(
    0,
    1,
    RTEMS_DEFAULT_ATTRIBUTES, 
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_create with illegal name"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_INVALID_NAME" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_DEFAULT_ATTRIBUTES,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 1 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 1 - RTEMS_SUCCESSFUL" );

  status = rtems_semaphore_create(
    Semaphore_name[ 2 ],
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY | RTEMS_INHERIT_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Semaphore_id[ 2 ]
  );
  directive_failed( status, "rtems_semaphore_create" );
  puts( "TA1 - rtems_semaphore_create - 2 - RTEMS_SUCCESSFUL" );

  do {
      status = rtems_semaphore_create(
          Semaphore_name[ 3 ],
          1,
          RTEMS_DEFAULT_ATTRIBUTES,
          RTEMS_NO_PRIORITY,
          &Junk_id
      );
  } while (status == RTEMS_SUCCESSFUL);

  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_semaphore_create of too many"
  );
  puts( "TA1 - rtems_semaphore_create - 3 - RTEMS_TOO_MANY" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_INHERIT_PRIORITY | RTEMS_BINARY_SEMAPHORE | RTEMS_FIFO,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of RTEMS_FIFO RTEMS_INHERIT_PRIORITY"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    1,
    RTEMS_INHERIT_PRIORITY | RTEMS_COUNTING_SEMAPHORE | RTEMS_PRIORITY,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_NOT_DEFINED,
    "rtems_semaphore_create of RTEMS_COUNTING_SEMAPHORE RTEMS_INHERIT_PRIORITY"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_NOT_DEFINED" );

  status = rtems_semaphore_create(
    Semaphore_name[ 1 ],
    2,
    RTEMS_BINARY_SEMAPHORE,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_semaphore_create of binary semaphore with count > 1"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_INVALID_NUMBER" );

  status = rtems_semaphore_create(
    Semaphore_name[ 3 ],
    1,
    RTEMS_GLOBAL,
    RTEMS_NO_PRIORITY,
    &Junk_id
  );
  fatal_directive_status(
    status,
    RTEMS_MP_NOT_CONFIGURED,
    "rtems_semaphore_create of mp not configured"
  );
  puts( "TA1 - rtems_semaphore_create - RTEMS_MP_NOT_CONFIGURED" );

  status = rtems_semaphore_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_delete with illegal id"
  );
  puts( "TA1 - rtems_semaphore_delete - unknown RTEMS_INVALID_ID" );

  status = rtems_semaphore_delete( 0x010100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_semaphore_delete with local illegal id"
  );
  puts( "TA1 - rtems_semaphore_delete - local RTEMS_INVALID_ID" );

  status = rtems_semaphore_ident( 100, RTEMS_SEARCH_ALL_NODES, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_ident will illegal name (local)"
  );
  puts( "TA1 - rtems_semaphore_ident - global RTEMS_INVALID_NAME" );

  status = rtems_semaphore_ident( 100, 1, &Junk_id );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NAME,
    "rtems_semaphore_ident will illegal name (global)"
  );
  puts( "TA1 - rtems_semaphore_ident - local RTEMS_INVALID_NAME" );
}
