/*
 *  exit
 *
 *  This routine returns control to DOS.
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

#include <rtems.h>
#include <bsp.h>
#include <clockdrv.h>
#include <iodrv.h>

#if 0
/* Prefer to pick up _exit() in djgcc */
void _exit( )
{
   Io_cleanup();
   bsp_cleanup();
}
#endif

