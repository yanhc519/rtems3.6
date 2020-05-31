/*  void Shm_Locked_queue_Add( lq_cb, ecb )
 *
 *  This routine adds an envelope control block to a shared memory queue.
 *
 *  Input parameters:
 *    lq_cb - pointer to a locked queue control block
 *    ecb   - pointer to an envelope control block
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

void Shm_Locked_queue_Add(
  Shm_Locked_queue_Control *lq_cb,
  Shm_Envelope_control     *ecb
)
{
  rtems_unsigned32 index;

  ecb->next  = Shm_Locked_queue_End_of_list;
  ecb->queue = lq_cb->owner;
  index      = ecb->index;

  Shm_Lock( lq_cb );
    if ( Shm_Convert(lq_cb->front) != Shm_Locked_queue_End_of_list )
      Shm_Envelopes[ Shm_Convert(lq_cb->rear) ].next = index;
    else
      lq_cb->front = index;
    lq_cb->rear  = index;
  Shm_Unlock( lq_cb );
}
