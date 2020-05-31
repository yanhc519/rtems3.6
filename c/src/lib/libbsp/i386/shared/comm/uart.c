/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 *
 *  $Id$
 */

#include <bsp.h>
#include <irq.h>
#include <uart.h>
#include <rtems/libio.h>
#include <assert.h>

/*
 * Basic 16552 driver
 */

struct uart_data
{
  int hwFlow;
  int baud;
};

static struct uart_data uart_data[2];

/* 
 * Macros to read/wirte register of uart, if configuration is
 * different just rewrite these macros
 */ 

static inline unsigned char
uread(int uart, unsigned int reg)
{
  register unsigned char val;

  if(uart == 0)
    {
      i386_inport_byte(COM1_BASE_IO+reg, val);
    }
  else
    {
      i386_inport_byte(COM2_BASE_IO+reg, val);
    }

  return val;
}

static inline void      
uwrite(int uart, int reg, unsigned int val)
{
  if(uart == 0)
    {
      i386_outport_byte(COM1_BASE_IO+reg, val);
    } 
  else
    {
      i386_outport_byte(COM2_BASE_IO+reg, val);
    }
}

#ifdef UARTDEBUG
    static void
uartError(int uart)
{
  unsigned char uartStatus, dummy;

  uartStatus = uread(uart, LSR);
  dummy = uread(uart, RBR);

  if (uartStatus & OE)
    printk("********* Over run Error **********\n");
  if (uartStatus & PE)
    printk("********* Parity Error   **********\n");
  if (uartStatus & FE)
    printk("********* Framing Error  **********\n");
  if (uartStatus & BI)
    printk("********* Parity Error   **********\n");
  if (uartStatus & ERFIFO)
    printk("********* Error receive Fifo **********\n");

}
#else
inline void uartError(int uart)
{
  unsigned char uartStatus;
  
  uartStatus = uread(uart, LSR);
  uartStatus = uread(uart, RBR);
}
#endif

/* 
 * Uart initialization, it is hardcoded to 8 bit, no parity,
 * one stop bit, FIFO, things to be changed
 * are baud rate and nad hw flow control,
 * and longest rx fifo setting
 */
void
BSP_uart_init(int uart, int baud, int hwFlow)
{
  unsigned char tmp;
  
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  switch(baud)
    {
    case 50:
    case 75:
    case 110:
    case 134:
    case 300:
    case 600:
    case 1200:
    case 2400:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      break;
    default:
      assert(0);
      return;
    }
  
  /* Set DLAB bit to 1 */
  uwrite(uart, LCR, DLAB);
  
  /* Set baud rate */
  uwrite(uart, DLL,  (BSPBaseBaud/baud) & 0xff); 
  uwrite(uart, DLM,  ((BSPBaseBaud/baud) >> 8) & 0xff); 

  /* 8-bit, no parity , 1 stop */
  uwrite(uart, LCR, CHR_8_BITS);
  

  /* Set DTR, RTS and OUT2 high */
  uwrite(uart, MCR, DTR | RTS | OUT_2);

  /* Enable FIFO */
  uwrite(uart, FCR, FIFO_EN | XMIT_RESET | RCV_RESET | RECEIVE_FIFO_TRIGGER12); 

  /* Disable Interrupts */
  uwrite(uart, IER, 0);

  /* Read status to clear them */
  tmp = uread(uart, LSR);
  tmp = uread(uart, RBR);
  tmp = uread(uart, MSR);

  /* Remember state */
  uart_data[uart].hwFlow     = hwFlow;
  uart_data[uart].baud       = baud;
  return;
}

/* 
 * Set baud
 */
void
BSP_uart_set_baud(int uart, int baud)
{
  unsigned char mcr, ier;

  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  /* 
   * This function may be called whenever TERMIOS parameters
   * are changed, so we have to make sire that baud change is 
   * indeed required
   */

  if(baud == uart_data[uart].baud)
    {
      return;
    }

  mcr = uread(uart, MCR);
  ier = uread(uart, IER);

  BSP_uart_init(uart, baud, uart_data[uart].hwFlow);

  uwrite(uart, MCR, mcr);
  uwrite(uart, IER, ier);
  
  return;
}

/*
 * Enable/disable interrupts 
 */
void 
BSP_uart_intr_ctrl(int uart, int cmd)
{

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  switch(cmd)
    {
    case BSP_UART_INTR_CTRL_DISABLE:
      uwrite(uart, IER, INTERRUPT_DISABLE);
      break;
    case BSP_UART_INTR_CTRL_ENABLE:
      if(uart_data[uart].hwFlow)
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  TRANSMIT_ENABLE |
		  RECEIVER_LINE_ST_ENABLE |
		  MODEM_ENABLE
		 )
		);
	}
      else
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  TRANSMIT_ENABLE |
		  RECEIVER_LINE_ST_ENABLE
		 )
		);
	}
      break;
    case BSP_UART_INTR_CTRL_TERMIOS:
      if(uart_data[uart].hwFlow)
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  RECEIVER_LINE_ST_ENABLE |
		  MODEM_ENABLE
		 )
		);
	}
      else
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  RECEIVER_LINE_ST_ENABLE
		 )
		);
	}
      break;
    case BSP_UART_INTR_CTRL_GDB:
      uwrite(uart, IER, RECEIVE_ENABLE);
      break;
    default:
      assert(0);
      break;
    }
 
  return;
}

void
BSP_uart_throttle(int uart)
{
  unsigned int mcr;
  
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  if(!uart_data[uart].hwFlow)
    {
      /* Should not happen */
      assert(0);
      return;
    }
  mcr = uread (uart, MCR);
  /* RTS down */
  mcr &= ~RTS;
  uwrite(uart, MCR, mcr);

  return;
}

void
BSP_uart_unthrottle(int uart)
{
  unsigned int mcr;

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  if(!uart_data[uart].hwFlow)
    {
      /* Should not happen */
      assert(0);
      return;
    }
  mcr = uread (uart, MCR);
  /* RTS up */
  mcr |= RTS;
  uwrite(uart, MCR, mcr);

  return;
}

/*
 * Status function, -1 if error
 * detected, 0 if no received chars available,
 * 1 if received char available, 2 if break
 * is detected, it will eat break and error 
 * chars. It ignores overruns - we cannot do 
 * anything about - it execpt count statistics
 * and we are not counting it.
 */
int 
BSP_uart_polled_status(int uart)
{
  unsigned char val;

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  val = uread(uart, LSR);

  if(val & BI)
    {
      /* BREAK found, eat character */
      uread(uart, RBR);
      return BSP_UART_STATUS_BREAK;
    }

  if((val & (DR | OE | FE)) ==  1)
    {
      /* No error, character present */ 
      return BSP_UART_STATUS_CHAR;
    }

  if((val & (DR | OE | FE)) == 0)
    {
      /* Nothing */
      return BSP_UART_STATUS_NOCHAR;
    }

  /* 
   * Framing or parity error
   * eat character
   */
  uread(uart, RBR);
 
  return BSP_UART_STATUS_ERROR;
}


/*
 * Polled mode write function
 */
void 
BSP_uart_polled_write(int uart, int val)
{
  unsigned char val1;
  
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  for(;;)
    {
      if((val1=uread(uart, LSR)) & THRE)
	{
	  break;
	}
    }

  if(uart_data[uart].hwFlow)
    {
      for(;;)
	{
	  if(uread(uart, MSR) & CTS)
	    {
	      break;
	    }
	}
    }

  uwrite(uart, THR, val & 0xff);
      
  return;
}

void
BSP_output_char_via_serial(int val)
{
  BSP_uart_polled_write(BSPConsolePort, val);
  if (val == '\n') BSP_uart_polled_write(BSPConsolePort,'\r');
}

/* 
 * Polled mode read function
 */
int 
BSP_uart_polled_read(int uart)
{
  unsigned char val;

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  for(;;)
    {
      if(uread(uart, LSR) & DR)
	{
	  break;
	}
    }
  
  val = uread(uart, RBR);

  return (int)(val & 0xff);
}

unsigned 
BSP_poll_char_via_serial()
{
	return BSP_uart_polled_read(BSPConsolePort);
}


/* ================ Termios support  =================*/

static volatile int  termios_stopped_com1        = 0;
static volatile int  termios_tx_active_com1      = 0;
static void*	     termios_ttyp_com1           = NULL;
static char          termios_tx_hold_com1        = 0;
static volatile char termios_tx_hold_valid_com1  = 0;

static volatile int  termios_stopped_com2        = 0;
static volatile int  termios_tx_active_com2      = 0;
static void*	     termios_ttyp_com2           = NULL;
static char          termios_tx_hold_com2        = 0;
static volatile char termios_tx_hold_valid_com2  = 0;

/*
 * Set channel parameters 
 */
void
BSP_uart_termios_set(int uart, void *ttyp)
{
  unsigned char val;
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);
  
  if(uart == BSP_UART_COM1)
    {
      if(uart_data[uart].hwFlow)
	{
	  val = uread(uart, MSR);

	  termios_stopped_com1   = (val & CTS) ? 0 : 1;
	}
      else
	{
	  termios_stopped_com1 = 0;
	}
      termios_tx_active_com1      = 0;
      termios_ttyp_com1           = ttyp;
      termios_tx_hold_com1        = 0; 
      termios_tx_hold_valid_com1  = 0;
    }
  else
    {
      if(uart_data[uart].hwFlow)
	{
	  val = uread(uart, MSR);

	  termios_stopped_com2   = (val & CTS) ? 0 : 1;
	}
      else
	{
	  termios_stopped_com2 = 0;
	}
      termios_tx_active_com2      = 0;
      termios_ttyp_com2           = ttyp;
      termios_tx_hold_com2        = 0; 
      termios_tx_hold_valid_com2  = 0;
    }

  return;
}

int
BSP_uart_termios_write_com1(int minor, const char *buf, int len)
{
  assert(buf != NULL);

  if(len <= 0)
    {
      return 0;
    }

  /* If there TX buffer is busy - something is royally screwed up */
  assert((uread(BSP_UART_COM1, LSR) & THRE) != 0);

  if(termios_stopped_com1)
    {
      /* CTS low */
      termios_tx_hold_com1       = *buf;
      termios_tx_hold_valid_com1 = 1;
      return 0;
    }

  /* Write character */
  uwrite(BSP_UART_COM1, THR, *buf & 0xff);

  /* Enable interrupts if necessary */
  if(!termios_tx_active_com1 && uart_data[BSP_UART_COM1].hwFlow)
    {
      termios_tx_active_com1 = 1;
      uwrite(BSP_UART_COM1, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE |
	      MODEM_ENABLE
	     )
	    );
    }
  else if(!termios_tx_active_com1)
    {
      termios_tx_active_com1 = 1;
      uwrite(BSP_UART_COM1, IER, 
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE
	     )
	    );
    }

  return 0;
}

int
BSP_uart_termios_write_com2(int minor, const char *buf, int len)
{
  assert(buf != NULL);

  if(len <= 0)
    {
      return 0;
    }


  /* If there TX buffer is busy - something is royally screwed up */
  assert((uread(BSP_UART_COM2, LSR) & THRE) != 0);

  if(termios_stopped_com2)
    {
      /* CTS low */
      termios_tx_hold_com2       = *buf;
      termios_tx_hold_valid_com2 = 1;
      return 0;
    }

  /* Write character */

  uwrite(BSP_UART_COM2, THR, *buf & 0xff);

  /* Enable interrupts if necessary */
  if(!termios_tx_active_com2 && uart_data[BSP_UART_COM2].hwFlow)
    {
      termios_tx_active_com2 = 1;
      uwrite(BSP_UART_COM2, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE |
	      MODEM_ENABLE
	     )
	    );
    }
  else if(!termios_tx_active_com2)
    {
      termios_tx_active_com2 = 1;
      uwrite(BSP_UART_COM2, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE
	     )
	    );
    }

  return 0;
}


void
BSP_uart_termios_isr_com1(void)
{
  unsigned char buf[40];
  unsigned char val;
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
      vect = uread(BSP_UART_COM1, IIR) & 0xf;
      
      switch(vect)
	{
	case MODEM_STATUS :
	  val = uread(BSP_UART_COM1, MSR);
	  if(uart_data[BSP_UART_COM1].hwFlow)
	    {
	      if(val & CTS)
		{
		  /* CTS high */
		  termios_stopped_com1 = 0;
		  if(termios_tx_hold_valid_com1)
		    {
		      termios_tx_hold_valid_com1 = 0;
		      BSP_uart_termios_write_com1(0, &termios_tx_hold_com1,
						    1);
		    }
		}
	      else
		{
		  /* CTS low */
		  termios_stopped_com1 = 1;
		}
	    }
	  break;
	case NO_MORE_INTR :
	  /* No more interrupts */
	  if(off != 0)
	    {
	      /* Update rx buffer */
	      rtems_termios_enqueue_raw_characters(termios_ttyp_com1,
						   (char *)buf,
						   off);
	    }
	  return;
	case TRANSMITTER_HODING_REGISTER_EMPTY :
	  /* 
	   * TX holding empty: we have to disable these interrupts 
	   * if there is nothing more to send. 
	   */

	  ret = rtems_termios_dequeue_characters(termios_ttyp_com1, 1);

	  /* If nothing else to send disable interrupts */
	  if(ret == 0 && uart_data[BSP_UART_COM1].hwFlow)
	    {
	      uwrite(BSP_UART_COM1, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE |
		      MODEM_ENABLE
		     )
		    );
              termios_tx_active_com1 = 0;
	    }
	  else if(ret == 0)
	    {
	      uwrite(BSP_UART_COM1, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE
		     )
		    );
              termios_tx_active_com1 = 0;
	    }
	  break;
	case RECEIVER_DATA_AVAIL :
	case CHARACTER_TIMEOUT_INDICATION:
	  /* RX data ready */
	  assert(off < sizeof(buf));
	  buf[off++] = uread(BSP_UART_COM1, RBR);
	  break;
	case RECEIVER_ERROR:
	  /* RX error: eat character */
	   uartError(BSP_UART_COM1);
	  break;
	default:
	  /* Should not happen */
	  assert(0);
	  return;
	}
    }
}
	  
void
BSP_uart_termios_isr_com2()
{
  unsigned char buf[40];
  unsigned char val;
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
      vect = uread(BSP_UART_COM2, IIR) & 0xf;
      
      switch(vect)
	{
	case MODEM_STATUS :
	  val = uread(BSP_UART_COM2, MSR);
	  if(uart_data[BSP_UART_COM2].hwFlow)
	    {
	      if(val & CTS)
		{
		  /* CTS high */
		  termios_stopped_com2 = 0;
		  if(termios_tx_hold_valid_com2)
		    {
		      termios_tx_hold_valid_com2 = 0;
		      BSP_uart_termios_write_com2(0, &termios_tx_hold_com2,
						    1);
		    }
		}
	      else
		{
		  /* CTS low */
		  termios_stopped_com2 = 1;
		}
	    }
	  break;
	case NO_MORE_INTR :
	  /* No more interrupts */
	  if(off != 0)
	    {
	      /* Update rx buffer */
	      rtems_termios_enqueue_raw_characters(termios_ttyp_com2,
						   (char *)buf,
						   off);
	    }
	  return;
	case TRANSMITTER_HODING_REGISTER_EMPTY :
	  /* 
	   * TX holding empty: we have to disable these interrupts 
	   * if there is nothing more to send.
	   */

	  ret = rtems_termios_dequeue_characters(termios_ttyp_com2, 1);

	  /* If nothing else to send disable interrupts */
	  if(ret == 0 && uart_data[BSP_UART_COM2].hwFlow)
	    {
	      uwrite(BSP_UART_COM2, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE |
		      MODEM_ENABLE
		     )
		    );
              termios_tx_active_com2 = 0;
	    }
	  else if(ret == 0)
	    {
	      uwrite(BSP_UART_COM2, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE
		     )
		    );
              termios_tx_active_com2 = 0;
	    }
	  break;
	case RECEIVER_DATA_AVAIL :
	case CHARACTER_TIMEOUT_INDICATION:
	  /* RX data ready */
	  assert(off < sizeof(buf));
	  buf[off++] = uread(BSP_UART_COM2, RBR);
	  break;
	case RECEIVER_ERROR:
	  /* RX error: eat character */
	   uartError(BSP_UART_COM2);
	  break;
	default:
	  /* Should not happen */
	  assert(0);
	  return;
	}
    }
}
	  
  
/* ================= GDB support     ===================*/ 
static int sav[4];

/*
 * Interrupt service routine for COM1 - all, 
 * it does it check whether ^C is received
 * if yes it will flip TF bit before returning
 * Note: it should be installed as raw interrupt
 * handler
 */

asm (".p2align 4");
asm (".text");
asm (".globl BSP_uart_dbgisr_com1");
asm ("BSP_uart_dbgisr_com1:");
asm ("    movl %eax, sav");          /* Save eax */
asm ("    movl %ebx, sav + 4");      /* Save ebx */
asm ("    movl %edx, sav + 8");      /* Save edx */

asm ("    movl $0, %ebx");           /* Clear flag */

/* 
 * We know that only receive related interrupts
 * are available, eat chars
 */
asm ("uart_dbgisr_com1_1:");
asm ("    movw $0x3FD, %dx");
asm ("    inb  %dx, %al"); /* Read LSR */
asm ("    andb $1, %al");
asm ("    cmpb $0, %al");
asm ("    je   uart_dbgisr_com1_2");
asm ("    movw $0x3F8, %dx");
asm ("    inb  %dx, %al");    /* Get input character */
asm ("    cmpb $3, %al");
asm ("    jne  uart_dbgisr_com1_1");

/* ^C received, set flag */ 
asm ("    movl $1, %ebx");
asm ("    jmp uart_dbgisr_com1_1");

/* All chars read */
asm ("uart_dbgisr_com1_2:");

/* If flag is set we have to tweak TF */
asm ("   cmpl $0, %ebx");
asm ("   je   uart_dbgisr_com1_3");

/* Flag is set */
asm ("   movl sav+4, %ebx");     /* Restore ebx */
asm ("   movl sav+8, %edx");     /* Restore edx */

/* Set TF bit */
asm ("   popl  %eax");           /* Pop eip */
asm ("   movl  %eax, sav + 4");  /* Save it */
asm ("   popl  %eax");           /* Pop cs */
asm ("   movl  %eax, sav + 8");  /* Save it */
asm ("   popl  %eax");           /* Pop flags */
asm ("   orl   $0x100, %eax");   /* Modify it */
asm ("   pushl %eax");           /* Push it back */
asm ("   movl  sav+8, %eax");    /* Put back cs */
asm ("   pushl %eax");
asm ("   movl  sav+4, %eax");    /* Put back eip */
asm ("   pushl %eax");

/* Acknowledge IRQ */
asm ("   movb  $0x20, %al");
asm ("   outb  %al, $0x20");
asm ("   movl  sav, %eax");      /* Restore eax */
asm ("   iret");                 /* Done */

/* Flag is not set */
asm("uart_dbgisr_com1_3:");
asm ("   movl sav+4, %ebx");     /* Restore ebx */
asm ("   movl sav+8, %edx");     /* Restore edx */

/* Acknowledge irq */
asm ("   movb  $0x20, %al");
asm ("   outb  %al, $0x20");
asm ("   movl  sav, %eax");      /* Restore eax */
asm ("   iret");                 /* Done */


/*
 * Interrupt service routine for COM2 - all, 
 * it does it check whether ^C is received
 * if yes it will flip TF bit before returning
 * Note: it has to be installed as raw interrupt 
 * handler
 */
asm (".p2align 4");
asm (".text");
asm (".globl BSP_uart_dbgisr_com2");
asm ("BSP_uart_dbgisr_com2:");
asm ("    movl %eax, sav");          /* Save eax */
asm ("    movl %ebx, sav + 4");      /* Save ebx */
asm ("    movl %edx, sav + 8");      /* Save edx */

asm ("    movl $0, %ebx");           /* Clear flag */

/* 
 * We know that only receive related interrupts
 * are available, eat chars
 */
asm ("uart_dbgisr_com2_1:");
asm ("    movw $0x2FD, %dx");
asm ("    inb  %dx, %al"); /* Read LSR */
asm ("    andb $1, %al");
asm ("    cmpb $0, %al");
asm ("    je   uart_dbgisr_com2_2");
asm ("    movw $0x2F8, %dx");
asm ("    inb  %dx, %al");    /* Get input character */
asm ("    cmpb $3, %al");
asm ("    jne  uart_dbgisr_com2_1");

/* ^C received, set flag */ 
asm ("    movl $1, %ebx");
asm ("    jmp uart_dbgisr_com2_1");

/* All chars read */
asm ("uart_dbgisr_com2_2:");

/* If flag is set we have to tweak TF */
asm ("   cmpl $0, %ebx");
asm ("   je   uart_dbgisr_com2_3");

/* Flag is set */
asm ("   movl sav+4, %ebx");     /* Restore ebx */
asm ("   movl sav+8, %edx");     /* Restore edx */

/* Set TF bit */
asm ("   popl  %eax");           /* Pop eip */
asm ("   movl  %eax, sav + 4");  /* Save it */
asm ("   popl  %eax");           /* Pop cs */
asm ("   movl  %eax, sav + 8");  /* Save it */
asm ("   popl  %eax");           /* Pop flags */
asm ("   orl   $0x100, %eax");   /* Modify it */
asm ("   pushl %eax");           /* Push it back */
asm ("   movl  sav+8, %eax");    /* Put back cs */
asm ("   pushl %eax");
asm ("   movl  sav+4, %eax");    /* Put back eip */
asm ("   pushl %eax");

/* Acknowledge IRQ */
asm ("   movb  $0x20, %al");
asm ("   outb  %al, $0x20");
asm ("   movl  sav, %eax");      /* Restore eax */
asm ("   iret");                 /* Done */

/* Flag is not set */
asm("uart_dbgisr_com2_3:");
asm ("   movl sav+4, %ebx");     /* Restore ebx */
asm ("   movl sav+8, %edx");     /* Restore edx */

/* Acknowledge irq */
asm ("   movb  $0x20, %al");
asm ("   outb  %al, $0x20");
asm ("   movl  sav, %eax");      /* Restore eax */
asm ("   iret");                 /* Done */






