/*
 *  SMC1 raw console serial I/O.
 *
 *  This driver is an example of `POLLING' or `INTERRUPT' I/O.
 *
 *  To run with interrupt-driven I/O, ensure m360_smc1_interrupt
 *  is set before calling the initialization routine.
 *
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <termios.h>
#include <bsp.h>
#include <rtems/libio.h>
#include "m68360.h"

/*
 * Declare clock speed -- may be overwritten by downloader or debugger
 */
int m360_clock_rate	= 25000000;

/*
 * Interrupt-driven input buffer
 */
#define RXBUFSIZE	16

/*
 * Interrupt-driven callback
 */
static int m360_smc1_interrupt = 1;
static void *smc1ttyp;

/*
 * I/O buffers and pointers to buffer descriptors
 */
static volatile char rxBuf[RXBUFSIZE];
static volatile m360BufferDescriptor_t *smcRxBd, *smcTxBd;

/*
 * Device-specific routines
 */

/*
 * Compute baud-rate-generator configuration register value
 */
static int
smc1BRGC (int baud)
{
	int divisor;
	int div16 = 0;

	divisor = ((m360_clock_rate / 16) + (baud / 2)) / baud;
	if (divisor > 4096) {
		div16 = 1;
		divisor = (divisor + 8) / 16;
	}
	return M360_BRG_EN | M360_BRG_EXTC_BRGCLK | ((divisor - 1) << 1) | div16;
}

/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
smc1SetAttributes (int minor, const struct termios *t)
{
	int baud;

	switch (t->c_cflag & CBAUD) {
	default:	baud = -1;	break;
	case B50:	baud = 50;	break;
	case B75:	baud = 75;	break;
	case B110:	baud = 110;	break;
	case B134:	baud = 134;	break;
	case B150:	baud = 150;	break;
	case B200:	baud = 200;	break;
	case B300:	baud = 300;	break;
	case B600:	baud = 600;	break;
	case B1200:	baud = 1200;	break;
	case B1800:	baud = 1800;	break;
	case B2400:	baud = 2400;	break;
	case B4800:	baud = 4800;	break;
	case B9600:	baud = 9600;	break;
	case B19200:	baud = 19200;	break;
	case B38400:	baud = 38400;	break;
	case B57600:	baud = 57600;	break;
	case B115200:	baud = 115200;	break;
	case B230400:	baud = 230400;	break;
	case B460800:	baud = 460800;	break;
	}
	if (baud > 0)
		m360.brgc1 = smc1BRGC (baud);
	return 0;
}

/*
 * Interrupt handler
 */
static rtems_isr
smc1InterruptHandler (rtems_vector_number v)
{
	/*
	 * Buffer received?
	 */
	if (m360.smc1.smce & 0x1) {
		m360.smc1.smce = 0x1;
		while ((smcRxBd->status & M360_BD_EMPTY) == 0) {
			rtems_termios_enqueue_raw_characters (smc1ttyp,
							(char *)smcRxBd->buffer,
							smcRxBd->length);
			smcRxBd->status = M360_BD_EMPTY | M360_BD_WRAP | M360_BD_INTERRUPT;
		}
	}

	/*
	 * Buffer transmitted?
	 */
	if (m360.smc1.smce & 0x2) {
		m360.smc1.smce = 0x2;
		if ((smcTxBd->status & M360_BD_READY) == 0)
			rtems_termios_dequeue_characters (smc1ttyp, smcTxBd->length);
	}
	m360.cisr = 1UL << 4;	/* Clear SMC1 interrupt-in-service bit */
}

static void
smc1Initialize (void)
{
	/*
	 * Allocate buffer descriptors
	 */
	smcRxBd = M360AllocateBufferDescriptors (1);
	smcTxBd = M360AllocateBufferDescriptors (1);

	/*
	 * Configure port B pins to enable SMTXD1 and SMRXD1 pins
	 */
	m360.pbpar |=  0xC0;
	m360.pbdir &= ~0xC0;
	m360.pbodr &= ~0xC0;

	/*
	 * Set up BRG1 (9,600 baud)
	 */
	m360.brgc1 = M360_BRG_RST;
	m360.brgc1 = smc1BRGC (9600);

	/*
	 * Put SMC1 in NMSI mode, connect SMC1 to BRG1
	 */
	m360.simode |= M360_SI_SMC1_BRG1;
	 
	/*
	 * Set up SMC1 parameter RAM common to all protocols
	 */
	m360.smc1p.rbase = (char *)smcRxBd - (char *)&m360;
	m360.smc1p.tbase = (char *)smcTxBd - (char *)&m360;
	m360.smc1p.rfcr = M360_RFCR_MOT | M360_RFCR_DMA_SPACE;
	m360.smc1p.tfcr = M360_TFCR_MOT | M360_TFCR_DMA_SPACE;
	if (m360_smc1_interrupt)
		m360.smc1p.mrblr = RXBUFSIZE;
	else
		m360.smc1p.mrblr = 1;
	 
	/*
	 * Set up SMC1 parameter RAM UART-specific parameters
	 */
	m360.smc1p.un.uart.max_idl = 10;
	m360.smc1p.un.uart.brklen = 0;
	m360.smc1p.un.uart.brkec = 0;
	m360.smc1p.un.uart.brkcr = 0;
	 
	/*
	 * Set up the Receive Buffer Descriptor
	 */
	smcRxBd->status = M360_BD_EMPTY | M360_BD_WRAP | M360_BD_INTERRUPT;
	smcRxBd->length = 0;
	smcRxBd->buffer = rxBuf;
	 
	/*
	 * Setup the Transmit Buffer Descriptor
	 */
	smcTxBd->status = M360_BD_WRAP;
	 
	/*
	 * Set up SMC1 general and protocol-specific mode registers
	 */
	m360.smc1.smce = ~0;	/* Clear any pending events */
	m360.smc1.smcm = 0;	/* Mask all interrupt/event sources */
	m360.smc1.smcmr = M360_SMCMR_CLEN(9) | M360_SMCMR_SM_UART;

	/*
	 * Send "Init parameters" command
	 */
	M360ExecuteRISC (M360_CR_OP_INIT_RX_TX | M360_CR_CHAN_SMC1);

	/*
	 * Enable receiver and transmitter
	 */
	m360.smc1.smcmr |= M360_SMCMR_TEN | M360_SMCMR_REN;

	if (m360_smc1_interrupt) {
	rtems_isr_entry old_handler;
	rtems_status_code sc;

	sc = rtems_interrupt_catch (smc1InterruptHandler,
						(m360.cicr & 0xE0) | 0x04,
						&old_handler);
	m360.smc1.smcm = 3;	/* Enable SMC1 TX and RX interrupts */
	m360.cimr |= 1UL << 4;	/* Enable SMC1 interrupts */
	}
}

static int
smc1PollRead (int minor)
{
	unsigned char c;

	if (smcRxBd->status & M360_BD_EMPTY)
		return -1;
	c = rxBuf[0];
	smcRxBd->status = M360_BD_EMPTY | M360_BD_WRAP;
	return c;
}

/*
 * Device-dependent write routine
 * Interrupt-driven devices:
 *	Begin transmission of as many characters as possible (minimum is 1).
 * Polling devices:
 *	Transmit all characters.
 */
static int
smc1InterruptWrite (int minor, const char *buf, int len)
{
	smcTxBd->buffer = (char *)buf;
	smcTxBd->length = len;
	smcTxBd->status = M360_BD_READY | M360_BD_WRAP | M360_BD_INTERRUPT;
	return 0;
}

static int
smc1PollWrite (int minor, const char *buf, int len)
{
	while (len--) {
		static char txBuf;
		while (smcTxBd->status & M360_BD_READY)
			continue;
		txBuf = *buf++;
		smcTxBd->buffer = &txBuf;
		smcTxBd->length = 1;
		smcTxBd->status = M360_BD_READY | M360_BD_WRAP;
	}
	return 0;
}

/*
 ***************
 * BOILERPLATE *
 ***************
 */

/*
 * Reserve resources consumed by this driver
 */
void console_reserve_resources(
  rtems_configuration_table *configuration
)
{
	rtems_termios_reserve_resources (configuration, 1);
}

/*
 * Initialize and register the device
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
	rtems_status_code status;

	/*
	 * Set up TERMIOS
	 */
	rtems_termios_initialize ();

	/*
	 * Do device-specific initialization
	 */
	smc1Initialize ();

	/*
	 * Register the device
	 */
	status = rtems_io_register_name ("/dev/console", major, 0);
	if (status != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred (status);
	return RTEMS_SUCCESSFUL;
}

/*
 * Open the device
 */
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	rtems_status_code sc;
	static const rtems_termios_callbacks intrCallbacks = {
		NULL,			/* firstOpen */
		NULL,			/* lastClose */
		NULL,			/* pollRead */
		smc1InterruptWrite,	/* write */
		smc1SetAttributes,	/* setAttributes */
		NULL,			/* stopRemoteTx */
		NULL,			/* startRemoteTx */
		1			/* outputUsesInterrupts */
	};
	static const rtems_termios_callbacks pollCallbacks = {
		NULL,			/* firstOpen */
		NULL,			/* lastClose */
		smc1PollRead,		/* pollRead */
		smc1PollWrite,		/* write */
		smc1SetAttributes,	/* setAttributes */
		NULL,			/* stopRemoteTx */
		NULL,			/* startRemoteTx */
		0			/* outputUsesInterrupts */
	};

	if (m360_smc1_interrupt) {
		rtems_libio_open_close_args_t *args = arg;

		sc = rtems_termios_open (major, minor, arg, &intrCallbacks);
		smc1ttyp = args->iop->data1;
	}
	else {
		sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
	}
	return sc;
}
 
/*
 * Close the device
 */
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_close (arg);
}

/*
 * Read from the device
 */
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_read (arg);
}

/*
 * Write to the device
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
	return rtems_termios_write (arg);
}

/*
 * Handle ioctl request.
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{ 
	return rtems_termios_ioctl (arg);
}
