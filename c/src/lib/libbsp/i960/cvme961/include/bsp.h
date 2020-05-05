/*  bsp.h
 *
 *  This include file contains some definitions specific to the
 *  Cyclone CVME960 and CVME961 boards.  These boards are the
 *  same except the 960 uses SRAM and the 961 DRAM.
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

#ifndef __CVME961_h
#define __CVME961_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are for i960CA and are board independent
 *
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 6, 1 )

#define Cause_tm27_intr()  i960_cause_intr( 0x62 )

#define Clear_tm27_intr()  i960_clear_intr( 6 )

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp; \
    asm volatile( "0: \
                     remo      3,31,%0 ; \
                     cmpo      0,%0 ; \
                     subo      1,%1,%1 ; \
                     cmpobne.t 0,%1,0b " \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* Constants */

#define RAM_START 0
#define RAM_END   0x100000

/* NINDY console I/O requests:
 *   CO sends a single character to stdout,
 *   CI reads one.
 */

#define NINDY_INPUT   0
#define NINDY_OUTPUT  1

/*
 *  get_prcb
 *
 *  Returns the PRCB pointer.
 */

static inline i960ca_PRCB *get_prcb( void )
{
  register i960ca_PRCB *_prcb = 0;

  asm volatile( "calls 5; \
                 mov   g0,%0" \
                 : "=d" (_prcb) \
                 : "0" (_prcb) );
  return ( _prcb );
}

#ifdef C961_INIT
#undef BSP_EXTERN
#define BSP_EXTERN
#else
#undef BSP_EXTERN
#define BSP_EXTERN extern
#endif

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

BSP_EXTERN i960ca_PRCB          *Prcb;
BSP_EXTERN i960ca_control_table *Ctl_tbl;

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_cleanup( void );

i960_isr_entry set_vector( rtems_isr_entry, unsigned int, unsigned int );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
