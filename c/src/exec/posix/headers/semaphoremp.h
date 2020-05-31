/*  semaphoremp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the POSIX Semaphore Manager.
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

#ifndef __RTEMS_POSIX_SEMAPHORE_MP_h
#define __RTEMS_POSIX_SEMAPHORE_MP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  The following enumerated type defines the list of
 *  remote semaphore operations.
 */

typedef enum {
  POSIX_SEMAPHORE_MP_ANNOUNCE_CREATE  =  0,
  POSIX_SEMAPHORE_MP_ANNOUNCE_DELETE  =  1,
  POSIX_SEMAPHORE_MP_EXTRACT_PROXY    =  2,
  POSIX_SEMAPHORE_MP_OBTAIN_REQUEST   =  3,
  POSIX_SEMAPHORE_MP_OBTAIN_RESPONSE  =  4,
  POSIX_SEMAPHORE_MP_RELEASE_REQUEST  =  5,
  POSIX_SEMAPHORE_MP_RELEASE_RESPONSE =  6,
}   POSIX_Semaphore_MP_Remote_operations;

/*
 *  The following data structure defines the packet used to perform
 *  remote semaphore operations.
 */

typedef struct {
  MP_packet_Prefix                      Prefix;
  POSIX_Semaphore_MP_Remote_operations  operation;
  Objects_Name                          name;
  boolean                               wait;  /* XXX options */
  Objects_Id                            proxy_id;
}   POSIX_Semaphore_MP_Packet;

/*
 *  _POSIX_Semaphore_MP_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */

void _POSIX_Semaphore_MP_Send_process_packet (
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                            semaphore_id,
  Objects_Name                          name,
  Objects_Id                            proxy_id
);

/*
 *  _POSIX_Semaphore_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

int _POSIX_Semaphore_MP_Send_request_packet (
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                            semaphore_id,
  boolean                               wait,  /* XXX options */
  Watchdog_Interval                     timeout
);

/*
 *  _POSIX_Semaphore_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _POSIX_Semaphore_MP_Send_response_packet (
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                        semaphore_id,
  Thread_Control                   *the_thread
);

/*
 *
 *  _POSIX_Semaphore_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _POSIX_Semaphore_MP_Process_packet (
  MP_packet_Prefix *the_packet_prefix
);

/*
 *  _POSIX_Semaphore_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */

void _POSIX_Semaphore_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
);

/*
 *  _POSIX_Semaphore_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */

void _POSIX_Semaphore_MP_Send_extract_proxy (
  Thread_Control *the_thread
);

/*
 *  _POSIX_Semaphore_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a semaphore mp packet.
 */

POSIX_Semaphore_MP_Packet *_POSIX_Semaphore_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
