/*  m68k.h
 *
 *  This include file contains information pertaining to the Motorola
 *  m68xxx processor family.
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

#ifndef __M68k_h
#define __M68k_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This section contains the information required to build
 *  RTEMS for a particular member of the Motorola MC68xxx
 *  family.  It does this by setting variables to indicate
 *  which implementation dependent features are present in
 *  a particular member of the family.
 *
 *  Currently recognized:
 *     -m68000
 *     -m68000 -msoft-float
 *     -m68020
 *     -m68020 -msoft-float
 *     -m68030
 *     -m68040 -msoft-float
 *     -m68040
 *     -m68040 -msoft-float
 *     -m68060
 *     -m68060 -msoft-float
 *     -m68302        (no FP) (deprecated, use -m68000)
 *     -m68332        (no FP) (deprecated, use -mcpu32)
 *     -mcpu32        (no FP)
 *     -m5200         (no FP)
 *
 *  As of gcc 2.8.1 and egcs 1.1, there is no distinction made between
 *  the CPU32 and CPU32+.  The option -mcpu32 generates code which can
 *  be run on either core.  RTEMS distinguishes between these two cores
 *  because they have different alignment rules which impact performance.
 *  If you are using a CPU32+, then the symbol RTEMS__mcpu32p__ should
 *  be defined in your custom file (see make/custom/gen68360.cfg for an
 *  example of how to do this.  If gcc ever distinguishes between these
 *  two cores, then RTEMS__mcpu32p__ usage will be replaced with the
 *  appropriate compiler defined predefine.
 *
 *  Here is some information on the 040 variants (courtesy of Doug McBride,
 *  mcbride@rodin.colorado.edu):
 *
 *    "The 68040 is a superset of the 68EC040 and the 68LC040.  The
 *    68EC040 and 68LC040 do not have FPU's.  The 68LC040 and the
 *    68EC040 have renamed the DLE pin as JS0 which must be tied to
 *    Gnd or Vcc. The 68EC040 has renamed the MDIS pin as JS1.  The
 *    68EC040 has access control units instead of memory management units.
 *    The 68EC040 should not have the PFLUSH or PTEST instructions executed
 *    (cause an indeterminate result).  The 68EC040 and 68LC040 do not
 *    implement the DLE or multiplexed bus modes.  The 68EC040 does not
 *    implement the output buffer impedance selection mode of operation."
 *
 *  M68K_HAS_EXTB_L is used to enable/disable usage of the extb.l instruction
 *  which is not available for 68000 or 68ec000 cores (68000, 68001, 68008, 
 *  68010, 68302, 68306, 68307).  This instruction is available on the 68020
 *  up and the cpu32 based models.  
 *
 *  M68K_HAS_MISALIGNED is non-zero if the CPU allows byte-misaligned
 *  data access (68020, 68030, 68040, 68060, CPU32+).
 *
 *  NOTE:
 *    Eventually it would be nice to evaluate doing a lot of this section
 *    by having each model specify which core it uses and then go from there.
 */

#if defined(__mc68020__)
 
#define CPU_MODEL_NAME          "m68020"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
# if defined (__HAVE_68881__)
# define M68K_HAS_FPU            1
# define M68K_HAS_FPSP_PACKAGE   0
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif
 
#elif defined(__mc68030__)
 
#define CPU_MODEL_NAME          "m68030"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
# if defined (__HAVE_68881__)
# define M68K_HAS_FPU            1
# define M68K_HAS_FPSP_PACKAGE   0
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif
 
#elif defined(__mc68040__)

#define CPU_MODEL_NAME          "m68040"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 1
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
# if defined (__HAVE_68881__)
# define M68K_HAS_FPU            1
# define M68K_HAS_FPSP_PACKAGE   1
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif
 
#elif defined(__mc68060__)

#define CPU_MODEL_NAME          "m68060"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_BFFFO           1
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
# if defined (__HAVE_68881__)
# define M68K_HAS_FPU            1
# define M68K_HAS_FPSP_PACKAGE   1
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif
 
#elif defined(__mc68302__)
#define CPU_MODEL_NAME          "m68302"
#define M68K_HAS_VBR             0
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_BFFFO           0
#define M68K_HAS_PREINDEXING     0
#define M68K_HAS_EXTB_L          0
#define M68K_HAS_MISALIGNED      0
#define M68K_HAS_FPU             0
#define M68K_HAS_FPSP_PACKAGE    0

  /* gcc and egcs do not distinguish between CPU32 and CPU32+ */
#elif defined(RTEMS__mcpu32p__)
 
#define CPU_MODEL_NAME          "mcpu32+"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_BFFFO           0
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
#define M68K_HAS_FPU             0
#define M68K_HAS_FPSP_PACKAGE    0

#elif defined(__mcpu32__)
 
#define CPU_MODEL_NAME          "mcpu32"
#define M68K_HAS_VBR             1
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_BFFFO           0
#define M68K_HAS_PREINDEXING     1
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      0
#define M68K_HAS_FPU             0
#define M68K_HAS_FPSP_PACKAGE    0

#elif defined(__mcf5200__)
/* Motorola ColdFire V2 core - RISC/68020 hybrid */ 
#define CPU_MODEL_NAME         "m5200"
#define M68K_HAS_VBR             1
#define M68K_HAS_BFFFO           0
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_PREINDEXING     0
#define M68K_HAS_EXTB_L          1
#define M68K_HAS_MISALIGNED      1
#define M68K_HAS_FPU             0
#define M68K_HAS_FPSP_PACKAGE    0
#define M68K_COLDFIRE_ARCH       1

#elif defined(__mc68000__)
 
#define CPU_MODEL_NAME          "m68000"
#define M68K_HAS_VBR             0
#define M68K_HAS_SEPARATE_STACKS 0
#define M68K_HAS_BFFFO           0
#define M68K_HAS_PREINDEXING     0
#define M68K_HAS_EXTB_L          0
#define M68K_HAS_MISALIGNED      0
# if defined (__HAVE_68881__)
# define M68K_HAS_FPU            1
# define M68K_HAS_FPSP_PACKAGE   0
# else
# define M68K_HAS_FPU            0
# define M68K_HAS_FPSP_PACKAGE   0
# endif

#else

#error "Unsupported CPU model -- are you sure you're running a 68k compiler?"

#endif

/*
 *  If the above did not specify a ColdFire architecture, then set
 *  this flag to indicate that it is not a ColdFire CPU.
 */

#if !defined(M68K_COLDFIRE_ARCH)
#define M68K_COLDFIRE_ARCH       0
#endif

/*
 *  Define the name of the CPU family.
 */

#if ( M68K_COLDFIRE_ARCH == 1 )
  #define CPU_NAME "Motorola ColdFire"
#else
  #define CPU_NAME "Motorola MC68xxx"
#endif

#ifndef ASM

#if ( M68K_COLDFIRE_ARCH == 1 )
#define m68k_disable_interrupts( _level ) \
   do { register unsigned32 _tmpsr = 0x0700; \
        asm volatile ( "move.w %%sr,%0\n\t" \
 		       "or.l   %0,%1\n\t" \
 		       "move.w %1,%%sr" \
 		       : "=d" (_level), "=d"(_tmpsr) : "1"(_tmpsr) ); \
   } while( 0 )
#else
#define m68k_disable_interrupts( _level ) \
  asm volatile ( "move.w  %%sr,%0\n\t" \
                 "or.w    #0x0700,%%sr" \
                    : "=d" (_level))
#endif

#define m68k_enable_interrupts( _level ) \
  asm volatile ( "move.w  %0,%%sr " : : "d" (_level));

#if ( M68K_COLDFIRE_ARCH == 1 )
#define m68k_flash_interrupts( _level ) \
   do { register unsigned32 _tmpsr = 0x0700; \
	asm volatile ( "move.w %2,%%sr\n\t" \
		       "or.l   %2,%1\n\t" \
		       "move.w %1,%%sr" \
		       : "=d"(_tmpsr) : "0"(_tmpsr), "d"(_level) ); \
   } while( 0 )
#else
#define m68k_flash_interrupts( _level ) \
  asm volatile ( "move.w  %0,%%sr\n\t" \
                 "or.w    #0x0700,%%sr" \
                    : : "d" (_level))
#endif

#define m68k_get_interrupt_level( _level ) \
  do { \
    register unsigned32 _tmpsr; \
    \
    asm volatile( "move.w %%sr,%0" : "=d" (_tmpsr)); \
    _level = (_tmpsr & 0x0700) >> 8; \
  } while (0)
    
#define m68k_set_interrupt_level( _newlevel ) \
  do { \
    register unsigned32 _tmpsr; \
    \
    asm volatile( "move.w  %%sr,%0" : "=d" (_tmpsr)); \
    _tmpsr = (_tmpsr & 0xf8ff) | ((_newlevel) << 8); \
    asm volatile( "move.w  %0,%%sr" : : "d" (_tmpsr)); \
  } while (0)

#if ( M68K_HAS_VBR == 1 && M68K_COLDFIRE_ARCH == 0 )
#define m68k_get_vbr( vbr ) \
  asm volatile ( "movec   %%vbr,%0 " : "=r" (vbr))

#define m68k_set_vbr( vbr ) \
  asm volatile ( "movec   %0,%%vbr " : : "r" (vbr))

#elif ( M68K_COLDFIRE_ARCH == 1 )
#define m68k_get_vbr( _vbr ) _vbr = (void *)_VBR

#define m68k_set_vbr( _vbr ) \
    asm volatile ("move.l  %%a7,%%d1 \n\t" \
	          "move.l  %0,%%a7\n\t"    \
	          "movec   %%a7,%%vbr\n\t" \
                  "move.l  %%d1,%%a7\n\t"  \
		  : : "d" (_vbr) : "d1" );
  
#else
#define m68k_get_vbr( _vbr ) _vbr = (void *)_VBR
#define m68k_set_vbr( _vbr )
#endif

/*
 *  The following routine swaps the endian format of an unsigned int.
 *  It must be static because it is referenced indirectly.
 */

static inline unsigned int m68k_swap_u32(
  unsigned int value
)
{
  unsigned int swapped = value;

  asm volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );
  asm volatile( "swap  %0"    : "=d" (swapped) : "0" (swapped) );
  asm volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );

  return( swapped );
}

static inline unsigned int m68k_swap_u16(
  unsigned int value
)
{
  unsigned short swapped = value;

  asm volatile( "rorw  #8,%0" : "=d" (swapped) : "0" (swapped) );

  return( swapped );
}

/* XXX this is only valid for some m68k family members and should be fixed */

#define m68k_enable_caching() \
  { register unsigned32 _ctl=0x01; \
    asm volatile ( "movec   %0,%%cacr" \
                       : "=d" (_ctl) : "0" (_ctl) ); \
  }

#define CPU_swap_u32( value )  m68k_swap_u32( value )
#define CPU_swap_u16( value )  m68k_swap_u16( value )

#endif  /* !ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
