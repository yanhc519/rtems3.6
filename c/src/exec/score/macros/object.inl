/*  object.inl
 *
 *  This include file contains the macro implementation of all
 *  of the inlined routines in the Object Handler.
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

#ifndef __OBJECTS_inl
#define __OBJECTS_inl

/*PAGE
 *
 *  _Objects_Build_id
 *
 */

#define _Objects_Build_id( _the_class, _node, _index ) \
  ( ((_the_class) << OBJECTS_CLASS_START_BIT) | \
    ((_node) << OBJECTS_NODE_START_BIT)       | \
    ((_index) << OBJECTS_INDEX_START_BIT) )

/*PAGE
 *
 *  _Objects_Get_class
 */
 
#define _Objects_Get_class( _id ) \
  (Objects_Classes) \
    (((_id) >> OBJECTS_CLASS_START_BIT) & OBJECTS_CLASS_VALID_BITS)

/*PAGE
 *
 *  _Objects_Get_node
 *
 */

#define _Objects_Get_node( _id ) \
  (((_id) >> OBJECTS_NODE_START_BIT) & OBJECTS_NODE_VALID_BITS)

/*PAGE
 *
 *  _Objects_Get_index
 *
 */

#define _Objects_Get_index( _id ) \
  (((_id) >> OBJECTS_INDEX_START_BIT) & OBJECTS_INDEX_VALID_BITS)

/*PAGE
 *
 *  _Objects_Is_class_valid
 *
 */
 
#define _Objects_Is_class_valid( _the_class ) \
  ( (_the_class) && (_the_class) <= OBJECTS_CLASSES_LAST )

/*PAGE
 *
 *  _Objects_Is_local_node
 *
 */

#define _Objects_Is_local_node( _node ) \
  ( (_node) == _Objects_Local_node )

/*PAGE
 *
 *  _Objects_Is_local_id
 *
 */

#define _Objects_Is_local_id( _id ) \
  _Objects_Is_local_node( _Objects_Get_node(_id) )

/*PAGE
 *
 *  _Objects_Are_ids_equal
 *
 */

#define _Objects_Are_ids_equal( _left, _right ) \
  ( (_left) == (_right) )

/*PAGE
 *
 *  _Objects_Allocate
 *
 */

#define _Objects_Allocate( _information )  \
  (Objects_Control *) _Chain_Get( &(_information)->Inactive )

/*PAGE
 *
 *  _Objects_Free
 *
 */

#define _Objects_Free( _information, _the_object )  \
  _Chain_Append( &(_information)->Inactive, &(_the_object)->Node )

/*PAGE
 *
 *  _Objects_Open
 *
 */

#define _Objects_Open( _information, _the_object, _name ) \
  { \
    unsigned32 _index; \
    \
    _index = _Objects_Get_index( (_the_object)->id ); \
    (_information)->local_table[ _index ] = (_the_object); \
    \
    if ( (_information)->is_string ) \
      _Objects_Copy_name_string( (_name), (_the_object)->name ); \
    else \
      _Objects_Copy_name_raw( \
        (_name), (_the_object)->name, (_information)->name_length ); \
  }

/*PAGE
 *
 *  _Objects_Close
 *
 */

#define _Objects_Close( _information, _the_object ) \
  { \
    unsigned32 _index; \
    \
    _index = _Objects_Get_index( (_the_object)->id ); \
    (_information)->local_table[ _index ] = (Objects_Control *) NULL; \
    _Objects_Clear_name( (_the_object)->name, (_information)->name_length ); \
  }

#endif
/* end of include file */
