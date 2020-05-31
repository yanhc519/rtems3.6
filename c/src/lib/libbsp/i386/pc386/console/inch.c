/*-------------------------------------------------------------------------+
| inch.c v1.1 - PC386 BSP - 1997/08/07
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   inch.c,v 1.3 1995/12/19 20:07:25 joel Exp - go32 BSP
| With the following copyright notice:
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1998.
| *  On-Line Applications Research Corporation (OAR).
| *  Copyright assigned to U.S. Government, 1994. 
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.OARcorp.com/rtems/license.html.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <irq.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define KBD_CTL      0x61  /* -------------------------------- */
#define KBD_DATA     0x60  /* Ports for PC keyboard controller */
#define KBD_STATUS   0x64  /* -------------------------------- */

#define KBD_BUF_SIZE 256

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
static char key_map[] =
{
  0,033,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
  'q','w','e','r','t','y','u','i','o','p','[',']',015,0x80,
  'a','s','d','f','g','h','j','k','l',';',047,0140,0x80,
  0134,'z','x','c','v','b','n','m',',','.','/',0x80,
  '*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,'0',0177
}; /* Keyboard scancode -> character map with no modifiers.       */

static char shift_map[] =
{
  0,033,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
  'Q','W','E','R','T','Y','U','I','O','P','{','}',015,0x80,
  'A','S','D','F','G','H','J','K','L',':',042,'~',0x80,
  '|','Z','X','C','V','B','N','M','<','>','?',0x80,
  '*',0x80,' ',0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0x80,0x80,'7','8','9',0x80,'4','5','6',0x80,
  '1','2','3','0',177
}; /* Keyboard scancode -> character map with SHIFT key modifier. */ 

static char             kbd_buffer[KBD_BUF_SIZE];
static rtems_unsigned16 kbd_first = 0;
static rtems_unsigned16 kbd_last  = 0;
static rtems_unsigned16 kbd_end   = KBD_BUF_SIZE - 1;

/*-------------------------------------------------------------------------+
|         Function: rtemsReboot
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void rtemsReboot(void)
{
  /* shutdown and reboot */
  outport_byte(0x64, 0xFE);      /* use keyboard controler to do the job... */
} /* rtemsReboot */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_scankey
|      Description: This function can be called during a poll for input, or by
|                   an ISR. Basically any time you want to process a keypress.
| Global Variables: key_map, shift_map.
|        Arguments: outChar - character read in case of a valid reading,
|                   otherwise unchanged.
|          Returns: TRUE in case a valid character has been read,
|                   FALSE otherwise.
+--------------------------------------------------------------------------*/
rtems_boolean
_IBMPC_scankey(char *outChar)
{
  unsigned char inChar;
  static int alt_pressed   = 0;
  static int ctrl_pressed  = 0;
  static int shift_pressed = 0;
  static int caps_pressed  = 0;
  static int extended      = 0;

  *outChar = NULL; /* default value if we return FALSE */

  /* Read keyboard controller, toggle enable */
  inport_byte(KBD_CTL, inChar);
  outport_byte(KBD_CTL, inChar & ~0x80);
  outport_byte(KBD_CTL, inChar | 0x80);
  outport_byte(KBD_CTL, inChar & ~0x80);

  /* See if it has data */
  inport_byte(KBD_STATUS, inChar);
  if ((inChar & 0x01) == 0)
    return FALSE;

  /* Read the data.  Handle nonsense with shift, control, etc. */
  inport_byte(KBD_DATA, inChar);

  if (extended)
    extended--;

  switch (inChar)
  {
    case 0xe0:
      extended = 2;
      return FALSE;
      break;

    case 0x38:
      alt_pressed = 1;
      return FALSE;
      break;
    case 0xb8:
      alt_pressed = 0;
      return FALSE;
      break;

    case 0x1d:
      ctrl_pressed = 1;
      return FALSE;
      break;
    case 0x9d:
      ctrl_pressed = 0;
      return FALSE;
      break;

    case 0x2a:
      if (extended)
        return FALSE;
    case 0x36:
      shift_pressed = 1;
      return FALSE;
      break;
    case 0xaa:
      if (extended)
        return FALSE;
    case 0xb6:
      shift_pressed = 0;
      return FALSE;
      break;

    case 0x3a:
      caps_pressed = 1;
      return FALSE;
      break;
    case 0xba:
      caps_pressed = 0;
      return FALSE;
      break;

    case 0x53:
      if (ctrl_pressed && alt_pressed) 
        rtemsReboot(); /* ctrl+alt+del -> reboot */
      break;

    /*
     * Ignore unrecognized keys--usually arrow and such
     */
    default:
      if ((inChar & 0x80) || (inChar > 0x39))
      /* High-bit on means key is being released, not pressed */
        return FALSE;
      break;
  } /* switch */

  /* Strip high bit, look up in our map */
  inChar &= 0x7f;
  if (ctrl_pressed)
  {
    *outChar = key_map[inChar];
    *outChar &= 037;
  }
  else
  {
    *outChar = shift_pressed ? shift_map[inChar] : key_map[inChar];
    if (caps_pressed)
    {
      if (*outChar >= 'A' && *outChar <= 'Z')
        *outChar += 'a' - 'A';
      else if (*outChar >= 'a' && *outChar <= 'z')
        *outChar -= 'a' - 'A';
    }
  }

  return TRUE;
} /* _IBMPC_scankey */

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_keyboard_isr
|      Description: Interrupt Service Routine for keyboard (0x01) IRQ.
| Global Variables: kbd_buffer, kbd_first, kbd_last.
|        Arguments: vector - standard RTEMS argument - see documentation.
|          Returns: standard return value - see documentation.
+--------------------------------------------------------------------------*/
void _IBMPC_keyboard_isr()
{
  if (_IBMPC_scankey(&kbd_buffer[kbd_last]))
  {
    /* Got one; save it if there is enough room in buffer. */
    unsigned int next = (kbd_last == kbd_end) ? 0 : kbd_last + 1;

    if (next != kbd_first)
      {
	kbd_last = next;
      }
  }
} /* _IBMPC_keyboard_isr */


/*-------------------------------------------------------------------------+
|         Function: _IBMPC_chrdy
|      Description: Check keyboard ISR buffer and return character if not empty.
| Global Variables: kbd_buffer, kbd_first, kbd_last.
|        Arguments: c - character read if keyboard buffer not empty, otherwise
|                   unchanged.
|          Returns: TRUE if keyboard buffer not empty, FALSE otherwise.
+--------------------------------------------------------------------------*/
rtems_boolean
_IBMPC_chrdy(char *c)
{
  /* FIX ME!!! It doesn't work without something like the following line.
     Find out why! */
  printk("");

  /* Check buffer our ISR builds */
  if (kbd_first != kbd_last)
  {
    *c = kbd_buffer[kbd_first];

    kbd_first = (kbd_first + 1) % KBD_BUF_SIZE;
    return TRUE;
  }
  else
    return FALSE;
} /* _IBMPC_chrdy */


/*-------------------------------------------------------------------------+
|         Function: _IBMPC_inch
|      Description: Poll keyboard until a character is ready and return it.
| Global Variables: None.
|        Arguments: None.
|          Returns: character read from keyboard.
+--------------------------------------------------------------------------*/
char
_IBMPC_inch(void)
{
    char c;
    while (!_IBMPC_chrdy(&c))
      continue;

    return c;
} /* _IBMPC_inch */

 
 /*
  * Routine that can be used before interrupt management is initialized.
  */
 
char
BSP_wait_polled_input(void)
{
  char c;
  while (!_IBMPC_scankey(&c))
    continue;

  return c;
}

/*-------------------------------------------------------------------------+
|         Function: _IBMPC_inch_sleep
|      Description: If charcter is ready return it, otherwise sleep until 
|                   it is ready
| Global Variables: None.
|        Arguments: None.
|          Returns: character read from keyboard.
+--------------------------------------------------------------------------*/
char
_IBMPC_inch_sleep(void)
{
    char           c;
    rtems_interval ticks_per_second;

    ticks_per_second = 0;

    for(;;)
      {
	if(_IBMPC_chrdy(&c))
	  {
	    return c;
	  }
 
        if(ticks_per_second == 0)
          {
            rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, 
                            &ticks_per_second);
          }
	rtems_task_wake_after((ticks_per_second+24)/25);
      }
	
    return c;
} /* _IBMPC_inch */






