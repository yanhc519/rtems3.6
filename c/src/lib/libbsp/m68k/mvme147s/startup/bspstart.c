/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <libcsupport.h>

#include <string.h>

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */
 
void bsp_pretasking_hook(void)
{
    extern int end;
    rtems_unsigned32        heap_start;

    heap_start = (rtems_unsigned32) &end;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    bsp_libc_init((void *) heap_start, 64 * 1024, 0);
 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 
/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  m68k_isr_entry *monitors_vector_table;
  int             index;
  rtems_unsigned8 node_number;

  monitors_vector_table = (m68k_isr_entry *)0;   /* 147Bug Vectors are at 0 */
  m68k_set_vbr( monitors_vector_table );

  for ( index=2 ; index<=255 ; index++ )
    M68Kvec[ index ] = monitors_vector_table[ 32 ];

  M68Kvec[  2 ] = monitors_vector_table[  2 ];   /* bus error vector */
  M68Kvec[  4 ] = monitors_vector_table[  4 ];   /* breakpoints vector */
  M68Kvec[  9 ] = monitors_vector_table[  9 ];   /* trace vector */
  M68Kvec[ 47 ] = monitors_vector_table[ 47 ];   /* system call vector */

  m68k_set_vbr( &M68Kvec );

  pcc->int_base_vector = PCC_BASE_VECTOR & 0xF0; 
  /* Set the PCC int vectors base */
  
  /* VME shared memory configuration */
  /* Only the first node shares its top 128k DRAM */

  vme_lcsr->utility_interrupt_vector = VME_BASE_VECTOR & 0xF8;
  /* Set VMEchip base interrupt vector */
  vme_lcsr->utility_interrupt_mask |= 0x02;
  /* Enable SIGLP interruption (see shm support) */
  pcc->general_purpose_control &= 0x10;
  /* Enable VME master interruptions */
  
  if (vme_lcsr->system_controller & 0x01) {
    /* the board is system controller */
    vme_lcsr->system_controller = 0x08; 
    /* Make VME access round-robin */
  }

  
  node_number = 
    (rtems_unsigned8) 
    (Configuration.User_multiprocessing_table->node - 1) & 0xF;
  /* Get and store node ID, first node_number = 0 */
  vme_gcsr->board_identification = node_number;

  vme_lcsr->gcsr_base_address = node_number;
  /* Setup the base address of this board's gcsr */
  vme_lcsr->timer_configuration = 0x6a;
  /* Enable VME time outs, maximum periods */

  if (node_number == 0) {
    pcc->slave_base_address = 0x01;
    /* Set local DRAM base address on the VME bus to the DRAM size */

    vme_lcsr->vme_bus_requester = 0x80;
    while (! (vme_lcsr->vme_bus_requester & 0x40));
    /* Get VMEbus mastership */
    vme_lcsr->slave_address_modifier = 0xfb;
    /* Share everything */
    vme_lcsr->slave_configuration = 0x80;
    /* Share local DRAM */
    vme_lcsr->vme_bus_requester = 0x0;
    /* release bus */
  } else {
    pcc->slave_base_address = 0;
    /* Set local DRAM base address on the VME bus to 0 */

    vme_lcsr->vme_bus_requester = 0x80;
    while (! (vme_lcsr->vme_bus_requester & 0x40));
    /* Get VMEbus mastership */
    vme_lcsr->slave_address_modifier = 0x08;
    /* Share only the short adress range */
    vme_lcsr->slave_configuration = 0;
    /* Don't share local DRAM */
    vme_lcsr->vme_bus_requester = 0x0;
    /* release bus */
  }

  vme_lcsr->master_address_modifier = 0;
  /* Automatically set the address modifier */
  vme_lcsr->master_configuration = 1;
  /* Disable D32 transfers : they don't work on my VMEbus rack */

  m68k_enable_caching();

  /*
   *  we only use a hook to get the C library initialized.
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.interrupt_vector_table = (m68k_isr_entry *) &M68Kvec;
  Cpu_table.interrupt_stack_size = 4096;

  BSP_Configuration.work_space_start = (void *)
     (RAM_END - BSP_Configuration.work_space_size);

}
