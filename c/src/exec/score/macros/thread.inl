/*  thread.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Thread handler.
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

#ifndef __THREAD_inl
#define __THREAD_inl

/*PAGE
 *
 *  _Thread_Stop_multitasking
 *
 */

#define _Thread_Stop_multitasking() \
  _Context_Switch( &_Thread_Executing->Registers, &_Thread_BSP_context );

/*PAGE
 *
 *  _Thread_Is_executing
 *
 */

#define _Thread_Is_executing( _the_thread ) \
        ( (_the_thread) == _Thread_Executing )

/*PAGE
 *
 *  _Thread_Is_heir
 *
 */

#define _Thread_Is_heir( _the_thread ) \
        ( (_the_thread) == _Thread_Heir )

/*PAGE
 *
 *  _Thread_Is_executing_also_the_heir
 *
 */

#define _Thread_Is_executing_also_the_heir() \
        ( _Thread_Executing == _Thread_Heir )

/*PAGE
 *
 *  _Thread_Resume
 *
 */

#define _Thread_Resume( _the_thread ) \
        _Thread_Clear_state( (_the_thread), STATES_SUSPENDED )

/*PAGE
 *
 *  _Thread_Unblock
 *
 */

#define _Thread_Unblock( _the_thread ) \
        _Thread_Clear_state( (_the_thread), STATES_BLOCKED );

/*PAGE
 *
 *  _Thread_Restart_self
 *
 */

#define _Thread_Restart_self()  \
  {  \
     if ( _Thread_Executing->fp_context != NULL ) \
       _Context_Restore_fp( &_Thread_Executing->fp_context ); \
     \
    _CPU_Context_Restart_self( &_Thread_Executing->Registers ); \
  }

/*PAGE
 *
 *  _Thread_Calculate_heir
 *
 */

#define _Thread_Calculate_heir() \
 { \
   Priority_Control  highest; \
   \
   _Priority_Get_highest( highest ); \
   \
   _Thread_Heir = (Thread_Control *) _Thread_Ready_chain[ highest ].first; \
  }

/*PAGE
 *
 *  _Thread_Is_allocated_fp
 *
 */

#define _Thread_Is_allocated_fp( _the_thread ) \
        ( (_the_thread) == _Thread_Allocated_fp )

/*PAGE
 *
 *  _Thread_Deallocate_fp
 *
 */

#define _Thread_Deallocate_fp() \
        _Thread_Allocated_fp = NULL

/*PAGE
 *
 *  _Thread_Disable_dispatch
 *
 */

#define _Thread_Disable_dispatch() \
  _Thread_Dispatch_disable_level += 1

/*PAGE
 *
 *  _Thread_Enable_dispatch
 *
 */

#if ( CPU_INLINE_ENABLE_DISPATCH == TRUE )
#define _Thread_Enable_dispatch()  \
      { if ( (--_Thread_Dispatch_disable_level) == 0 ) \
             _Thread_Dispatch();  \
      }
#endif

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )
void _Thread_Enable_dispatch( void );
#endif

/*PAGE
 *
 *  _Thread_Unnest_dispatch
 *
 */

#define _Thread_Unnest_dispatch()  \
  _Thread_Dispatch_disable_level -= 1

/*PAGE
 *
 *  _Thread_Is_dispatching_enabled
 *
 */

#define _Thread_Is_dispatching_enabled() \
  ( _Thread_Dispatch_disable_level == 0 )

/*PAGE
 *
 *  _Thread_Is_context_switch_necessary
 *
 */

#define _Thread_Is_context_switch_necessary() \
  ( _Context_Switch_necessary == TRUE )

/*PAGE
 *
 *  _Thread_Dispatch_initialization
 *
 */

#define _Thread_Dispatch_initialization() \
  _Thread_Dispatch_disable_level = 1

/*PAGE
 *
 *  _Thread_Is_null
 *
 */

#define _Thread_Is_null( _the_thread ) \
  ( (_the_thread) == NULL )

/*
 *  _Thread_Is_proxy_blocking
 *
 */
 
#define _Thread_Is_proxy_blocking( _code ) \
  ( (_code) == THREAD_STATUS_PROXY_BLOCKING )

/*
 *  _Thread_Internal_allocate
 *
 */
 
#define _Thread_Internal_allocate() \
  ((Thread_Control *) _Objects_Allocate( &_Thread_Internal_information ))
 
/*
 *  _Thread_Internal_free
 *
 */
 
#define _Thread_Internal_free( _the_task ) \
  _Objects_Free( &_Thread_Internal_information, &(_the_task)->Object )

#endif
/* end of include file */
