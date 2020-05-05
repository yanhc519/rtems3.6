/*  timer_isr()
 *
 *  This routine provides the ISR for the Z8036 timer on the MVME136
 *  board.   The timer is set up to generate an interrupt at maximum
 *  intervals.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
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

#include "asm.h"

        BEGIN_CODE

        EXTERN (Ttimer_val)

        PUBLIC (timerisr)
SYM (timerisr):
        addl    $250, SYM (Ttimer_val)   # another 250 microseconds
        iret

END_CODE
END
