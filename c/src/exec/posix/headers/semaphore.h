/*  rtems/posix/semaphore.h
 *
 *  This include file contains all the private support information for
 *  POSIX Semaphores.
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
 
#ifndef __RTEMS_POSIX_SEMAPHORE_h
#define __RTEMS_POSIX_SEMAPHORE_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coresem.h>

/*
 *  Data Structure used to manage a POSIX semaphore
 */
 
typedef struct {
   Objects_Control         Object;
   int                     process_shared;
   boolean                 named;
   boolean                 linked;
   unsigned32              open_count;
   CORE_semaphore_Control  Semaphore;
}  POSIX_Semaphore_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Semaphore_Information;
 
/*
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Semaphore_Manager_initialization(
  unsigned32 maximum_semaphorees
);
 
/*
 *  _POSIX_Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void );
 
/*
 *  _POSIX_Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
);
 
/*
 *  _POSIX_Semaphore_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  Objects_Id        *id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Semaphore_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_semaphore is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Semaphore_Is_null (
  POSIX_Semaphore_Control *the_semaphore
);

/*
 *  _POSIX_Semaphore_Name_to_id
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

int _POSIX_Semaphore_Name_to_id(
  const char          *name,
  Objects_Id          *id
);

#include <rtems/posix/semaphore.inl>
#include <rtems/posix/semaphoremp.h>

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

