/*
 *  $Id$
 */

#ifndef __POSIX_SIGNALS_h
#define __POSIX_SIGNALS_h

void _POSIX_signals_Manager_Initialization(
  int  maximum_queued_signals
);

void _POSIX_signals_Post_switch_extension(
  Thread_Control  *the_thread
);

#endif
/* end of file */
