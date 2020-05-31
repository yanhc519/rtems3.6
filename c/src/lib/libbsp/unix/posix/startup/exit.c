/*
 *  exit
 *
 *  This routine returns control to "the pre-RTEMS environment".
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

#include <bsp.h>
#include <clockdrv.h>

void _exit( void )
{
   /* Clock or Timer cleanup is run by at_exit() */

   Io_cleanup();

   bsp_cleanup();
}
