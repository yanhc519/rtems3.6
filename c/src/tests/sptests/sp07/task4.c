/*  Task_4
 *
 *  This routine serves as a low priority test task that should exit
 *  a soon as it runs to test the taskexitted user extension.
 *  execute.
 *
 *  Input parameters:
 *    argument - task argument
 *
 *  Output parameters:  NONE
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

#include "system.h"

rtems_task Task_4(
  rtems_task_argument argument
)
{
  puts( "TA4 - exitting task" );
}
