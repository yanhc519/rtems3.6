/*  Spurious_driver
 *
 *  This routine installs spurious interrupt handlers for the efi68k.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdio.h>

const char * const _Spurious_Error_[] = {"Reset","Bus Error","Address Error",
   "Illegal Instruction","Zero Division","CHK, CHK2 Instruction",
   "TRAPcc, TRAPV Instruction","Privilege Violation","Trace",
   "Line 1010 Emulation","Line 1111 Emulation","Hardware Breakpoint",
   "Coprocessor Protocal Violation",
   "Format Error ans Uninitialized Interrupt","Unassigned",
   "Spurious Interrupt","AVec1","AVec2","AVec3","AVec4","AVec5","AVec6",
   "AVec7","Trap Instruction","Debug","Reboot","Reserved Coprocessor",
   "Reserved Unassigned","User Defined"};

rtems_isr Spurious_Isr(
  rtems_vector_number vector
)
{
  int sp = 0;
  const char * const VectDescrip[] = {
    _Spurious_Error_[0],   _Spurious_Error_[0],  _Spurious_Error_[1], 
    _Spurious_Error_[2],   _Spurious_Error_[3],  _Spurious_Error_[4], 
    _Spurious_Error_[5],   _Spurious_Error_[6],  _Spurious_Error_[7], 
    _Spurious_Error_[8],   _Spurious_Error_[9], _Spurious_Error_[10], 
    _Spurious_Error_[11], _Spurious_Error_[12], _Spurious_Error_[13], 
    _Spurious_Error_[13], _Spurious_Error_[14], _Spurious_Error_[14], 
    _Spurious_Error_[14], _Spurious_Error_[14], _Spurious_Error_[14], 
    _Spurious_Error_[14], _Spurious_Error_[14], _Spurious_Error_[14], 
    _Spurious_Error_[15], _Spurious_Error_[16], _Spurious_Error_[17], 
    _Spurious_Error_[18], _Spurious_Error_[19], _Spurious_Error_[20], 
    _Spurious_Error_[21], _Spurious_Error_[22], _Spurious_Error_[23], 
    _Spurious_Error_[24], _Spurious_Error_[23], _Spurious_Error_[23], 
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23], 
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23], 
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[23], 
    _Spurious_Error_[23], _Spurious_Error_[23], _Spurious_Error_[25], 
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26], 
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26], 
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[26], 
    _Spurious_Error_[26], _Spurious_Error_[26], _Spurious_Error_[27], 
    _Spurious_Error_[27], _Spurious_Error_[27], _Spurious_Error_[27], 
    _Spurious_Error_[27], _Spurious_Error_[28]};

  asm volatile ( "movea.l   %%sp,%0 " : "=a" (sp) : "0" (sp) );

  _CPU_ISR_Set_level( 7 );
  _UART_flush();

  RAW_PUTS("\n\rRTEMS: Spurious interrupt: ");
  RAW_PUTS((char *)VectDescrip[( (vector>64) ? 64 : vector )]);
  RAW_PUTS("\n\rRTEMS:    Vector: ");
  RAW_PUTI(vector);
  RAW_PUTS(" sp: ");
  RAW_PUTI(sp);
  RAW_PUTS("\n\r");

  bsp_cleanup();

  for(;;);
}

void Spurious_Initialize(void)
{
  rtems_vector_number vector;

  for ( vector = 0x0 ; vector <= 0xFF ; vector++ )
    (void) set_vector( Spurious_Isr, vector, 1 );
}
