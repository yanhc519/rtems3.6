/*  sparc.h
 *
 *  This include file contains information pertaining to the SPARC 
 *  processor family.
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

#ifndef _INCLUDE_SPARC_h
#define _INCLUDE_SPARC_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_THE_CPU_MODEL" is replaced
 *        with the name of the appropriate macro for this target CPU.
 */
 
#ifdef sparc
#undef sparc
#endif
#define sparc

#ifdef REPLACE_THIS_WITH_THE_CPU_MODEL
#undef REPLACE_THIS_WITH_THE_CPU_MODEL
#endif
#define REPLACE_THIS_WITH_THE_CPU_MODEL
 
#ifdef REPLACE_THIS_WITH_THE_BSP
#undef REPLACE_THIS_WITH_THE_BSP
#endif
#define REPLACE_THIS_WITH_THE_BSP

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the "sparc" family.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  Currently recognized feature flags:
 *
 *    + SPARC_HAS_FPU 
 *        0 - no HW FPU
 *        1 - has HW FPU (assumed to be compatible w/90C602)
 *
 *    + SPARC_HAS_BITSCAN 
 *        0 - does not have scan instructions
 *        1 - has scan instruction  (not currently implemented)
 * 
 *    + SPARC_NUMBER_OF_REGISTER_WINDOWS
 *        8 is the most common number supported by SPARC implementations.
 *        SPARC_PSR_CWP_MASK is derived from this value.
 * 
 *    + SPARC_HAS_LOW_POWER_MODE
 *        0 - does not have low power mode support (or not supported)
 *        1 - has low power mode and thus a CPU model dependent idle task.
 *
 */
 
#if defined(erc32)
 
#define CPU_MODEL_NAME                   "erc32"
#define SPARC_HAS_FPU                    1
#define SPARC_HAS_BITSCAN                0
#define SPARC_NUMBER_OF_REGISTER_WINDOWS 8
#define SPARC_HAS_LOW_POWER_MODE         1
 
#else
 
#error "Unsupported CPU Model"
 
#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "SPARC"

/*
 *  Miscellaneous constants
 */

/*
 *  PSR masks and starting bit positions
 *
 *  NOTE: Reserved bits are ignored.
 */

#if (SPARC_NUMBER_OF_REGISTER_WINDOWS == 8)
#define SPARC_PSR_CWP_MASK               0x07   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 16)
#define SPARC_PSR_CWP_MASK               0x0F   /* bits  0 -  4 */
#elif (SPARC_NUMBER_OF_REGISTER_WINDOWS == 32)
#define SPARC_PSR_CWP_MASK               0x1F   /* bits  0 -  4 */
#else
#error "Unsupported number of register windows for this cpu"
#endif

#define SPARC_PSR_ET_MASK   0x00000020   /* bit   5 */
#define SPARC_PSR_PS_MASK   0x00000040   /* bit   6 */
#define SPARC_PSR_S_MASK    0x00000080   /* bit   7 */
#define SPARC_PSR_PIL_MASK  0x00000F00   /* bits  8 - 11 */
#define SPARC_PSR_EF_MASK   0x00001000   /* bit  12 */
#define SPARC_PSR_EC_MASK   0x00002000   /* bit  13 */
#define SPARC_PSR_ICC_MASK  0x00F00000   /* bits 20 - 23 */
#define SPARC_PSR_VER_MASK  0x0F000000   /* bits 24 - 27 */
#define SPARC_PSR_IMPL_MASK 0xF0000000   /* bits 28 - 31 */

#define SPARC_PSR_CWP_BIT_POSITION   0   /* bits  0 -  4 */
#define SPARC_PSR_ET_BIT_POSITION    5   /* bit   5 */
#define SPARC_PSR_PS_BIT_POSITION    6   /* bit   6 */
#define SPARC_PSR_S_BIT_POSITION     7   /* bit   7 */
#define SPARC_PSR_PIL_BIT_POSITION   8   /* bits  8 - 11 */
#define SPARC_PSR_EF_BIT_POSITION   12   /* bit  12 */
#define SPARC_PSR_EC_BIT_POSITION   13   /* bit  13 */
#define SPARC_PSR_ICC_BIT_POSITION  20   /* bits 20 - 23 */
#define SPARC_PSR_VER_BIT_POSITION  24   /* bits 24 - 27 */
#define SPARC_PSR_IMPL_BIT_POSITION 28   /* bits 28 - 31 */

#ifndef ASM

/*
 *  Standard nop
 */

#define nop() \
  do { \
    asm volatile ( "nop" ); \
  } while ( 0 )

/*
 *  Get and set the PSR
 */

#define sparc_get_psr( _psr ) \
  do { \
     (_psr) = 0; \
     asm volatile( "rd %%psr, %0" :  "=r" (_psr) : "0" (_psr) ); \
  } while ( 0 )

#define sparc_set_psr( _psr ) \
  do { \
    asm volatile ( "mov  %0, %%psr " : "=r" ((_psr)) : "0" ((_psr)) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

/*
 *  Get and set the TBR
 */

#define sparc_get_tbr( _tbr ) \
  do { \
     (_tbr) = 0; /* to avoid unitialized warnings */ \
     asm volatile( "rd %%tbr, %0" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

#define sparc_set_tbr( _tbr ) \
  do { \
     asm volatile( "wr %0, 0, %%tbr" :  "=r" (_tbr) : "0" (_tbr) ); \
  } while ( 0 )

/*
 *  Get and set the WIM
 */

#define sparc_get_wim( _wim ) \
  do { \
    asm volatile( "rd %%wim, %0" :  "=r" (_wim) : "0" (_wim) ); \
  } while ( 0 )

#define sparc_set_wim( _wim ) \
  do { \
    asm volatile( "wr %0, %%wim" :  "=r" (_wim) : "0" (_wim) ); \
    nop(); \
    nop(); \
    nop(); \
  } while ( 0 )

/*
 *  Get and set the Y
 */
 
#define sparc_get_y( _y ) \
  do { \
    asm volatile( "rd %%y, %0" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )
 
#define sparc_set_y( _y ) \
  do { \
    asm volatile( "wr %0, %%y" :  "=r" (_y) : "0" (_y) ); \
  } while ( 0 )

/*
 *  Manipulate the interrupt level in the psr 
 *
 */

#define sparc_disable_interrupts( _level ) \
  do { \
    register unsigned int _newlevel; \
    \
    sparc_get_psr( _level ); \
    (_newlevel) = (_level) | SPARC_PSR_PIL_MASK; \
    sparc_set_psr( _newlevel ); \
  } while ( 0 )
 
#define sparc_enable_interrupts( _level ) \
  do { \
    unsigned int _tmp; \
    \
    sparc_get_psr( _tmp ); \
    _tmp &= ~SPARC_PSR_PIL_MASK; \
    _tmp |= (_level) & SPARC_PSR_PIL_MASK; \
    sparc_set_psr( _tmp ); \
  } while ( 0 ) 
  
#define sparc_flash_interrupts( _level ) \
  do { \
    register unsigned32 _ignored = 0; \
    \
    sparc_enable_interrupts( (_level) ); \
    sparc_disable_interrupts( _ignored ); \
  } while ( 0 )

#define sparc_set_interrupt_level( _new_level ) \
  do { \
    register unsigned32 _new_psr_level = 0; \
    \
    sparc_get_psr( _new_psr_level ); \
    _new_psr_level &= ~SPARC_PSR_PIL_MASK; \
    _new_psr_level |= \
      (((_new_level) << SPARC_PSR_PIL_BIT_POSITION) & SPARC_PSR_PIL_MASK); \
    sparc_set_psr( _new_psr_level ); \
  } while ( 0 )

#define sparc_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _psr_level = 0; \
    \
    sparc_get_psr( _psr_level ); \
    (_level) = \
      (_psr_level & SPARC_PSR_PIL_MASK) >> SPARC_PSR_PIL_BIT_POSITION; \
  } while ( 0 )

#endif

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_SPARC_h */
/* end of include file */
