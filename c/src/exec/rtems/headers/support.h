/*  support.h
 *
 *  This include file contains information about support functions for
 *  the RTEMS API.
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

#ifndef __RTEMS_RTEMS_SUPPORT_h
#define __RTEMS_RTEMS_SUPPORT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>

/*
 *  rtems_build_name
 *
 *  DESCRIPTION:
 *
 *  This function returns an object name composed of the four characters
 *  C1, C2, C3, and C4.
 *
 *  NOTE:
 *
 *  This must be implemented as a macro for use in Configuration Tables.
 *
 */
 
#define rtems_build_name( _C1, _C2, _C3, _C4 ) \
  ( (_C1) << 24 | (_C2) << 16 | (_C3) << 8 | (_C4) )
 
/*
 *  rtems_get_class
 *
 *  DESCRIPTION:
 *
 *  This function returns the class portion of the ID.
 *
 */
 
#define rtems_get_class( _id ) \
  _Objects_Get_class( _id )
 
/*
 *  rtems_get_node
 *
 *  DESCRIPTION:
 *
 *  This function returns the node portion of the ID.
 *
 */
 
#define rtems_get_node( _id ) \
  _Objects_Get_node( _id )
 
/*
 *  rtems_get_index
 *
 *  DESCRIPTION:
 *
 *  This function returns the index portion of the ID.
 *
 */
 
#define rtems_get_index( _id ) \
  _Objects_Get_index( _id )

/*
 *  Time related
 */

#define RTEMS_MILLISECONDS_TO_MICROSECONDS(_ms) \
        TOD_MILLISECONDS_TO_MICROSECONDS(_ms)
#define RTEMS_MILLISECONDS_TO_TICKS(_ms) \
        TOD_MILLISECONDS_TO_TICKS(_ms)

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/support.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */