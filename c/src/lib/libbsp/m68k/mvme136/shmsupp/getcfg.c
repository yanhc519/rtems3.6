/*  void Shm_Get_configuration( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the Cyclone CVME961.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  The MPCSR interrupt on the MVME136 is used as an interprocessor
 *          interrupt.   The capablities of the MPCSR are used to generate
 *          interprocessor interrupts for up to eight nodes.
 *
 *          The following table illustrates the configuration limitations:
 *
 *                                   BUS     MAX
 *                          MODE    ENDIAN  NODES
 *                        ========= ====== =======
 *                         POLLED   LITTLE  2+
 *                        INTERRUPT LITTLE  2-8
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

#include <rtems.h>
#include "shm.h"

#define INTERRUPT 1                   /* MVME136 target supports both */
#define POLLING   0                   /* polling and interrupt modes  */

shm_config_table BSP_shm_cfgtbl;

rtems_unsigned32 *BSP_int_address()
{
  rtems_unsigned32 id, offset;

  id      = (rtems_unsigned32) *(rtems_unsigned8 *)0xfffb0061;
  offset  = ((id & 0x1f) << 5) | ((id & 0xe0) << 8);
  offset |= 0xffff000b;
  return( (rtems_unsigned32 * ) offset );
}

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
   BSP_shm_cfgtbl.base         = (rtems_unsigned32 *)0x20000000;
   BSP_shm_cfgtbl.length       = 1 * MEGABYTE;
   BSP_shm_cfgtbl.format       = SHM_BIG;

   BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt;

#ifdef NEUTRAL_BIG
   BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
   BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#if (POLLING==1)
   BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
   BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
   BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
   BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
   BSP_shm_cfgtbl.Intr.address = BSP_int_address();
   BSP_shm_cfgtbl.Intr.value   = 0x80;
   BSP_shm_cfgtbl.Intr.length  = BYTE;
#endif

   *shmcfg = &BSP_shm_cfgtbl;

}
