/*
 *  This file contains the clock driver the Hitachi SH 703X
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h>

#include <stdlib.h>

#include <rtems/libio.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/sh.h>
#include <rtems/score/cpu_isps.h>
#include <rtems/score/iosh7030.h>

#define _ITU_COUNTER0_MICROSECOND (MHZ/4)

#ifndef CLOCKPRIO
#define CLOCKPRIO 10
#endif

#define ITU0_STARTMASK 	0xfe
#define ITU0_SYNCMASK 	0xfe
#define ITU0_MODEMASK 	0xfe
#define ITU0_TCRMASK 	0x22
#define ITU_STAT_MASK 	0xf8
#define ITU0_IRQMASK 	0xfe
#define ITU0_TIERMASK 	0x01
#define IPRC_ITU0_MASK 	0xff0f
#define ITU0_TIORVAL 	0x08

/*
 *  The interrupt vector number associated with the clock tick device
 *  driver.
 */

#define CLOCK_VECTOR IMIA0_ISP_V

/*
 *  Clock_driver_ticks is a monotonically increasing counter of the
 *  number of clock ticks since the driver was initialized.
 */

volatile rtems_unsigned32 Clock_driver_ticks;

static void Clock_exit( void );
static rtems_isr Clock_isr( rtems_vector_number vector );

/*
 *  Clock_isrs is the number of clock ISRs until the next invocation of
 *  the RTEMS clock tick routine.  The clock tick device driver
 *  gets an interrupt once a millisecond and counts down until the
 *  length of time between the user configured microseconds per tick
 *  has passed.
 */

rtems_unsigned32 Clock_isrs;              /* ISRs until next tick */
static rtems_unsigned32 Clock_isrs_const;        /* only calculated once */

/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;

/*
 *  The previous ISR on this clock tick interrupt vector.
 */

rtems_isr_entry  Old_ticker;

/*
 *  Isr Handler
 */

rtems_isr Clock_isr(
  rtems_vector_number vector
)
{
  /*
   * bump the number of clock driver ticks since initialization
   *

   * determine if it is time to announce the passing of tick as configured
   * to RTEMS through the rtems_clock_tick directive
   *
   * perform any timer dependent tasks
   */
  unsigned8 temp;

  /* reset the flags of the status register */
  temp = read8( ITU_TSR0) & ITU_STAT_MASK;
  write8( temp, ITU_TSR0);

  Clock_driver_ticks++ ;

  if( Clock_isrs == 1)
    {
      rtems_clock_tick();
      Clock_isrs = Clock_isrs_const;
    }
  else
    {
      Clock_isrs-- ;
    }
}

/*
 *  Install_clock
 *
 *  Install a clock tick handler and reprograms the chip.  This
 *  is used to initially establish the clock tick.
 */

void Install_clock(
  rtems_isr_entry clock_isr
)
{
  unsigned8 temp8 = 0;
  
  /*
   *  Initialize the clock tick device driver variables
   */

  Clock_driver_ticks = 0;
  Clock_isrs_const = BSP_Configuration.microseconds_per_tick / 10000;
  Clock_isrs = Clock_isrs_const;

  /*
   *  If ticks_per_timeslice is configured as non-zero, then the user
   *  wants a clock tick.
   */

  if ( BSP_Configuration.ticks_per_timeslice ) {
    rtems_interrupt_catch( Clock_isr, CLOCK_VECTOR, &Old_ticker );
    /*
     *  Hardware specific initialize goes here
     */
    
    /* stop Timer 0 */
    temp8 = read8( ITU_TSTR) & ITU0_STARTMASK;
    write8( temp8, ITU_TSTR);

    /* set initial counter value to 0 */
    write16( 0, ITU_TCNT0);

    /* Timer 0 runs independent */
    temp8 = read8( ITU_TSNC) & ITU0_SYNCMASK;
    write8( temp8, ITU_TSNC);

    /* Timer 0 normal mode */
    temp8 = read8( ITU_TMDR) & ITU0_MODEMASK;
    write8( temp8, ITU_TMDR);

    /* TCNT is cleared by GRA ; internal clock /4 */
    write8( ITU0_TCRMASK , ITU_TCR0);

    /* use GRA without I/O - pins  */
    write8( ITU0_TIORVAL, ITU_TIOR0); 
    
    /* reset flags of the status register */
    temp8 = read8( ITU_TSR0) & ITU_STAT_MASK;
    write8( temp8, ITU_TSR0);

    /* Irq if is equal GRA */
    temp8 = read8( ITU_TIER0) | ITU0_TIERMASK;
    write8( temp8, ITU_TIER0);

    /* set interrupt priority */
    if( sh_set_irq_priority( CLOCK_VECTOR, CLOCKPRIO ) != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred( RTEMS_NOT_CONFIGURED);

    /* set counter limits */
    write16( _ITU_COUNTER0_MICROSECOND * BSP_Configuration.microseconds_per_tick,
	     ITU_GRA0);
   
    /* start counter */
    temp8 = read8( ITU_TSTR) |~ITU0_STARTMASK;
    write8( temp8, ITU_TSTR);
    
  }

  /*
   *  Schedule the clock cleanup routine to execute if the application exits.
   */

  atexit( Clock_exit );
}

/*
 *  Clean up before the application exits
 */

void Clock_exit( void )
{
  unsigned8 temp8 = 0;
  if ( BSP_Configuration.ticks_per_timeslice ) {

    /* turn off the timer interrupts */
    /* set interrupt priority to 0 */
    if( sh_set_irq_priority( CLOCK_VECTOR, 0 ) != RTEMS_SUCCESSFUL)
      rtems_fatal_error_occurred( RTEMS_UNSATISFIED);

/*
 *   temp16 = read16( ITU_TIER0) & IPRC_ITU0_IRQMASK;
 *   write16( temp16, ITU_TIER0);
 */

    /* stop counter */
    temp8 = read8( ITU_TSTR) & ITU0_STARTMASK;
    write8( temp8, ITU_TSTR);

    /* old vector shall not be installed */
  }
}

/*
 *  Clock_initialize
 *
 *  Device driver entry point for clock tick driver initialization.
 */

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
  
  if (args != 0)
    {
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
	  rtems_isr_entry	ignored ;
	  rtems_interrupt_disable( isrlevel );
	  rtems_interrupt_catch( args->buffer, CLOCK_VECTOR, &ignored );
	  
	  rtems_interrupt_enable( isrlevel );
	}
    }
  return RTEMS_SUCCESSFUL;
}
