/*  signal.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Signal Manager.   This manager provides capabilities required
 *  for asynchronous communication between tasks via signal sets.
 *
 *  Directives provided are:
 *
 *     + establish an asynchronous signal routine
 *     + send a signal set to a task
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

#ifndef __RTEMS_SIGNAL_h
#define __RTEMS_SIGNAL_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/asr.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

/*
 *  _Signal_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _Signal_Manager_initialization( void );

/*
 *  rtems_signal_catch
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_signal_catch directive.  This directive
 *  is used to establish asr_handler as the Asynchronous Signal Routine
 *  (RTEMS_ASR) for the calling task.  The asr_handler will execute with a
 *  mode of mode_set.
 */

rtems_status_code rtems_signal_catch(
  rtems_asr_entry   asr_handler,
  rtems_mode        mode_set
);

/*
 *  rtems_signal_send
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_signal_send directive.  This directive
 *  sends the signal_set to the task specified by ID.
 */

rtems_status_code rtems_signal_send(
  Objects_Id             id,
  rtems_signal_set signal_set
);

#include <rtems/rtems/signalmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
