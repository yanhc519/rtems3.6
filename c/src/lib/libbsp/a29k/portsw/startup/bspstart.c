/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
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

#include <bsp.h>
#include <rtems/libio.h>
 
#include <libcsupport.h>
 
#include <string.h>
#include <fcntl.h>
 
#ifdef STACK_CHECKER_ON
#include <stackchk.h>
#endif

#ifndef lint
static char _sccsid[] = "@(#)bspstart.c 09/11/96     1.15\n";
#endif

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

#define HEAP_BLOCK_SIZE (16 * 1024)

rtems_unsigned32        heap_size = 0;
rtems_unsigned32        heap_start;

void bsp_libc_init()
{
    heap_size = 2 * 1024 * 1024; /* allocate a maximum of 2 megabytes for the heap */

    /* allocate all remaining memory to the heap */
    do {
       heap_size -= HEAP_BLOCK_SIZE;
       heap_start = _sysalloc( heap_size );
    } while ( !heap_start );

    if (!heap_start)
       rtems_fatal_error_occurred( heap_size );

    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    /*
     *  The last parameter to RTEMS_Malloc_Initialize is the "chunk"
     *  size which a multiple of will be requested on each sbrk()
     *  call by malloc().  A value of 0 indicates that sbrk() should
     *  not be called to extend the heap.
     */

    RTEMS_Malloc_Initialize((void *) heap_start, heap_size, 0);

    /*
     *  Init the RTEMS libio facility to provide UNIX-like system
     *  calls for use by newlib (ie: provide __open, __close, etc)
     *  Uses malloc() to get area for the iops, so must be after malloc init
     */

    rtems_libio_init();

    /*
     * Set up for the libc handling.
     */

    if (BSP_Configuration.ticks_per_timeslice > 0)
        libc_init(1);                /* reentrant if possible */
    else
        libc_init(0);                /* non-reentrant */

}

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
 
void
bsp_pretasking_hook(void)
{
    bsp_libc_init();

#ifdef STACK_CHECKER_ON
    /*
     *  Initialize the stack bounds checker
     *  We can either turn it on here or from the app.
     */
 
    Stack_check_Initialize();
#endif
 
#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}
 

/*
 * After drivers are setup, register some "filenames"
 * and open stdin, stdout, stderr files
 *
 * Newlib will automatically associate the files with these
 * (it hardcodes the numbers)
 */
 
void
bsp_postdriver_hook(void)
{
  int stdin_fd, stdout_fd, stderr_fd;
  int error_code;
 
  error_code = 'S' << 24 | 'T' << 16;
 
  if ((stdin_fd = __open("/dev/console", O_RDONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '0' );
 
  if ((stdout_fd = __open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '1' );
 
  if ((stderr_fd = __open("/dev/console", O_WRONLY, 0)) == -1)
    rtems_fatal_error_occurred( error_code | 'D' << 8 | '2' );
 
  if ((stdin_fd != 0) || (stdout_fd != 1) || (stderr_fd != 2))
    rtems_fatal_error_occurred( error_code | 'I' << 8 | 'O' );

  printf("allocated %d heap size, %d work space size\n",
     heap_size, BSP_Configuration.work_space_size);
  printf(" work space start 0x%x\n",(unsigned int)BSP_Configuration.work_space_start);
}


int bsp_start(
  int argc,
  char **argv,
  char **environp
)
{
  if ((argc > 0) && argv && argv[0])
    rtems_progname = argv[0];
  else
    rtems_progname = "RTEMS";

  /* set the PIA0 register wait states */
  *(volatile unsigned32 *)0x80000020 = 0x04080000;

  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  /*
   *  Copy the Configuration Table .. so we can change it
   */

  BSP_Configuration = Configuration;

  /*
   * Add 1 region for the RTEMS Malloc
   */

  BSP_Configuration.RTEMS_api_configuration->maximum_regions++;

  /*
   * Add 1 extension for newlib libc
   */

#ifdef RTEMS_NEWLIB
    BSP_Configuration.maximum_extensions++;
#endif

  /*
   * Add 1 extension for newlib libc
   */

#ifdef RTEMS_NEWLIB
    BSP_Configuration.maximum_extensions++;
#endif

#ifdef STACK_CHECKER_ON
    /*
     * Add 1 extension for stack checker
     */
 
    BSP_Configuration.maximum_extensions++;
#endif

  /*
   * Tell libio how many fd's we want and allow it to tweak config
   */

  rtems_libio_config(&BSP_Configuration, BSP_LIBIO_MAX_FDS);

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  BSP_Configuration.work_space_start = _sysalloc( BSP_Configuration.work_space_size + 512 );
  if (!BSP_Configuration.work_space_start)
    rtems_fatal_error_occurred( BSP_Configuration.work_space_size );
   
  BSP_Configuration.work_space_start = (void *) ((unsigned int)((char *)BSP_Configuration.work_space_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1));

  /*
   *  initialize the CPU table for this BSP
   */

  /*
   *  we do not use the pretasking_hook
   */

  Cpu_table.pretasking_hook = bsp_pretasking_hook;  /* init libc, etc. */

  Cpu_table.predriver_hook = NULL;

  Cpu_table.postdriver_hook = bsp_postdriver_hook;

  Cpu_table.idle_task = NULL;  /* do not override system IDLE task */

  Cpu_table.do_zero_of_workspace = TRUE;

  Cpu_table.interrupt_stack_size = 4096;

  Cpu_table.extra_system_initialization_stack = 0;

  /*
   *  Don't forget the other CPU Table entries.
   */

  _settrap( 109,&a29k_enable_sup);
  _settrap( 110,&a29k_disable_sup);
  _settrap( 111,&a29k_enable_all_sup);
  _settrap( 112,&a29k_disable_all_sup);
  _settrap( 106,&a29k_context_switch_sup);
  _settrap( 107,&a29k_context_restore_sup);
  _settrap( 108,&a29k_context_save_sup);
  _settrap( 105,&a29k_sigdfl_sup);

  /*
   *  Start RTEMS
   */

  rtems_initialize_executive( &BSP_Configuration, &Cpu_table );

  bsp_cleanup();

  return 0;
}
