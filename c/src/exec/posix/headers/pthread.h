/*  rtems/posix/pthread.h
 *
 *  This include file contains all the private support information for
 *  POSIX threads.
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
 
#ifndef __RTEMS_POSIX_THREADS_h
#define __RTEMS_POSIX_THREADS_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/posix/config.h>
#include <rtems/posix/threadsup.h>

#define PTHREAD_MINIMUM_STACK_SIZE (STACK_MINIMUM_SIZE * 2)

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Threads_Information;

/*
 *  These are used to manage the user initialization threads.
 */
 
POSIX_EXTERN posix_initialization_threads_table 
                   *_POSIX_Threads_User_initialization_threads;
POSIX_EXTERN unsigned32   _POSIX_Threads_Number_of_initialization_threads;

 
/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Threads_Manager_initialization(
  unsigned32                          maximum_pthreads,
  unsigned32                          number_of_initialization_threads,
  posix_initialization_threads_table *user_threads
);
 
/*
 *  _POSIX_Threads_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a pthread control block from
 *  the inactive chain of free pthread control blocks.
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate( void );
 
/*
 *  _POSIX_Threads_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a pthread control block to the
 *  inactive chain of free pthread control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free(
  Thread_Control *the_pthread
);
 
/*
 *  _POSIX_Threads_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps pthread IDs to pthread control blocks.
 *  If ID corresponds to a local pthread, then it returns
 *  the_pthread control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the pthread ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_pthread is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_pthread is undefined.
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Get(
  pthread_t          id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Threads_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_pthread is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Threads_Is_null(
  Thread_Control *the_pthread
);

#include <rtems/posix/pthread.inl>
#include <rtems/posix/pthreadmp.h>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

