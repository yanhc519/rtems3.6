/*  config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the POSIX API.
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

#ifndef __RTEMS_POSIX_CONFIGURATION_h
#define __RTEMS_POSIX_CONFIGURATION_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  XXX
 *
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + required number of each object type
 */

/*
 *  For now, we are only allowing the user to specify the entry point
 *  for posix initialization threads.
 */

typedef struct {
  void       *(*entry)(void *);
} posix_initialization_threads_table;

typedef struct {
  int                                 maximum_threads;
  int                                 maximum_mutexes;
  int                                 maximum_condition_variables;
  int                                 maximum_keys;
  int                                 maximum_queued_signals;
  int                                 number_of_initialization_threads;
  posix_initialization_threads_table *User_initialization_threads_table;
} posix_api_configuration_table;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
