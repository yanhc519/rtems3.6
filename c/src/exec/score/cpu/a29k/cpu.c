/*
 *  AMD 29K CPU Dependent Source
 *
 *  Author:     Craig Lebakken <craigl@transition.com>
 *
 *  COPYRIGHT (c) 1996 by Transition Networks Inc.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Transition Networks not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      Transition Networks makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/score/cpu/no_cpu/cpu.c:
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
#ifndef lint
static char _sccsid[] = "@(#)cpu.c 21 Aug 1996     1.6\n";
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/thread.h>
#include <stdio.h>
#include <stdlib.h>

void a29k_ISR_Handler(unsigned32 vector);

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */


void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)()      /* ignored on this CPU */
)
{
  unsigned int i;
  /*
   *  The thread_dispatch argument is the address of the entry point
   *  for the routine called at the end of an ISR once it has been
   *  decided a context switch is necessary.  On some compilation
   *  systems it is difficult to call a high-level language routine
   *  from assembly.  This allows us to trick these systems.
   *
   *  If you encounter this problem save the entry point in a CPU
   *  dependent variable.
   */

  _CPU_Thread_dispatch_pointer = thread_dispatch;

  /*
   *  If there is not an easy way to initialize the FP context
   *  during Context_Initialize, then it is usually easier to
   *  save an "uninitialized" FP context here and copy it to
   *  the task's during Context_Initialize.
   */

  /* FP context initialization support goes here */

  _CPU_Table = *cpu_table;

  for ( i = 0; i < ISR_NUMBER_OF_VECTORS; i++ )
  {
     _ISR_Vector_table[i] = (proc_ptr)NULL;
  }
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  /*
   *  This routine returns the current interrupt level.
   */
   return 0;
}

/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 */
 
extern void intr14( void );
extern void intr3( void );
extern void intr2( void );

void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  /*
   *  This is where we install the interrupt handler into the "raw" interrupt
   *  table used by the CPU to dispatch interrupt handlers.
   */
   switch( vector )
   {
      case 14:
         _settrap( vector, intr14 );
         break;
      case 3:
         _settrap( vector, intr3 );
         break;
      case 2:
         _settrap( vector, intr2 );
         break;
      default:
         break;
   }
}


/*PAGE
 *
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector      - interrupt vector number
 *    old_handler - former ISR for this vector number
 *    new_handler - replacement ISR for this vector number
 *
 *  Output parameters:  NONE
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  If the interrupt vector table is a table of pointer to isr entry
    *  points, then we need to install the appropriate RTEMS interrupt
    *  handler for this vector number.
    */

   _CPU_ISR_install_raw_handler( vector, new_handler, old_handler );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
}

/*PAGE
 *
 *  _CPU_Internal_threads_Idle_thread_body
 *
 *  NOTES:
 *
 *  1. This is the same as the regular CPU independent algorithm.
 *
 *  2. If you implement this using a "halt", "idle", or "shutdown"
 *     instruction, then don't forget to put it in an infinite loop.
 *
 *  3. Be warned. Some processors with onboard DMA have been known
 *     to stop the DMA if the CPU were put in IDLE mode.  This might
 *     also be a problem with other on-chip peripherals.  So use this
 *     hook with caution.
 */

void _CPU_Internal_threads_Idle_thread_body( void )
{

  for( ; ; )
  {
  }
    /* insert your "halt" instruction here */ ;
}

void a29k_fatal_error( unsigned32 error )
{
   printf("\n\nfatal error %d, rebooting!!!\n",error );
   exit(error);
}

   /*
    *  This discussion ignores a lot of the ugly details in a real
    *  implementation such as saving enough registers/state to be
    *  able to do something real.  Keep in mind that the goal is
    *  to invoke a user's ISR handler which is written in C and
    *  uses a certain set of registers.
    *
    *  Also note that the exact order is to a large extent flexible.
    *  Hardware will dictate a sequence for a certain subset of
    *  _ISR_Handler while requirements for setting
    */

  /*
   *  At entry to "common" _ISR_Handler, the vector number must be
   *  available.  On some CPUs the hardware puts either the vector
   *  number or the offset into the vector table for this ISR in a
   *  known place.  If the hardware does not give us this information,
   *  then the assembly portion of RTEMS for this port will contain
   *  a set of distinct interrupt entry points which somehow place
   *  the vector number in a known place (which is safe if another
   *  interrupt nests this one) and branches to _ISR_Handler.
   *
   */

void a29k_ISR_Handler(unsigned32 vector)
{
   _ISR_Nest_level++;
   _Thread_Dispatch_disable_level++;
   if ( _ISR_Vector_table[ vector ] )
      (*_ISR_Vector_table[ vector ])( vector );
   --_Thread_Dispatch_disable_level;
   --_ISR_Nest_level;
   if ( !_Thread_Dispatch_disable_level && !_ISR_Nest_level &&
    (_Context_Switch_necessary || _ISR_Signals_to_thread_executing ))
      _Thread_Dispatch();
   return;
}
