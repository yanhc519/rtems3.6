/*  Clock_init()
 *
 *  This routine initializes the Z80386 1 on the MVME136 board.
 *  The tick frequency is 1 millisecond.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <z8036.h>

#define MICRVAL     0xe2            /* disable lower chain, no vec */
                                    /*  set right justified addr */
                                    /*  and master int enable */
#define MCCRVAL     0xc4            /* enable T1 and port B */
                                    /*   timers independent */
#define MS_COUNT    0x07d0          /* T1's countdown constant (1 ms) */
#define T1MSRVAL    0x80            /* T1 cont. cycle/pulse output */
#define T1CSRVAL    0xc6            /* enable interrupt, allow and */
                                    /*   and trigger countdown */

#define TIMER        0xfffb0000
#define RELOAD       0x24            /* clr IP & IUS,allow countdown */
 
#define CLOCK_VECTOR 66

rtems_unsigned32 Clock_isrs;        /* ISRs until next tick */

volatile rtems_unsigned32 Clock_driver_ticks; /* ticks since initialization */

rtems_isr_entry  Old_ticker;

void Clock_exit( void );

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 
/*
 *  ISR Handler
 */
 
rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  Clock_driver_ticks += 1;
  ((volatile struct z8036_map *) TIMER)->CT1_CMD_STATUS = RELOAD;

  if ( Clock_isrs == 1 ) {
    rtems_clock_tick();
    Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;
  }
  else
    Clock_isrs -= 1;
}

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  volatile struct z8036_map *timer;

  Clock_driver_ticks = 0;
  Clock_isrs = BSP_Configuration.microseconds_per_tick / 1000;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    Old_ticker = (rtems_isr_entry) set_vector( clock_isr, CLOCK_VECTOR, 1 );
    timer = (struct z8036_map *) 0xfffb0000;
    timer->MASTER_INTR        = MICRVAL;
    timer->CT1_MODE_SPEC      = T1MSRVAL;

    *((rtems_unsigned16 *)0xfffb0016) = MS_COUNT;  /* write countdown value */

    /*
     *  timer->CT1_TIME_CONST_MSB = (MS_COUNT >> 8);
     *  timer->CT1_TIME_CONST_LSB = (MS_COUNT &  0xff);
     */

    timer->MASTER_CFG         = MCCRVAL;
    timer->CT1_CMD_STATUS     = T1CSRVAL;

    /*
     * Enable interrupt via VME interrupt mask register
     */
    (*(rtems_unsigned8 *)0xfffb0038) &= 0xfd;

    atexit( Clock_exit );
  }

}

void Clock_exit( void )
{
  volatile struct z8036_map *timer;

  if ( BSP_Configuration.ticks_per_timeslice ) {
    timer = (struct z8036_map *) 0xfffb0000;
    timer->MASTER_INTR        = 0x62;
    timer->CT1_MODE_SPEC      = 0x00;
    timer->MASTER_CFG         = 0xf4;
    timer->CT1_CMD_STATUS     = 0x00;
    /* do not restore old vector */
  }
}

rtems_device_driver Clock_initialize(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  Install_clock( Clock_isr );

  /*
   * make major/minor avail to others such as shared memory driver
   */

  rtems_clock_major = major;
  rtems_clock_minor = minor;
 
  return RTEMS_SUCCESSFUL;
}

rtems_device_driver Clock_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
    rtems_unsigned32 isrlevel;
    rtems_libio_ioctl_args_t *args = pargp;
 
    if (args == 0)
        goto done;
 
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(CLOCK_VECTOR);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
      rtems_interrupt_disable( isrlevel );
       (void) set_vector( args->buffer, CLOCK_VECTOR, 1 );
      rtems_interrupt_enable( isrlevel );
    }
 
done:
    return RTEMS_SUCCESSFUL;
}
