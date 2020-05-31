/*  clock.c
 *
 *  This routine initializes the interval timer on the
 *  PowerPC 403 CPU.  The tick frequency is specified by the bsp.
 *
 *  Author: Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libcpu/hppa1.1/clock/clock.c:
 *
 *  Modifications for deriving timer clock from cpu system clock by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
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

#include <bsp.h>
#include <clockdrv.h>
#include <rtems/libio.h>

#include <stdlib.h>                     /* for atexit() */

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

volatile rtems_unsigned32 Clock_driver_ticks;
static rtems_unsigned32 pit_value, tick_time;
static rtems_boolean auto_restart;

void Clock_exit( void );
 
/*
 * These are set by clock driver during its init
 */
 
rtems_device_major_number rtems_clock_major = ~0;
rtems_device_minor_number rtems_clock_minor;
 
static inline rtems_unsigned32 get_itimer(void)
{
    register rtems_unsigned32 rc;

    asm volatile ("mfspr %0, 0x3dd" : "=r" ((rc))); /* TBLO */

    return rc;
}

/*
 *  ISR Handler
 */
 
rtems_isr
Clock_isr(rtems_vector_number vector)
{
    if (!auto_restart)
    {
      rtems_unsigned32 clicks_til_next_interrupt;
      rtems_unsigned32 itimer_value;
 
      /*
       * setup for next interrupt; making sure the new value is reasonably
       * in the future.... in case we lost out on an interrupt somehow
       */
 
      itimer_value = get_itimer();
      tick_time += pit_value;
 
      /*
       * how far away is next interrupt *really*
       * It may be a long time; this subtraction works even if
       * Clock_clicks_interrupt < Clock_clicks_low_order via
       * the miracle of unsigned math.
       */
      clicks_til_next_interrupt = tick_time - itimer_value;
 
      /*
       * If it is too soon then bump it up.
       * This should only happen if CPU_HPPA_CLICKS_PER_TICK is too small.
       * But setting it low is useful for debug, so...
       */
 
      if (clicks_til_next_interrupt < 400)
      {
        tick_time = itimer_value + 1000;
        clicks_til_next_interrupt = 1000;
        /* XXX: count these! this should be rare */
      }
 
      /*
       * If it is too late, that means we missed the interrupt somehow.
       * Rather than wait 35-50s for a wrap, we just fudge it here.
       */
 
      if (clicks_til_next_interrupt > pit_value)
      {
        tick_time = itimer_value + 1000;
        clicks_til_next_interrupt = 1000;
        /* XXX: count these! this should never happen :-) */
      }
 
      asm volatile ("mtspr 0x3db, %0" :: "r" 
                         (clicks_til_next_interrupt)); /* PIT */
  }
 
    asm volatile ( "mtspr 0x3d8, %0" :: "r" (0x08000000)); /* TSR */
 
    Clock_driver_ticks++;
 
    rtems_clock_tick();
}

void Install_clock(rtems_isr_entry clock_isr)
{
    rtems_isr_entry previous_isr;
    rtems_unsigned32 pvr, iocr;
 
    Clock_driver_ticks = 0;
 
    asm volatile ("mfdcr %0, 0xa0" : "=r" (iocr)); /* IOCR */

    if (Cpu_table.timer_internal_clock) {
	iocr &= ~4; /* timer clocked from system clock */
    }
    else {
	iocr |= 4; /* select external timer clock */
    }

    asm volatile ("mtdcr 0xa0, %0" : "=r" (iocr) : "0" (iocr)); /* IOCR */
 
    asm volatile ("mfspr %0, 0x11f" : "=r" ((pvr))); /* PVR */
 
    if (((pvr & 0xffff0000) >> 16) != 0x0020)
      return; /* Not a ppc403 */
 
    if ((pvr & 0xff00) == 0x0000) /* 403GA */
#if 0 /* FIXME: in which processor versions will "autoload" work properly? */
      auto_restart = (pvr & 0x00f0) > 0x0000 ? 1 : 0;
#else 
    /* no known chip version supports auto restart of timer... */
    auto_restart = 0;
#endif
    else if ((pvr & 0xff00) == 0x0100) /* 403GB */
      auto_restart = 1;
 
    pit_value = BSP_Configuration.microseconds_per_tick *
      Cpu_table.clicks_per_usec;
 
    if (BSP_Configuration.ticks_per_timeslice)
    {
      register rtems_unsigned32 tcr;

        /*
         * initialize the interval here
         * First tick is set to right amount of time in the future
         * Future ticks will be incremented over last value set
         * in order to provide consistent clicks in the face of
         * interrupt overhead
         */
 
      rtems_interrupt_catch(clock_isr, PPC_IRQ_PIT, &previous_isr);
 
      asm volatile ("mtspr 0x3db, %0" : : "r" (pit_value)); /* PIT */
 
      asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
 
      tcr &= ~ 0x04400000;
 
      tcr |= (auto_restart ? 0x04400000 : 0x04000000);
 
      tick_time = get_itimer() + pit_value;
 
      asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
    }
    atexit(Clock_exit);
}

void
ReInstall_clock(rtems_isr_entry new_clock_isr)
{
    rtems_isr_entry previous_isr;
    rtems_unsigned32 isrlevel = 0;

    rtems_interrupt_disable(isrlevel);
    
    rtems_interrupt_catch(new_clock_isr, PPC_IRQ_PIT, &previous_isr);

    rtems_interrupt_enable(isrlevel);
}


/*
 * Called via atexit()
 * Remove the clock interrupt handler by setting handler to NULL
 */

void
Clock_exit(void)
{
    if ( BSP_Configuration.ticks_per_timeslice )
    {
      register rtems_unsigned32 tcr;
 
      asm volatile ("mfspr %0, 0x3da" : "=r" ((tcr))); /* TCR */
 
      tcr &= ~ 0x04400000;
 
      asm volatile ("mtspr 0x3da, %0" : "=r" ((tcr)) : "0" ((tcr))); /* TCR */
 
      (void) set_vector(0, PPC_IRQ_PIT, 1);
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
    rtems_libio_ioctl_args_t *args = pargp;
 
    if (args == 0)
        goto done;
 
    /*
     * This is hokey, but until we get a defined interface
     * to do this, it will just be this simple...
     */
 
    if (args->command == rtems_build_name('I', 'S', 'R', ' '))
    {
        Clock_isr(PPC_IRQ_PIT);
    }
    else if (args->command == rtems_build_name('N', 'E', 'W', ' '))
    {
        ReInstall_clock(args->buffer);
    }
 
done:
    return RTEMS_SUCCESSFUL;
}

