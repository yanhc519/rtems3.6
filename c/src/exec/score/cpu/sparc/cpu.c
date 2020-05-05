/*
 *  SPARC Dependent Source
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space 
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995. 
 *  European Space Agency.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/isr.h>

#if defined(erc32)
#include <erc32.h>
#endif

/*
 *  This initializes the set of opcodes placed in each trap 
 *  table entry.  The routine which installs a handler is responsible
 *  for filling in the fields for the _handler address and the _vector
 *  trap type.
 *
 *  The constants following this structure are masks for the fields which
 *  must be filled in when the handler is installed.
 */

const CPU_Trap_table_entry _CPU_Trap_slot_template = {
  0xa1480000,      /* mov   %psr, %l0           */
  0x29000000,      /* sethi %hi(_handler), %l4  */
  0x81c52000,      /* jmp   %l4 + %lo(_handler) */
  0xa6102000       /* mov   _vector, %l3        */
};

/*PAGE
 *
 *  _CPU_Initialize
 *
 *  This routine performs processor dependent initialization.
 *
 *  Input Parameters:
 *    cpu_table       - CPU table to initialize
 *    thread_dispatch - address of disptaching routine
 *
 *  Output Parameters: NONE
 * 
 *  NOTE: There is no need to save the pointer to the thread dispatch routine.
 *        The SPARC's assembly code can reference it directly with no problems.
 */

void _CPU_Initialize(
  rtems_cpu_table  *cpu_table,
  void            (*thread_dispatch)      /* ignored on this CPU */
)
{
  void                  *pointer;
  unsigned32             trap_table_start;
  unsigned32             tbr_value;
  CPU_Trap_table_entry  *old_tbr;
  CPU_Trap_table_entry  *trap_table;

  /*
   *  Install the executive's trap table.  All entries from the original
   *  trap table are copied into the executive's trap table.  This is essential
   *  since this preserves critical trap handlers such as the window underflow
   *  and overflow handlers.  It is the responsibility of the BSP to provide
   *  install these in the initial trap table.
   */
 
  trap_table_start = (unsigned32) &_CPU_Trap_Table_area;
  if (trap_table_start & (SPARC_TRAP_TABLE_ALIGNMENT-1))
    trap_table_start = (trap_table_start + SPARC_TRAP_TABLE_ALIGNMENT) & 
                       ~(SPARC_TRAP_TABLE_ALIGNMENT-1);

  trap_table = (CPU_Trap_table_entry *) trap_table_start;

  sparc_get_tbr( tbr_value );

  old_tbr = (CPU_Trap_table_entry *) (tbr_value & 0xfffff000);

  memcpy( trap_table, (void *) old_tbr, 256 * sizeof( CPU_Trap_table_entry ) );

  sparc_set_tbr( trap_table_start );

  /*
   *  This seems to be the most appropriate way to obtain an initial
   *  FP context on the SPARC.  The NULL fp context is copied it to
   *  the task's FP context during Context_Initialize.
   */

  pointer = &_CPU_Null_fp_context;
  _CPU_Context_save_fp( &pointer );

  /*
   *  Grab our own copy of the user's CPU table.
   */

  _CPU_Table = *cpu_table;

#if defined(erc32)

  /*
   *  ERC32 specific initialization
   */

  _ERC32_MEC_Timer_Control_Mirror = 0;
  ERC32_MEC.Timer_Control = 0;

  ERC32_MEC.Control |= ERC32_CONFIGURATION_POWER_DOWN_ALLOWED;

#endif

}

/*PAGE
 *
 *  _CPU_ISR_Get_level
 *
 *  Input Parameters: NONE
 *
 *  Output Parameters:
 *    returns the current interrupt level (PIL field of the PSR)
 */
 
unsigned32 _CPU_ISR_Get_level( void )
{
  unsigned32 level;
 
  sparc_get_interrupt_level( level );
 
  return level;
}

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
 *  On the SPARC, there are really only 256 vectors.  However, the executive
 *  has no easy, fast, reliable way to determine which traps are synchronous
 *  and which are asynchronous.  By default, synchronous traps return to the
 *  instruction which caused the interrupt.  So if you install a software
 *  trap handler as an executive interrupt handler (which is desirable since
 *  RTEMS takes care of window and register issues), then the executive needs
 *  to know that the return address is to the trap rather than the instruction
 *  following the trap.
 *
 *  So vectors 0 through 255 are treated as regular asynchronous traps which
 *  provide the "correct" return address.  Vectors 256 through 512 are assumed
 *  by the executive to be synchronous and to require that the return address
 *  be fudged.
 *
 *  If you use this mechanism to install a trap handler which must reexecute
 *  the instruction which caused the trap, then it should be installed as
 *  an asynchronous trap.  This will avoid the executive changing the return
 *  address.
 */
 
void _CPU_ISR_install_raw_handler(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
  unsigned32             real_vector;
  CPU_Trap_table_entry  *tbr;
  CPU_Trap_table_entry  *slot;
  unsigned32             u32_tbr;
  unsigned32             u32_handler;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

  real_vector = SPARC_REAL_TRAP_NUMBER( vector );

  /*
   *  Get the current base address of the trap table and calculate a pointer
   *  to the slot we are interested in.
   */

  sparc_get_tbr( u32_tbr );

  u32_tbr &= 0xfffff000;

  tbr = (CPU_Trap_table_entry *) u32_tbr;

  slot = &tbr[ real_vector ];

  /*
   *  Get the address of the old_handler from the trap table.
   *
   *  NOTE: The old_handler returned will be bogus if it does not follow
   *        the RTEMS model.
   */

#define HIGH_BITS_MASK   0xFFFFFC00
#define HIGH_BITS_SHIFT  10
#define LOW_BITS_MASK    0x000003FF

  if ( slot->mov_psr_l0 == _CPU_Trap_slot_template.mov_psr_l0 ) {
    u32_handler = 
      ((slot->sethi_of_handler_to_l4 & HIGH_BITS_MASK) << HIGH_BITS_SHIFT) |
      (slot->jmp_to_low_of_handler_plus_l4 & LOW_BITS_MASK);
    *old_handler = (proc_ptr) u32_handler;
  } else
    *old_handler = 0;

  /*
   *  Copy the template to the slot and then fix it.
   */

  *slot = _CPU_Trap_slot_template;

  u32_handler = (unsigned32) new_handler;

  slot->mov_vector_l3 |= vector;
  slot->sethi_of_handler_to_l4 |= 
    (u32_handler & HIGH_BITS_MASK) >> HIGH_BITS_SHIFT;
  slot->jmp_to_low_of_handler_plus_l4 |= (u32_handler & LOW_BITS_MASK);
}

/*PAGE
 *
 *  _CPU_ISR_install_vector
 *
 *  This kernel routine installs the RTEMS handler for the
 *  specified vector.
 *
 *  Input parameters:
 *    vector       - interrupt vector number
 *    new_handler  - replacement ISR for this vector number
 *    old_handler  - pointer to former ISR for this vector number
 *
 *  Output parameters: 
 *    *old_handler - former ISR for this vector number
 *
 */

void _CPU_ISR_install_vector(
  unsigned32  vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
   unsigned32 real_vector;
   proc_ptr   ignored;

  /*
   *  Get the "real" trap number for this vector ignoring the synchronous
   *  versus asynchronous indicator included with our vector numbers.
   */

   real_vector = SPARC_REAL_TRAP_NUMBER( vector );

   /*
    *  Return the previous ISR handler.
    */

   *old_handler = _ISR_Vector_table[ real_vector ];

   /*
    *  Install the wrapper so this ISR can be invoked properly.
    */

   _CPU_ISR_install_raw_handler( vector, _ISR_Handler, &ignored );

   /*
    *  We put the actual user ISR address in '_ISR_vector_table'.  This will
    *  be used by the _ISR_Handler so the user gets control.
    */

    _ISR_Vector_table[ real_vector ] = new_handler;
}

/*PAGE
 *
 *  _CPU_Context_Initialize
 *
 *  This kernel routine initializes the basic non-FP context area associated
 *  with each thread.
 *
 *  Input parameters:
 *    the_context  - pointer to the context area
 *    stack_base   - address of memory for the SPARC
 *    size         - size in bytes of the stack area
 *    new_level    - interrupt level for this context area
 *    entry_point  - the starting execution point for this this context
 *    is_fp        - TRUE if this context is associated with an FP thread
 *
 *  Output parameters: NONE
 */

void _CPU_Context_Initialize(
  Context_Control  *the_context,
  unsigned32       *stack_base,
  unsigned32        size,
  unsigned32        new_level,
  void             *entry_point,
  boolean           is_fp
)
{
    unsigned32   stack_high;  /* highest "stack aligned" address */
    unsigned32   the_size;
    unsigned32   tmp_psr;
 
    /*
     *  On CPUs with stacks which grow down (i.e. SPARC), we build the stack
     *  based on the stack_high address.  
     */
 
    stack_high = ((unsigned32)(stack_base) + size);
    stack_high &= ~(CPU_STACK_ALIGNMENT - 1);
 
    the_size = size & ~(CPU_STACK_ALIGNMENT - 1);
 
    /*
     *  See the README in this directory for a diagram of the stack.
     */
 
    the_context->o7    = ((unsigned32) entry_point) - 8;
    the_context->o6_sp = stack_high - CPU_MINIMUM_STACK_FRAME_SIZE;
    the_context->i6_fp = stack_high;

    /*
     *  Build the PSR for the task.  Most everything can be 0 and the
     *  CWP is corrected during the context switch.
     *
     *  The EF bit determines if the floating point unit is available.
     *  The FPU is ONLY enabled if the context is associated with an FP task
     *  and this SPARC model has an FPU.
     */

    sparc_get_psr( tmp_psr );
    tmp_psr &= ~SPARC_PSR_PIL_MASK;
    tmp_psr |= (new_level << 8) & SPARC_PSR_PIL_MASK;
    tmp_psr &= ~SPARC_PSR_EF_MASK;      /* disabled by default */
    
#if (SPARC_HAS_FPU == 1)
    /*
     *  If this bit is not set, then a task gets a fault when it accesses
     *  a floating point register.  This is a nice way to detect floating
     *  point tasks which are not currently declared as such.
     */

    if ( is_fp )
      tmp_psr |= SPARC_PSR_EF_MASK;
#endif
    the_context->psr = tmp_psr;
}

/*PAGE
 *
 *  _CPU_Thread_Idle_body
 *
 *  Some SPARC implementations have low power, sleep, or idle modes.  This
 *  tries to take advantage of those models.  
 */
 
#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)

/*
 *  This is the implementation for the erc32.
 *
 *  NOTE: Low power mode was enabled at initialization time.
 */

#if defined(erc32)

void _CPU_Thread_Idle_body( void )
{
  while (1) {
    ERC32_MEC.Power_Down = 0;   /* value is irrelevant */
  }
}

#endif

#endif /* CPU_PROVIDES_IDLE_THREAD_BODY */
