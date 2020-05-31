/*  options.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Options Handler.
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

#ifndef __OPTIONS_inl
#define __OPTIONS_inl

/*PAGE
 *
 *  _Options_Is_no_wait
 *
 */

#define _Options_Is_no_wait( _option_set ) \
   ( (_option_set) & RTEMS_NO_WAIT )

/*PAGE
 *
 *  _Options_Is_any
 *
 */

#define _Options_Is_any( _option_set ) \
   ( (_option_set) & RTEMS_EVENT_ANY )

#endif
/* end of include file */
