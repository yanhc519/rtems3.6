/*  userext.h
 *
 *  This include file contains all information about user extensions.  This
 *  Handler provides mechanisms which can be used to initialize and manipulate
 *  all user extensions.
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

#ifndef __USER_EXTENSIONS_h
#define __USER_EXTENSIONS_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/interr.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

/*
 *  The following records defines the User Extension Table.
 *  This table defines the application dependent routines which
 *  are invoked at critical points in the life of each thread and
 *  the system as a whole.
 */
 
typedef void User_extensions_routine;
 
typedef boolean ( *User_extensions_thread_create_extension )(
                 Thread_Control *,
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_delete_extension )(
                 Thread_Control *,
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_start_extension )(
                 Thread_Control *,
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_restart_extension )(
                 Thread_Control *,
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_switch_extension )(
                 Thread_Control *,
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_begin_extension )(
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_thread_exitted_extension )(
                 Thread_Control *
             );
 
typedef User_extensions_routine ( *User_extensions_fatal_extension )(
                 Internal_errors_Source  /* the_source  */,
                 boolean                 /* is_internal */,
                 unsigned32              /* the_error   */
             );

 
typedef struct {
  User_extensions_thread_create_extension       thread_create;
  User_extensions_thread_start_extension        thread_start;
  User_extensions_thread_restart_extension      thread_restart;
  User_extensions_thread_delete_extension       thread_delete;
  User_extensions_thread_switch_extension       thread_switch;
  User_extensions_thread_begin_extension        thread_begin;
  User_extensions_thread_exitted_extension      thread_exitted;
  User_extensions_fatal_extension               fatal;
}   User_extensions_Table;

/*
 *  The following is used to manage each user extension set.
 */

typedef struct {
  Chain_Node              Node;
  User_extensions_Table   Callouts;
}   User_extensions_Control;

/*
 *  The following contains the static extension set which may be
 *  configured by the application.
 */

SCORE_EXTERN User_extensions_Control _User_extensions_Initial;

/*
 *  The following is used to manage the list of active extensions.
 */

SCORE_EXTERN Chain_Control _User_extensions_List;

/*
 *  _User_extensions_Thread_create
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the thread creation operate.
 */

boolean _User_extensions_Thread_create (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Thread_delete
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the thread deletion operation.
 */

void _User_extensions_Thread_delete (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Thread_start
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the thread start operation.
 */

void _User_extensions_Thread_start (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Thread_restart
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension for
 *  the thread restart operation.
 */

void _User_extensions_Thread_restart (
  Thread_Control *the_thread
);

/*
 *  _User_extensions_Thread_begin
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread begins.
 */

void _User_extensions_Thread_begin (
  Thread_Control *executing
);

/*
 *  _User_extensions_Thread_exitted
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a thread exits.
 */

void _User_extensions_Thread_exitted (
  Thread_Control *executing
);

/*
 *  _User_extensions_Fatal
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension invoked
 *  when a fatal error occurs.
 */

void _User_extensions_Fatal (
  Internal_errors_Source  the_source,
  boolean                 is_internal,
  unsigned32              the_error
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/userext.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
