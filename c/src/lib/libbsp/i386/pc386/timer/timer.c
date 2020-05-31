/*-------------------------------------------------------------------------+
| timer.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| This file contains the PC386 timer package.
+--------------------------------------------------------------------------+
| NOTE: It is important that the timer start/stop overhead be determined
|       when porting or modifying this code.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is base on:
|   timer.c,v 1.7 1995/12/19 20:07:43 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/


#include <stdlib.h>

#include <bsp.h>
#include <irq.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define AVG_OVERHEAD  0              /* 0.1 microseconds to start/stop timer. */
#define LEAST_VALID   1              /* Don't trust a value lower than this.  */
#define SLOW_DOWN_IO  0x80     	/* io which does nothing */

#define TWO_MS	(rtems_unsigned32)(2000)     /* TWO_MS = 2000us (sic!) */

#define MSK_NULL_COUNT 0x40	/* bit counter available for reading */

#define CMD_READ_BACK_STATUS 0xE2   /* command read back status */
/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
volatile rtems_unsigned32 Ttimer_val;
         rtems_boolean    Timer_driver_Find_average_overhead = TRUE;
         unsigned int     loop1ms;

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void timerisr(void);
       /* timer (int 08h) Interrupt Service Routine (defined in 'timerisr.s') */

/*-------------------------------------------------------------------------+
| Pentium optimized timer handling.
+--------------------------------------------------------------------------*/
#if defined(pentium)

/*-------------------------------------------------------------------------+
|         Function: rdtsc
|      Description: Read the value of PENTIUM on-chip cycle counter.
| Global Variables: None.
|        Arguments: None.
|          Returns: Value of PENTIUM on-chip cycle counter. 
+--------------------------------------------------------------------------*/
static inline unsigned long long
rdtsc(void)
{
  /* Return the value of the on-chip cycle counter. */
  unsigned long long result;
  asm volatile(".byte 0x0F, 0x31" : "=A" (result));
  return result;
} /* rdtsc */


/*-------------------------------------------------------------------------+
|         Function: Timer_exit
|      Description: Timer cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_exit(void)
{
} /* Timer_exit */


/*-------------------------------------------------------------------------+
|         Function: Timer_initialize
|      Description: Timer initialization routine.
| Global Variables: Ttimer_val.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_initialize(void)
{
  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
  }
  Ttimer_val = rdtsc(); /* read starting time */
} /* Timer_initialize */


/*-------------------------------------------------------------------------+
|         Function: Read_timer
|      Description: Read hardware timer value.
| Global Variables: Ttimer_val, Timer_driver_Find_average_overhead.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_unsigned32
Read_timer(void)
{
  register rtems_unsigned32 total;

  total =  (rtems_unsigned32)(rdtsc() - Ttimer_val);

  if (Timer_driver_Find_average_overhead)
    return total;
  else if (total < LEAST_VALID)
    return 0; /* below timer resolution */
  else
    return (total - AVG_OVERHEAD);
} /* Read_timer */

#else /* pentium */

/*-------------------------------------------------------------------------+
| Non-Pentium timer handling.
+--------------------------------------------------------------------------*/
#define US_PER_ISR   250  /* Number of micro-seconds per timer interruption */


/*-------------------------------------------------------------------------+
|         Function: Timer_exit
|      Description: Timer cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
static void
timerOff(const rtems_raw_irq_connect_data* used)
{
    /*
     * disable interrrupt at i8259 level
     */
     BSP_irq_disable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
     /* reset timer mode to standard (DOS) value */
     outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
     outport_byte(TIMER_CNTR0, 0);
     outport_byte(TIMER_CNTR0, 0);
} /* Timer_exit */


static void 
timerOn(const rtems_raw_irq_connect_data* used)
{
     /* load timer for US_PER_ISR microsecond period */
     outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
     outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 0 & 0xff);
     outport_byte(TIMER_CNTR0, US_TO_TICK(US_PER_ISR) >> 8 & 0xff);
    /*
     * enable interrrupt at i8259 level
     */
     BSP_irq_enable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

static int 
timerIsOn(const rtems_raw_irq_connect_data *used)
{
     return BSP_irq_enabled_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);}

static rtems_raw_irq_connect_data timer_raw_irq_data = {
  BSP_PERIODIC_TIMER + BSP_IRQ_VECTOR_BASE,
  timerisr,
  timerOn,
  timerOff,
  timerIsOn
};

/*-------------------------------------------------------------------------+
|         Function: Timer_exit
|      Description: Timer cleanup routine at RTEMS exit. NOTE: This routine is
|                   not really necessary, since there will be a reset at exit.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_exit(void)
{
  i386_delete_idt_entry (&timer_raw_irq_data);
} /* Timer_exit */

/*-------------------------------------------------------------------------+
|         Function: Timer_initialize
|      Description: Timer initialization routine.
| Global Variables: Ttimer_val.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Timer_initialize(void)
{
  static rtems_boolean First = TRUE;

  if (First)
  {
    First = FALSE;

    atexit(Timer_exit); /* Try not to hose the system at exit. */
    if (!i386_set_idt_entry (&timer_raw_irq_data)) {
      printk("raw handler connexion failed\n");
      rtems_fatal_error_occurred(1);
    }
  }
  /* wait for ISR to be called at least once */
  Ttimer_val = 0;
  while (Ttimer_val == 0)
    continue;
  Ttimer_val = 0;
} /* Timer_initialize */


/*-------------------------------------------------------------------------+
|         Function: Read_timer
|      Description: Read hardware timer value.
| Global Variables: Ttimer_val, Timer_driver_Find_average_overhead.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_unsigned32
Read_timer(void)
{
  register rtems_unsigned32 total, clicks;
  register rtems_unsigned8  lsb, msb;

  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
  clicks = (msb << 8) | lsb;
  total  = (Ttimer_val * US_PER_ISR) + (US_PER_ISR - TICK_TO_US(clicks));

  if (Timer_driver_Find_average_overhead)
    return total;
  else if (total < LEAST_VALID)
    return 0; /* below timer resolution */
  else
    return (total - AVG_OVERHEAD);
}

#endif /* pentium */


/*-------------------------------------------------------------------------+
|         Function: Empty_function
|      Description: Empty function used in time tests.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
rtems_status_code Empty_function(void)
{
  return RTEMS_SUCCESSFUL;
} /* Empty function */
 

/*-------------------------------------------------------------------------+
|         Function: Set_find_average_overhead
|      Description: Set internal Timer_driver_Find_average_overhead flag value.
| Global Variables: Timer_driver_Find_average_overhead.
|        Arguments: find_flag - new value of the flag.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Set_find_average_overhead(rtems_boolean find_flag)
{
  Timer_driver_Find_average_overhead = find_flag;
} /* Set_find_average_overhead */

/*-------------------------------------------------------------------------+
|         Function: Calibrate_loop_1ms
|      Description: Set loop variable to calibrate a 1ms loop
| Global Variables: loop1ms
|        Arguments: none
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Calibrate_loop_1ms(void){
  unsigned int i;
  unsigned short loadedValue, offset;
  unsigned int timerValue;
  rtems_interrupt_level  level;
  unsigned short lsb, msb;
  unsigned char status;
  
  
  loop1ms = 100 ;
  timerValue = 2000;
  loadedValue = US_TO_TICK(2000);
  
  rtems_interrupt_disable(level);

  /*
   * Compute the offset to apply due to read counter register
   */
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
  outport_byte(TIMER_CNTR0, loadedValue >> 0 & 0xff);
  outport_byte(TIMER_CNTR0, loadedValue >> 8 & 0xff);

  outport_byte(TIMER_MODE, CMD_READ_BACK_STATUS); /* read Status counter 0 */
  inport_byte(TIMER_CNTR0, status);
  while (status & MSK_NULL_COUNT){ 	/* wait for counter ready */ 	
    outport_byte(TIMER_MODE, CMD_READ_BACK_STATUS);
    inport_byte(TIMER_CNTR0, status);
  }
  
  outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
  inport_byte(TIMER_CNTR0, lsb);
  inport_byte(TIMER_CNTR0, msb);
  offset = loadedValue - (unsigned short)((msb << 8) | lsb);

  while (timerValue > 1000){
    loop1ms++;

    /* load timer for 2ms+offset period */
    outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_16BIT|TIMER_RATEGEN);
    outport_byte(TIMER_CNTR0, (loadedValue+offset) >> 0 & 0xff);
    outport_byte(TIMER_CNTR0, (loadedValue+offset) >> 8 & 0xff);

    outport_byte(TIMER_MODE, CMD_READ_BACK_STATUS); /* read Status counter 0 */
    inport_byte(TIMER_CNTR0, status);
    while (status & MSK_NULL_COUNT) {	/* wait for counter ready */ 
      outport_byte(TIMER_MODE, CMD_READ_BACK_STATUS);
      inport_byte(TIMER_CNTR0, status);
    }
    
    for (i=0; i<loop1ms; i++)
      outport_byte(SLOW_DOWN_IO, 0);	/* write is # 1us */

    outport_byte(TIMER_MODE, TIMER_SEL0|TIMER_LATCH);
    inport_byte(TIMER_CNTR0, lsb);
    inport_byte(TIMER_CNTR0, msb);
    timerValue = TICK_TO_US((msb << 8) | lsb);
  }

  rtems_interrupt_enable(level);
}

/*-------------------------------------------------------------------------+
|         Function: Wait_X_1ms
|      Description: loop which waits at least timeToWait ms
| Global Variables: loop1ms
|        Arguments: timeToWait
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void
Wait_X_ms( unsigned int timeToWait){

  unsigned int i, j;

  for (j=0; j<timeToWait ; j++)
    for (i=0; i<loop1ms; i++)
      outport_byte(SLOW_DOWN_IO, 0);	/* write is # 1us */
}






