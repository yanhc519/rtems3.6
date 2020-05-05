/*  asm.h
 *
 *  This include file attempts to address the problems
 *  caused by incompatible flavors of assemblers and
 *  toolsets.  It primarily addresses variations in the
 *  use of leading underscores on symbols and the requirement
 *  that register names be preceded by a %.
 *
 *
 *  NOTE: The spacing in the use of these macros
 *        is critical to them working as advertised.
 *
 *  COPYRIGHT:
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.  This file is:
 *
 *  COPYRIGHT (c) 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  $Id$
 */

#ifndef __M68k_ASM_h
#define __M68k_ASM_h

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#define ASM
#include <rtems/score/m68k.h>

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

/* ANSI concatenation macros.  */

#define CONCAT1(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) a ## b

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

#define d0 REG (d0)
#define d1 REG (d1)
#define d2 REG (d2)
#define d3 REG (d3)
#define d4 REG (d4)
#define d5 REG (d5)
#define d6 REG (d6)
#define d7 REG (d7)
#define a0 REG (a0)
#define a1 REG (a1)
#define a2 REG (a2)
#define a3 REG (a3)
#define a4 REG (a4)
#define a5 REG (a5)
#define a6 REG (a6)
#define a7 REG (a7)

#define msp REG (msp)
#define usp REG (usp)
#define isp REG (isp)
#define sr  REG (sr)
#define vbr REG (vbr)
#define dfc REG (dfc)

#define fp0 REG (fp0)
#define fp1 REG (fp1)
#define fp2 REG (fp2)
#define fp3 REG (fp3)
#define fp4 REG (fp4)
#define fp5 REG (fp5)
#define fp6 REG (fp6)
#define fp7 REG (fp7)

#define fpc REG (fpc)
#define fpi REG (fpi)
#define fps REG (fps)

/*
 *  Define macros to handle section beginning and ends.
 */


#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA
#define END_DATA
#define BEGIN_BSS
#define END_BSS
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC(sym) .globl SYM (sym)
#define EXTERN(sym) .globl SYM (sym)

#endif
/* end of include file */


