/*
 *  Workspace Handler
 *
 *  XXX
 *
 *  NOTE:
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

#include <rtems/system.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>

/*PAGE
 *
 *  _Workspace_Handler_initialization
 */
 
void _Workspace_Handler_initialization(
  void       *starting_address,
  unsigned32  size
)
{
  unsigned32 *zero_out_array;
  unsigned32  index;
  unsigned32  memory_available;
 
  if ( !starting_address || !_Addresses_Is_aligned( starting_address ) )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INVALID_WORKSPACE_ADDRESS
    );
 
  if ( _CPU_Table.do_zero_of_workspace ) {
    for( zero_out_array  = (unsigned32 *) starting_address, index = 0 ;
         index < size / sizeof( unsigned32 ) ;
         index++ )
      zero_out_array[ index ] = 0;
  }
 
  memory_available = _Heap_Initialize(
    &_Workspace_Area,
    starting_address,
    size,
    CPU_HEAP_ALIGNMENT
  );
 
  if ( memory_available == 0 )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_TOO_LITTLE_WORKSPACE
    );
}

/*PAGE
 *
 *  _Workspace_Allocate_or_fatal_error
 *
 */

void *_Workspace_Allocate_or_fatal_error(
  unsigned32   size
)
{
  void        *memory;

  memory = _Workspace_Allocate( size );

  if ( memory == NULL )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_WORKSPACE_ALLOCATION
    );

  return memory;
}
