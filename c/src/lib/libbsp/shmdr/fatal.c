/*  void MPCI_Fatal( error )
 *
 *  This routine is the shared memory driver fatal error handler.
 *
 *  Input parameters:
 *    error - fatal error code
 *
 *  Output parameters:  NEVER RETURNS
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

void MPCI_Fatal(
  Internal_errors_Source  source,
  boolean                 is_internal,
  rtems_unsigned32        error
)
{
   /*   Eventually need to attempt to broadcast a K_FATAL message
    *   without checking for all possible errors (do not want to
    *   recurse).
    *
    *   Also need to avoid using Shm_Node_statuses if the driver has not been
    *   initialized.
    */

   Shm_Local_node_status->error = Shm_Convert(error);
}
