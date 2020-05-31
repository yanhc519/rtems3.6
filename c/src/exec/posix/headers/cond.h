/*  rtems/posix/cond.h
 *
 *  This include file contains all the private support information for
 *  POSIX condition variables.
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
 
#ifndef __RTEMS_POSIX_CONDITION_VARIABLES_h
#define __RTEMS_POSIX_CONDITION_VARIABLES_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/threadq.h>

/*
 *  Constant to indicate condition variable does not currently have
 *  a mutex assigned to it.
 */

#define POSIX_CONDITION_VARIABLES_NO_MUTEX 0

/*
 *  Data Structure used to manage a POSIX condition variable
 */
 
typedef struct {
   Objects_Control       Object;
   int                   process_shared;
   pthread_mutex_t       Mutex;
   Thread_queue_Control  Wait_queue;
}  POSIX_Condition_variables_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Condition_variables_Information;
 
/*
 *  _POSIX_Condition_variables_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Condition_variables_Manager_initialization(
  unsigned32 maximum_condition_variables
);
 
/*
 *  _POSIX_Condition_variables_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a condition variable control block from
 *  the inactive chain of free condition variable control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control * 
  _POSIX_Condition_variables_Allocate( void );
 
/*
 *  _POSIX_Condition_variables_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a condition variable control block to the
 *  inactive chain of free condition variable control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Condition_variables_Free (
  POSIX_Condition_variables_Control *the_condition_variable
);
 
/*
 *  _POSIX_Condition_variables_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps condition variable IDs to condition variable control 
 *  blocks.  If ID corresponds to a local condition variable, then it returns
 *  the_condition variable control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the condition variable ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_condition variable is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_condition variable is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Condition_variables_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_condition variable is NULL 
 *  and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Condition_variables_Is_null (
  POSIX_Condition_variables_Control *the_condition_variable
);

#include <rtems/posix/cond.inl>
#include <rtems/posix/condmp.h>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

