/*
 *  Multiprocessing Support for the Thread Handler
 *
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
#include <rtems/score/priority.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  _Thread_MP_Handler_initialization
 *
 */

void _Thread_MP_Handler_initialization (
  unsigned32  maximum_proxies
)
{

  _Chain_Initialize_empty( &_Thread_MP_Active_proxies );

  if ( maximum_proxies == 0 ) {
    _Chain_Initialize_empty( &_Thread_MP_Inactive_proxies );
    return;
  }


  _Chain_Initialize(
    &_Thread_MP_Inactive_proxies,
    _Workspace_Allocate_or_fatal_error(
      maximum_proxies * sizeof( Thread_Proxy_control )
    ),
    maximum_proxies,
    sizeof( Thread_Proxy_control )
  );

}

/*PAGE
 *
 *  _Thread_MP_Allocate_proxy
 *
 */

Thread_Control *_Thread_MP_Allocate_proxy (
  States_Control  the_state
)
{
  Thread_Control       *the_thread;
  Thread_Proxy_control *the_proxy;

  the_thread = (Thread_Control *)_Chain_Get( &_Thread_MP_Inactive_proxies );

  if ( !_Thread_Is_null( the_thread ) ) {

    the_proxy = (Thread_Proxy_control *) the_thread;

    _Thread_Executing->Wait.return_code = THREAD_STATUS_PROXY_BLOCKING;

    the_proxy->receive_packet = _Thread_MP_Receive->receive_packet;

    the_proxy->Object.id = _Thread_MP_Receive->receive_packet->source_tid;

    the_proxy->current_priority =
      _Thread_MP_Receive->receive_packet->source_priority;

    the_proxy->current_state = _States_Set( STATES_DORMANT, the_state );

    the_proxy->Wait = _Thread_Executing->Wait;

    _Chain_Append( &_Thread_MP_Active_proxies, &the_proxy->Active );

    return the_thread;
  }

  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    TRUE,
    INTERNAL_ERROR_OUT_OF_PROXIES
  );

  /*
   *  NOTE: The following return insures that the compiler will
   *        think that all paths return a value.
   */

  return NULL;
}

/*PAGE
 *
 *  _Thread_MP_Find_proxy
 *
 */

/*
 *  The following macro provides the offset of the Active element
 *  in the Thread_Proxy_control structure.  This is the logical
 *  equivalent of the POSITION attribute in Ada.
 */

#define _Thread_MP_Proxy_Active_offset \
     ((unsigned32)&(((Thread_Proxy_control *)0))->Active)

Thread_Control *_Thread_MP_Find_proxy (
  Objects_Id  the_id
)
{

  Chain_Node           *proxy_node;
  Thread_Control       *the_thread;
  ISR_Level             level;

restart:

  _ISR_Disable( level );

    for (  proxy_node = _Thread_MP_Active_proxies.first;
           !_Chain_Is_tail( &_Thread_MP_Active_proxies, proxy_node ) ;
        ) {

      the_thread = _Addresses_Subtract_offset(
                     proxy_node,
                     _Thread_MP_Proxy_Active_offset
                   );

      if ( _Objects_Are_ids_equal( the_thread->Object.id, the_id ) ) {
        _ISR_Enable( level );
        return the_thread;
      }

      _ISR_Flash( level );

      proxy_node = proxy_node->next;

      /*
       *  A proxy which is only dormant is not in a blocking state.
       *  Therefore, we are looking at proxy which has been moved from
       *  active to inactive chain (by an ISR) and need to restart
       *  the search.
       */

      if ( _States_Is_only_dormant( the_thread->current_state ) ) {
        _ISR_Enable( level );
        goto restart;
      }
    }

  _ISR_Enable( level );
  return NULL;
}
