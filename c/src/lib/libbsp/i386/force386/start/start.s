/*  start.s
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
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

#include "asm.h"

/*
 *  A Descriptor table register has the following format
 */

.set DTR_LIMIT,   0    # offset of two byte limit
.set DTR_BASE,    2    # offset of four byte base address
.set DTR_SIZE,    6    # size of DTR register

        BEGIN_DATA
        EXTERN (Do_Load_IDT)
        EXTERN (Do_Load_GDT)
        END_DATA

        BEGIN_CODE

        PUBLIC (start)                   # GNU default entry point

        EXTERN (boot_card)
        EXTERN (load_segments)
        EXTERN (exit)

SYM (start):
        nop
        cli                             # DISABLE INTERRUPTS!!!
/*
 *  Load the segment registers
 *
 *  NOTE: Upon return, gs will contain the segment descriptor for
 *        a segment which maps directly to all of physical memory.
 */
        jmp     SYM (_load_segments)    # load board dependent segments

/*
 *  Set up the stack
 */

        PUBLIC (_establish_stack)
SYM (_establish_stack):

        movl    $end,eax               # eax = end of bss/start of heap
        addl    $heap_size,eax         # eax = end of heap
        movl    eax,stack_start        # Save for brk() routine
        addl    $stack_size,eax        # make room for stack
        andl    $0xffffffc0,eax        # align it on 16 byte boundary
        movl    eax,esp               # set stack pointer
        movl    eax,ebp               # set base pointer
/*
 *  Zero out the BSS segment
 */
SYM (zero_bss):
        cld                             # make direction flag count up
        movl    $ SYM (end),ecx        # find end of .bss
        movl    $ SYM (_bss_start),edi # edi = beginning of .bss
        subl    edi,ecx               # ecx = size of .bss in bytes
        shrl    ecx                    # size of .bss in longs
        shrl    ecx
        xorl    eax,eax               # value to clear out memory
        repne                           # while ecx != 0
        stosl                           #   clear a long in the bss

/*
 *  Copy the Global Descriptor Table to our space
 */

        sgdt    SYM (_Original_GDTR)          # save original GDT
        movzwl  SYM (_Original_GDTR)+DTR_LIMIT,ecx
                                        /* size of GDT in bytes; limit is */
                                        /*   8192 entries * 8 bytes per */

        /*
         * make ds:esi point to the original GDT
         */

        movl    SYM (_Original_GDTR)+DTR_BASE,esi
        push    ds                     # save ds
        movw    gs,ax
        movw    ax,ds

        /*
         * make es:edi point to the new (our copy) GDT
         */

        movl    $ SYM (_Global_descriptor_table),edi

        rep
        movsb                            # copy the GDT (ds:esi -> es:edi)

        pop     ds                      # restore ds

        /*
         * Build and load new contents of GDTR
         */
        movw     SYM (_Original_GDTR)+DTR_LIMIT,ecx # set new limit
        movw    cx, SYM (_New_GDTR)+DTR_LIMIT

        push    $ SYM (_Global_descriptor_table)
        push    es
        call    SYM (i386_Logical_to_physical)
        addl    $6,esp
        movl    eax, SYM (_New_GDTR)+DTR_BASE      # set new base

        cmpb    $0, SYM (_Do_Load_GDT)         # Should the new GDT be loaded?
        je      SYM (no_gdt_load)              # NO, then branch
        lgdt    SYM (_New_GDTR)                # load the new GDT
SYM (no_gdt_load):

/*
 *  Copy the Interrupt Descriptor Table to our space
 */

        sidt    SYM (_Original_IDTR)    # save original IDT
        movzwl  SYM (_Original_IDTR)+DTR_LIMIT,ecx
                                        /* size of IDT in bytes; limit is */
                                        /*   256 entries * 8 bytes per */


        /*
         * make ds:esi point to the original IDT
         */
        movl    SYM (_Original_IDTR)+DTR_BASE,esi

        push    ds                     # save ds
        movw    gs,ax
        movw    ax,ds

        /*
         * make es:edi point to the new (our copy) IDT
         */
        movl    $ SYM (Interrupt_descriptor_table),edi

        rep
        movsb                            # copy the IDT (ds:esi -> es:edi)
        pop     ds                      # restore ds

        /*
         * Build and load new contents of IDTR
         */
        movw    SYM (_Original_IDTR+DTR_LIMIT),ecx # set new limit
        movw    cx,SYM (_New_IDTR)+DTR_LIMIT

        push    $ SYM (Interrupt_descriptor_table)
        push    es
        call    SYM (i386_Logical_to_physical)
        addl    $6,esp
        movl    eax, SYM (_New_IDTR)+DTR_BASE      # set new base

        cmpb    $0, SYM (_Do_Load_IDT)    # Should the new IDT be loaded?
        je      SYM (no_idt_load)         # NO, then branch
        lidt    SYM (_New_IDTR)           # load the new IDT
SYM (no_idt_load):

/*
 *  Initialize the i387.
 *
 *  Using the NO WAIT form of the instruction insures that
 *  if it is not present the board will not lock up or get an
 *  exception.
 */

        fninit                           # MUST USE NO-WAIT FORM

/*
 *  Transfer control to User's Board Support Package
 */
        pushl   $0                       # environp
        pushl   $0                       # argv
        pushl   $0                       # argc
        call    SYM (boot_card)
        addl    $12,esp

/*
 *  Clean up
 */

        EXTERN (return_to_monitor)

        PUBLIC (Bsp_cleanup)
SYM (Bsp_cleanup):
        cmpb    $0, SYM (_Do_Load_IDT)      # Was the new IDT loaded?
        je      SYM (no_idt_restore)        # NO, then branch
        lidt    SYM (_Original_IDTR)        # restore the new IDT
SYM (no_idt_restore):

        cmpb    $0, SYM (_Do_Load_GDT)      # Was the new GDT loaded?
        je      SYM (no_gdt_restore)        # NO, then branch
        lgdt    SYM (_Original_GDTR)        # restore the new GDT
SYM (no_gdt_restore):
        jmp     SYM (_return_to_monitor)

END_CODE

BEGIN_DATA

        .align  2
        PUBLIC (start_frame)
SYM (start_frame):
        .long  0

        PUBLIC (stack_start)
SYM (stack_start):
        .long  0

END_DATA

BEGIN_BSS

        PUBLIC(heap_size)
        .set  heap_size,0x2000

        PUBLIC(stack_size)
        .set  stack_size,0x1000

        PUBLIC (Interrupt_descriptor_table)
SYM (Interrupt_descriptor_table):
	.space 256*8

        PUBLIC (_Original_IDTR)
SYM (_Original_IDTR):
	.space DTR_SIZE

        PUBLIC (_New_IDTR)
SYM (_New_IDTR):
	.space DTR_SIZE

        PUBLIC (_Global_descriptor_table)
SYM (_Global_descriptor_table):
	.space 8192*8

        PUBLIC (_Original_GDTR)
SYM (_Original_GDTR):
	.space DTR_SIZE

        PUBLIC (_New_GDTR)
SYM (_New_GDTR):
	.space DTR_SIZE

        PUBLIC (_Physical_base_of_ds)
SYM (_Physical_base_of_ds):
	.space 4

        PUBLIC (_Physical_base_of_cs)
SYM (_Physical_base_of_cs):
	.space 4

END_BSS
END
