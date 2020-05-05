/*  tqdata.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines needed to support the Thread Queue Data.
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

#ifndef __THREAD_QUEUE_DATA_inl
#define __THREAD_QUEUE_DATA_inl

/*PAGE
 *
 *  _Thread_queue_Header_number
 *
 *  DESCRIPTION:
 *
 *  This function returns the index of the priority chain on which
 *  a thread of the_priority should be placed.
 */

RTEMS_INLINE_ROUTINE unsigned32 _Thread_queue_Header_number (
  Priority_Control the_priority
)
{
  return (the_priority / TASK_QUEUE_DATA_PRIORITIES_PER_HEADER);
}

/*PAGE
 *
 *  _Thread_queue_Is_reverse_search
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_priority indicates that the
 *  enqueue search should start at the front of this priority
 *  group chain, and FALSE if the search should start at the rear.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_queue_Is_reverse_search (
  Priority_Control the_priority
)
{
  return ( the_priority & TASK_QUEUE_DATA_REVERSE_SEARCH_MASK );
}

/*PAGE
 *
 *  _Thread_queue_Enter_critical_section
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked to indicate that the specified thread queue is
 *  entering a critical section.
 */
 
RTEMS_INLINE_ROUTINE void _Thread_queue_Enter_critical_section (
  Thread_queue_Control *the_thread_queue
)
{
  the_thread_queue->sync_state = THREAD_QUEUE_NOTHING_HAPPENED;
}

#endif
/* end of include file */
