/*  Shm_Convert_address
 *
 *  This routine takes into account the peculiar short VME address
 *  of the CVME961 board.  The CVME961 maps short address space
 *  0xffffxxxx to 0xb400xxxx.
 *
 *  Input parameters:
 *    address - address to convert
 *
 *  Output parameters:
 *    returns - converted address
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
#include <bsp.h>
#include "shm.h"

void *Shm_Convert_address(
  void *address
)
{
  rtems_unsigned32 workaddr = (rtems_unsigned32) address;

  if ( workaddr >= 0xffff0000 )
    workaddr = (workaddr & 0xffff) | 0xb4000000;
  return ( (rtems_unsigned32 *)workaddr );
}
