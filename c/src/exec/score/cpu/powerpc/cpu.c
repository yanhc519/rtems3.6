/*
 *  PowerPC CPU Dependent Source
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
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
 *  Derived from c/src/exec/cpu/no_cpu/cpu.c:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be found in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/context.h>
#include <rtems/score/thread.h>
#include <rtems/score/interr.h>

/*
 *  These are for testing purposes.
 */

/*  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  INPUT PARAMETERS:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 */

static void ppc_spurious(int, CPU_Interrupt_frame *);

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void      (*thread_dispatch)      /* ignored on this CPU */
)
{
  proc_ptr handler = (proc_ptr)ppc_spurious;
  int i;
#if (PPC_ABI != PPC_ABI_POWEROPEN)
  register unsigned32 r2 = 0;
#if (PPC_ABI != PPC_ABI_GCC27)
  register unsigned32 r13 = 0;

  asm ("mr %0,13" : "=r" ((r13)) : "0" ((r13)));
  _CPU_IRQ_info.Default_r13 = r13;
#endif

  asm ("mr %0,2" : "=r" ((r2)) : "0" ((r2)));
  _CPU_IRQ_info.Default_r2 = r2;
#endif

  _CPU_IRQ_info.Nest_level = &_ISR_Nest_level;
  _CPU_IRQ_info.Disable_level = &_Thread_Dispatch_disable_level;
  _CPU_IRQ_info.Vector_table = _ISR_Vector_table;
#if (PPC_ABI == PPC_ABI_POWEROPEN)
  _CPU_IRQ_info.Dispatch_r2 = ((unsigned32 *)_Thread_Dispatch)[1];
#endif
  _CPU_IRQ_info.Switch_necessary = &_Context_Switch_necessary;
  _CPU_IRQ_info.Signal = &_ISR_Signals_to_thread_executing;

#if (PPC_USE_SPRG)
  i = (int)&_CPU_IRQ_info;
  asm volatile("mtspr 0x113, %0" : "=r" (i) : "0" (i)); /* SPRG 3 */
#endif

  /*
   * Store Msr Value in the IRQ info structure.
   */
   _CPU_MSR_Value(_CPU_IRQ_info.msr_initial);
  
#if (PPC_USE_SPRG)
  i = _CPU_IRQ_info.msr_initial;
  asm volatile("mtspr 0x112, %0" : "=r" (i) : "0" (i)); /* SPRG 2 */
#endif

  if ( cpu_table->spurious_handler )
    handler = (proc_ptr)cpu_table->spurious_handler;

  for (i = 0; i < PPC_INTERRUPT_MAX;  i++)
    _ISR_Vector_table[i] = handler;

  _CPU_Table = *cpu_table;
}

/*PAGE
 *
 *  _CPU_ISR_Calculate_level
 *
 *  The PowerPC puts its interrupt enable status in the MSR register
 *  which also contains things like endianness control.  To be more
 *  awkward, the layout varies from processor to processor.  This
 *  is why it was necessary to adopt a scheme which allowed the user
 *  to specify specifically which interrupt sources were enabled.
 */
 
unsigned32 _CPU_ISR_Calculate_level(
  unsigned32 new_level
)
{
  register unsigned32 new_msr = 0;

  /*
   *  Set the critical interrupt enable bit
   */

#if (PPC_HAS_RFCI)
  if ( !(new_level & PPC_INTERRUPT_LEVEL_CE) )
    new_msr |= PPC_MSR_CE;
#endif

  if ( !(new_level & PPC_INTERRUPT_LEVEL_ME) )
    new_msr |= PPC_MSR_ME;

  if ( !(new_level & PPC_INTERRUPT_LEVEL_EE) )
    new_msr |= PPC_MSR_EE;

  return new_msr;
}

/*PAGE
 *
 *  _CPU_ISR_Set_level
 *
 *  This routine sets the requested level in the MSR.
 */

void _CPU_ISR_Set_level(
  unsigned32 new_level
)
{
  register unsigned32 tmp = 0;
  register unsigned32 new_msr;

  new_msr = _CPU_ISR_Calculate_level( new_level );

  asm volatile (
    "mfmsr %0; andc %0,%0,%1; and %2, %2, %1; or %0, %0, %2; mtmsr %0" :
    "=&r" ((tmp)) :
    "r" ((PPC_MSR_DISABLE_MASK)), "r" ((new_msr)), "0" ((tmp))
  );
}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  This routine gets the current interrupt level from the MSR and 
 *  converts it to an RTEMS interrupt level.
 */

unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level = 0;
  unsigned32 msr;
 
  asm volatile("mfmsr %0" : "=r" ((msr)));
 
  msr &= PPC_MSR_DISABLE_MASK;

  /*
   *  Set the critical interrupt enable bit
   */

#if (PPC_HAS_RFCI)
  if ( !(msr & PPC_MSR_CE) )
    level |= PPC_INTERRUPT_LEVEL_CE;
#endif

  if ( !(msr & PPC_MSR_ME) )
    level |= PPC_INTERRUPT_LEVEL_ME;

  if ( !(msr & PPC_MSR_EE) )
    level |= PPC_INTERRUPT_LEVEL_EE;

  return level;
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 */

#if (PPC_ABI == PPC_ABI_POWEROPEN)
#define CPU_MINIMUM_STACK_FRAME_SIZE 56
#else /* PPC_ABI_SVR4 or PPC_ABI_EABI */
#define CPU_MINIMUM_STACK_FRAME_SIZE 8
#endif

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  unsigned32       *stack_base,
  unsigned32        size,
  unsigned32        new_level,
  void             *entry_point,
  boolean           is_fp
)
{
  unsigned32 msr_value;
  unsigned32 sp;

  sp = (unsigned32)stack_base + size - CPU_MINIMUM_STACK_FRAME_SIZE;
  *((unsigned32 *)sp) = 0;
  the_context->gpr1 = sp;
   
  the_context->msr = _CPU_ISR_Calculate_level( new_level );

  /*
   *  The FP bit of the MSR should only be enabled if this is a floating
   *  point task.  Unfortunately, the vfprintf_r routine in newlib 
   *  ends up pushing a floating point register regardless of whether or
   *  not a floating point number is being printed.  Serious restructuring
   *  of vfprintf.c will be required to avoid this behavior.  At this
   *  time (7 July 1997), this restructuring is not being done.
   */

  /*if ( is_fp ) */
    the_context->msr |= PPC_MSR_FP;

  /*
   *  Calculate the task's MSR value:
   *
   *     + Set the exception prefix bit to point to the exception table
   *     + Force the RI bit
   *     + Use the DR and IR bits 
   */
  _CPU_MSR_Value( msr_value );
  the_context->msr |= (msr_value & PPC_MSR_EP);
  the_context->msr |= PPC_MSR_RI;
  the_context->msr |= msr_value & (PPC_MSR_DR|PPC_MSR_IR);

#if (PPC_ABI == PPC_ABI_POWEROPEN)
  { unsigned32 *desc = (unsigned32 *)entry_point;

    the_context->pc = desc[0];
    the_context->gpr2 = desc[1];
  }
#endif

#if (PPC_ABI == PPC_ABI_SVR4)
  { unsigned    r13 = 0;
    asm volatile ("mr %0, 13" : "=r" ((r13)));
   
    the_context->pc = (unsigned32)entry_point;
    the_context->gpr13 = r13;
  }
#endif

#if (PPC_ABI == PPC_ABI_EABI)
  { unsigned32  r2 = 0;
    unsigned    r13 = 0;
    asm volatile ("mr %0,2; mr %1,13" : "=r" ((r2)), "=r" ((r13)));
 
    the_context->pc = (unsigned32)entry_point;
    the_context->gpr2 = r2;
    the_context->gpr13 = r13;
  }
#endif
}


/*  _CPU_ISR_install_vector
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
   proc_ptr   ignored;
   *old_handler = _ISR_Vector_table[ vector ];

   /*
    *  If the interrupt vector table is a table of pointer to isr entry
    *  points, then we need to install the appropriate RTEMS interrupt
    *  handler for this vector number.
    */

   /*
    * Install the wrapper so this ISR can be invoked properly.
    */
   if (_CPU_Table.exceptions_in_RAM) 
      _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ vector ] = new_handler ? (ISR_Handler_entry)new_handler :
       _CPU_Table.spurious_handler ? 
          (ISR_Handler_entry)_CPU_Table.spurious_handler :
          (ISR_Handler_entry)ppc_spurious;
}

/*PAGE
 *
 *  _CPU_Install_interrupt_stack
 */

void _CPU_Install_interrupt_stack( void )
{
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
  _CPU_IRQ_info.Stack = _CPU_Interrupt_stack_high - 56;
#else
  _CPU_IRQ_info.Stack = _CPU_Interrupt_stack_high - 8;
#endif
}

/* Handle a spurious interrupt */
static void ppc_spurious(int v, CPU_Interrupt_frame *i)
{
#if 0
    printf("Spurious interrupt on vector %d from %08.8x\n",
	   v, i->pc);
#endif
#ifdef ppc403
    if (v == PPC_IRQ_EXTERNAL)
	{
	    register int r = 0;

	    asm volatile("mtdcr 0x42, %0" :
                "=&r" ((r)) : "0" ((r))); /* EXIER */
	}
    else if (v == PPC_IRQ_PIT)
	{
	    register int r = 0x08000000;

	    asm volatile("mtspr 0x3d8, %0" :
                "=&r" ((r)) : "0" ((r))); /* TSR */
	}
    else if (v == PPC_IRQ_FIT)
	{
	    register int r = 0x04000000;

	    asm volatile("mtspr 0x3d8, %0" :
                "=&r" ((r)) : "0" ((r))); /* TSR */
	}
#endif
}

void _CPU_Fatal_error(unsigned32 _error)
{
  asm volatile ("mr 3, %0" : : "r" ((_error)));
  asm volatile ("tweq 5,5");
  asm volatile ("li 0,0; mtmsr 0");
  while (1) ;
}

#define PPC_SYNCHRONOUS_TRAP_BIT_MASK    0x100
#define PPC_ASYNCHRONOUS_TRAP( _trap ) (_trap)
#define PPC_SYNCHRONOUS_TRAP ( _trap ) ((_trap)+PPC_SYNCHRONOUS_TRAP_BIT_MASK)
#define PPC_REAL_TRAP_NUMBER ( _trap ) ((_trap)%PPC_SYNCHRONOUS_TRAP_BIT_MASK)


const CPU_Trap_table_entry _CPU_Trap_slot_template = {

#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
#error " Vector install not tested."
#if (PPC_HAS_FPU)
#error " Vector install not tested."
  0x9421feb0,           /* stwu r1, -(20*4 + 18*8 + IP_END)(r1) */
#else
#error " Vector install not tested."
  0x9421ff40,           /* stwu    r1, -(20*4 + IP_END)(r1)     */
#endif
#else
  0x9421ff90,           /* stwu    r1, -(IP_END)(r1)            */
#endif

  0x90010008,           /* stw   %r0, IP_0(%r1)                 */
  0x38000000,           /* li    %r0, PPC_IRQ                   */
  0x48000002            /* ba    PROC (_ISR_Handler)            */
};

unsigned32  ppc_exception_vector_addr( 
  unsigned32 vector
);


/*PAGE
 *
 *  _CPU_ISR_install_raw_handler
 *
 *  This routine installs the specified handler as a "raw" non-executive
 *  supported trap handler (a.k.a. interrupt service routine).
 *
 *  Input Parameters:
 *    vector      - trap table entry number plus synchronous 
 *                    vs. asynchronous information
 *    new_handler - address of the handler to be installed
 *    old_handler - pointer to an address of the handler previously installed
 *
 *  Output Parameters: NONE
 *    *new_handler - address of the handler previously installed
 * 
 *  NOTE: 
 *
 *  This routine is based on the SPARC routine _CPU_ISR_install_raw_handler.
 *  Install a software trap handler as an executive interrupt handler 
 *  (which is desirable since RTEMS takes care of window and register issues),
 *  then the executive needs to know that the return address is to the trap 
 *  rather than the instruction following the trap.
 *
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  unsigned32             real_vector;
  CPU_Trap_table_entry  *slot;
  unsigned32             u32_handler=0;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

  real_vector = vector;

  /*
   *  Get the current base address of the trap table and calculate a pointer
   *  to the slot we are interested in.
   */
  slot = (CPU_Trap_table_entry  *)ppc_exception_vector_addr( real_vector );

  /*
   *  Get the address of the old_handler from the trap table.
   *
   *  NOTE: The old_handler returned will be bogus if it does not follow
   *        the RTEMS model.
   */

#define HIGH_BITS_MASK   0xFFFFFC00
#define HIGH_BITS_SHIFT  10
#define LOW_BITS_MASK    0x000003FF

  if (slot->stwu_r1 == _CPU_Trap_slot_template.stwu_r1) {
    /* 
     * Set u32_handler = to target address  
     */
    u32_handler = slot->b_Handler & 0x03fffffc;

    /* IMD FIX: sign extend address fragment... */
    if (u32_handler & 0x02000000) {
      u32_handler  |= 0xfc000000;
    }

    *old_handler =  (proc_ptr) u32_handler;
  } else
    *old_handler = 0;

  /*
   *  Copy the template to the slot and then fix it.
   */
  *slot = _CPU_Trap_slot_template;

  u32_handler = (unsigned32) new_handler;

  /* 
   * IMD FIX: insert address fragment only (bits 6..29) 
   *          therefore check for proper address range 
   *          and remove unwanted bits
   */
  if ((u32_handler & 0xfc000000) == 0xfc000000) {
    u32_handler  &= ~0xfc000000;
  }
  else if ((u32_handler & 0xfc000000) != 0x00000000) {
    _Internal_error_Occurred(INTERNAL_ERROR_CORE,
			     TRUE,
			     u32_handler);
  }

  slot->b_Handler |= u32_handler;

  slot->li_r0_IRQ  |= vector;

  _CPU_Data_Cache_Block_Flush( slot );
}

unsigned32  ppc_exception_vector_addr( 
  unsigned32 vector
)
{
#if (!PPC_HAS_EVPR)
  unsigned32 Msr;
#endif
  unsigned32 Top = 0;
  unsigned32 Offset = 0x000;

#if (PPC_HAS_EXCEPTION_PREFIX)
  _CPU_MSR_Value ( Msr );
  if ( ( Msr & PPC_MSR_EP) != 0 ) /* Vectors at FFFx_xxxx */
    Top = 0xfff00000;
#elif (PPC_HAS_EVPR)
  asm volatile( "mfspr %0,0x3d6" : "=r" (Top)); /* EVPR */
  Top = Top & 0xffff0000;
#endif

  switch ( vector ) {
    case PPC_IRQ_SYSTEM_RESET:   /* on 40x aka PPC_IRQ_CRIT */
      Offset = 0x00100;
      break;
    case PPC_IRQ_MCHECK:
      Offset = 0x00200;
      break;
    case PPC_IRQ_PROTECT:
      Offset = 0x00300;
      break;
    case PPC_IRQ_ISI:
      Offset = 0x00400;
      break;
    case PPC_IRQ_EXTERNAL:
      Offset = 0x00500;
      break;
    case PPC_IRQ_ALIGNMENT:
      Offset = 0x00600;
      break;
    case PPC_IRQ_PROGRAM:
      Offset = 0x00700;
      break;
    case PPC_IRQ_NOFP:
      Offset = 0x00800;
      break;
    case PPC_IRQ_DECREMENTER:
      Offset = 0x00900;
      break;
    case PPC_IRQ_RESERVED_A:
      Offset = 0x00a00;
      break;
    case PPC_IRQ_RESERVED_B:
      Offset = 0x00b00;
      break;
    case PPC_IRQ_SCALL:
      Offset = 0x00c00;
      break;
    case PPC_IRQ_TRACE:
      Offset = 0x00d00;
      break;
    case PPC_IRQ_FP_ASST:
      Offset = 0x00e00;
      break;

#if defined(ppc403)
                                  
/*  PPC_IRQ_CRIT is the same vector as PPC_IRQ_RESET
    case PPC_IRQ_CRIT:
      Offset = 0x00100;
      break;
*/
    case PPC_IRQ_PIT:
      Offset = 0x01000;
      break;
    case PPC_IRQ_FIT:
      Offset = 0x01010;
      break;
    case PPC_IRQ_WATCHDOG:
      Offset = 0x01020;
      break;
    case PPC_IRQ_DEBUG:
      Offset = 0x02000;
      break;

#elif defined(ppc601)
    case PPC_IRQ_TRACE:
      Offset = 0x02000;
      break;

#elif defined(ppc603)
    case PPC_IRQ_TRANS_MISS:
      Offset = 0x1000;
      break;
    case PPC_IRQ_DATA_LOAD:
      Offset = 0x1100;
      break;
    case PPC_IRQ_DATA_STORE:
      Offset = 0x1200;
      break;
    case PPC_IRQ_ADDR_BRK:
      Offset = 0x1300;
      break;
    case PPC_IRQ_SYS_MGT:
      Offset = 0x1400;
      break;

#elif defined(ppc603e)
    case PPC_TLB_INST_MISS:
      Offset = 0x1000;
      break;
    case PPC_TLB_LOAD_MISS:
      Offset = 0x1100;
      break;
    case PPC_TLB_STORE_MISS:
      Offset = 0x1200;
      break;
    case PPC_IRQ_ADDRBRK:
      Offset = 0x1300;
      break;
    case PPC_IRQ_SYS_MGT:
      Offset = 0x1400;
      break;

#elif defined(ppc604)
    case PPC_IRQ_ADDR_BRK:
      Offset = 0x1300;
      break;
    case PPC_IRQ_SYS_MGT:
      Offset = 0x1400;
      break;
#endif

  }
  Top += Offset;
  return Top;
}

