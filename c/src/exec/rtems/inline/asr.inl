/*  inline/asr.inl
 *
 *  This include file contains the implemenation of all routines
 *  associated with the asynchronous signal handler which are inlined.
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

#ifndef __INLINE_ASR_inl
#define __INLINE_ASR_inl

#include <rtems/score/isr.h>

/*PAGE
 *
 *  _ASR_Initialize
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the given RTEMS_ASR information record.
 */

RTEMS_INLINE_ROUTINE void _ASR_Initialize (
  ASR_Information *information
)
{
  information->is_enabled      = TRUE;
  information->handler         = NULL;
  information->mode_set        = RTEMS_DEFAULT_MODES;
  information->signals_posted  = 0;
  information->signals_pending = 0;
  information->nest_level      = 0;
}

/*PAGE
 *
 *  _ASR_Swap_signals
 *
 *  DESCRIPTION:
 *
 *  This routine atomically swaps the pending and posted signal
 *  sets.  This is done when the thread alters its mode in such a
 *  way that the RTEMS_ASR disable/enable flag changes.
 */

RTEMS_INLINE_ROUTINE void _ASR_Swap_signals (
  ASR_Information *information
)
{
  rtems_signal_set _signals;
  ISR_Level        _level;

  _ISR_Disable( _level );
    _signals                     = information->signals_pending;
    information->signals_pending = information->signals_posted;
    information->signals_posted  = _signals;
  _ISR_Enable( _level );
}

/*PAGE
 *
 *  _ASR_Is_null_handler
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the given asr_handler is NULL and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _ASR_Is_null_handler (
  rtems_asr_entry asr_handler
)
{
  return asr_handler == NULL;
}

/*PAGE
 *
 *  _ASR_Are_signals_pending
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if there are signals pending in the
 *  given RTEMS_ASR information record and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _ASR_Are_signals_pending (
  ASR_Information *information
)
{
  return information->signals_posted != 0;
}

/*PAGE
 *
 *  _ASR_Post_signals
 *
 *  DESCRIPTION:
 *
 *  This routine posts the given signals into the signal_set
 *  passed in.  The result is returned to the user in signal_set.
 *
 *  NOTE:  This must be implemented as a macro.
 */

RTEMS_INLINE_ROUTINE void _ASR_Post_signals(
  rtems_signal_set  signals,
  rtems_signal_set *signal_set
)
{
  ISR_Level              _level;

  _ISR_Disable( _level );
    *signal_set |= signals;
  _ISR_Enable( _level );
}


#endif
/* end of include file */
