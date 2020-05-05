/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the executive.  This must be the first include file
 *  included in all internal RTEMS files.
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

#ifndef __RTEMS_SYSTEM_h
#define __RTEMS_SYSTEM_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_USE_INLINE_OR_MACROS" is replaced
 *        with either "USE_INLINES" or "USE_MACROS" based upon the
 *        whether this target configuration uses the inline or
 *        macro implementations of the inlined RTEMS routines.
 */
 

#define REPLACE_THIS_WITH_USE_INLINE_OR_MACROS

/*
 *  The following insures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is 
 *  referenced as "external" in every other file.
 */

#ifdef SCORE_INIT
#undef  SCORE_EXTERN
#define SCORE_EXTERN
#else
#undef  SCORE_EXTERN
#define SCORE_EXTERN  extern
#endif

#ifdef SAPI_INIT
#undef  SAPI_EXTERN
#define SAPI_EXTERN
#else
#undef  SAPI_EXTERN
#define SAPI_EXTERN  extern
#endif

#ifdef RTEMS_API_INIT
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN
#else
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN  extern
#endif

#ifdef POSIX_API_INIT
#undef  POSIX_EXTERN
#define POSIX_EXTERN
#else
#undef  POSIX_EXTERN
#define POSIX_EXTERN  extern
#endif

/*
 *  The following (in conjunction with compiler arguments) are used
 *  to choose between the use of static inline functions and macro
 *  functions.   The static inline implementation allows better
 *  type checking with no cost in code size or execution speed.
 */

#ifdef USE_INLINES
# ifdef __GNUC__
#  define RTEMS_INLINE_ROUTINE static __inline__
# else
#  define RTEMS_INLINE_ROUTINE static inline
# endif
#else
# define RTEMS_INLINE_ROUTINE
#endif

/*
 *  Include a base set of files.
 */

/*
 * XXX: Eventually proc_ptr needs to disappear!!!
 */

typedef void * proc_ptr;

#include <rtems/score/cpu.h>        /* processor specific information */

/*
 *  Define NULL
 */

#ifndef NULL
#define NULL      0          /* NULL value */
#endif

/*
 *  Boolean constants
 */

#if !defined( TRUE ) || (TRUE != 1)
#undef TRUE
#define TRUE     (1)
#endif

#if !defined( FALSE ) || (FALSE != 0)
#undef FALSE
#define FALSE     (0)
#endif

#define stringify( _x ) # _x

#define RTEMS_offsetof(type, field) \
	((unsigned32) &(((type *) 0)->field))

/*
 *  The following is the extern for the RTEMS version string.
 *  The contents of this string are CPU specific.
 */

extern const char _RTEMS_version[];         /* RTEMS version string */
extern const char _Copyright_Notice[];      /* RTEMS copyright string */

/*
 *  The following defines the CPU dependent information table.
 */

SCORE_EXTERN rtems_cpu_table _CPU_Table;               /* CPU dependent info */

/*
 *  XXX weird RTEMS stuff
 */

#define RTEMS_MAXIMUM_NAME_LENGTH 4

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
