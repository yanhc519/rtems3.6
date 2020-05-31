/*  macros/address.h
 *
 *  This include file contains the bodies of the routines
 *  about addresses which are inlined.
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

#ifndef __MACROS_ADDRESSES_h
#define __MACROS_ADDRESSES_h

/*PAGE
 *
 *  _Addresses_Add_offset
 *
 */

#define _Addresses_Add_offset( _base, _offset ) \
  ((void *)((char *)(_base) + (_offset)))

/*PAGE
 *
 *  _Addresses_Subtract_offset
 *
 */

#define _Addresses_Subtract_offset( _base, _offset ) \
  ((void *)((char *)(_base) - (_offset)))

/*PAGE
 *
 *  _Addresses_Subtract
 *
 *  NOTE:  The cast of an address to an unsigned32 makes this code
 *         dependent on an addresses being thirty two bits.
 */

#define _Addresses_Subtract( _left, _right ) \
  ((void *)(_left) - (void *)(_right))

/*PAGE
 *
 *  _Addresses_Is_aligned
 *
 */

#define _Addresses_Is_aligned( _address ) \
    ( ( (unsigned32)(_address) % 4 ) == 0 )

/*PAGE
 *
 *  _Addresses_Is_in_range
 *
 */

#define _Addresses_Is_in_range( _address, _base, _limit ) \
  ( (_address) >= (_base) && (_address) <= (_limit) )

#endif
/* end of include file */
