/*  bsp.h
 *
 *  This include file contains all DMV152 board IO definitions.
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

#ifndef __DMV152_h
#define __DMV152_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <iosupp.h>
#include <vmeintr.h>
#include <z8530.h>
#include <z8536.h>

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
 */

/* use a VMEbus interrupt */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( handler ) \
  { \
    set_vector( (handler), 0x50, 1 );  \
    (*(volatile rtems_unsigned32 *)0x0d800024) = 0x50; /* set IVECT reg */ \
    (*(volatile rtems_unsigned8  *)0x0d00000c) = 0x40; /* set VIE reg */ \
  }

#define Cause_tm27_intr()  \
  (*(volatile rtems_unsigned8 *)0x0d000003) = 0x0f  /* set VINT */

#define Clear_tm27_intr()  /* no operation necessary */

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp=123; \
    asm volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* macros */

#undef Z8x36_STATE0
#undef Z8x36_WRITE
#undef Z8x36_READ

#define Z8x36_STATE0 ( z8536 ) \
  { char *garbage; \
    (garbage) = *(VOL8(z8536+0x7)) \
  }

#define Z8x36_WRITE( z8536, reg, data ) \
   *(VOL8(z8536+0x7)) = (reg); \
   *(VOL8(z8536+0x7)) = (data)

#define Z8x36_READ( z8536, reg, data ) \
   *(VOL8(z8536+0x7)) = (reg); \
   (data) = *(VOL8(z8536+0x7))

/*
 *  ACC Register Addresses
 */

#define ACC_BASE    0x0D000000

#define ACC_STAT0   ((volatile rtems_unsigned8 *) (ACC_BASE + 0x00))
#define ACC_STAT1   ((volatile rtems_unsigned8 *) (ACC_BASE + 0x01))
#define ACC_GENCTL  ((volatile rtems_unsigned8 *) (ACC_BASE + 0x02))
#define ACC_VINT    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x03))
#define ACC_VREQ    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x04))
#define ACC_VARB    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x05))
#define ACC_ID      ((volatile rtems_unsigned8 *) (ACC_BASE + 0x06))
#define ACC_CTL2    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x07))
#define ACC_7IS     ((volatile rtems_unsigned8 *) (ACC_BASE + 0x08))
#define ACC_LIS     ((volatile rtems_unsigned8 *) (ACC_BASE + 0x09))
#define ACC_7IE     ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0A))
#define ACC_LIE     ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0B))
#define ACC_VIE     ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0C))
#define ACC_IC10    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0D))
#define ACC_IC32    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0E))
#define ACC_IC54    ((volatile rtems_unsigned8 *) (ACC_BASE + 0x0F))

/* constants */

#define RAM_START 0
#define RAM_END   0x100000

#define USE_CHANNEL_A   0                /* 1 = use channel A for console */
#define USE_CHANNEL_B   1                /* 1 = use channel B for console */

#define TIMER 0x0c000000
#define TIMER_VECTOR 0x4D

#if (USE_CHANNEL_A == 1)
#define CONSOLE_CONTROL  0x0C800005
#define CONSOLE_DATA     0x0C800007
#elif (USE_CHANNEL_B == 1)
#define CONSOLE_CONTROL  0x0C800001
#define CONSOLE_DATA     0x0C800003
#endif

/* Structures */

#ifdef D152_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr_entry M68Kvec[];   /* vector table address */

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

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
