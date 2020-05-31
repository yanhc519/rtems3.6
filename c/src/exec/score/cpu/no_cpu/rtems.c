/*  rtems.c  ===> rtems.S or rtems.s
 *
 *  This file contains the single entry point code for
 *  the XXX implementation of RTEMS.
 *
 *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
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

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" rtems file.
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
/* #include "asm.h> */

/*
 *  RTEMS
 *
 *  This routine jumps to the directive indicated in the
 *  CPU defined register.  This routine is used when RTEMS is
 *  linked by itself and placed in ROM.  This routine is the
 *  first address in the ROM space for RTEMS.  The user "calls"
 *  this address with the directive arguments in the normal place.
 *  This routine then jumps indirectly to the correct directive
 *  preserving the arguments.  The directive should not realize
 *  it has been "wrapped" in this way.  The table "_Entry_points"
 *  is used to look up the directive.
 */

void RTEMS()
{
}

