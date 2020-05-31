/*  void Shm_Locked_queue_Initialize( lq_cb, owner )
 *
 *  This routine initializes a shared memory locked queue.
 *
 *  Input parameters:
 *    lq_cb - pointer to the control block of the queue
 *            to be initialized
 *    owner - unique idenitifier of who owns this queue.
 *
 *  Output parameters: NONE
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

#include <rtems.h>
#include "shm.h"

void Shm_Locked_queue_Initialize(
  Shm_Locked_queue_Control *lq_cb,
  rtems_unsigned32 owner
)
{
  Shm_Initialize_lock( lq_cb );
  lq_cb->front = Shm_Locked_queue_End_of_list;
  lq_cb->rear  = Shm_Locked_queue_End_of_list;
  lq_cb->owner = Shm_Convert(owner);
}
