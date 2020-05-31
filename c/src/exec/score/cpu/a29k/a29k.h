/*  a29k.h
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 *
 */
/* @(#)a29k.h       10/21/96     1.3 */

#ifndef _INCLUDE_A29K_h
#define _INCLUDE_A29K_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "no cpu"
 *  family when executing in protected mode.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 */
 
#if defined(a29205)
 
#define CPU_MODEL_NAME  "a29205"
#define A29K_HAS_FPU     0
 
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "AMD 29K"

/*
 * Some bits in the CPS:
 */
#define	TD	0x20000
#define	DI	0x00002

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_A29K_h */
/* end of include file */
