//-----------------------------------------------------------------------------
//  File          : usbkbd.c
//  Module        :
//  Description   : Main Entry.
//  Author        : Kenneth
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  9 Sep 2011    Kenneth     Initial Version.
//  20 Sep 2011   Kenneth     Some bugs fixed
//-----------------------------------------------------------------------------
#include <stdio.h>
#include "midware.h"
#include "sysutil.h"
#include "usbkbd.h"
#include "Sysutil.h"
#include "apm.h"
#include "hardware.h"

//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------
BOOLEAN checkExtKeyboard(int *idPort);
//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define DEVNAME  "USB Kbd"
#define HEADING DEVNAME" test"
#define WAIT_DEV_REMOVE "Press cancel to exit or remove usb"

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
static unsigned char num_lock = 0;
static unsigned char caps_lock = 0;
static unsigned char scroll_lock = 0;
static int enterline[8];
int line, col;
static unsigned char keybuf[128];
static unsigned char keylen=0;
static unsigned char ctrl_active=0;

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
static unsigned char usb_kbd_numkey[] = {
//  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\n', 0x1b, '\b', '\t', ' ', '-',
//  '=', '[', ']', '\\', '\\', ';', '\'', '`', ',', '.', '/'
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\n', 0x1b, '\b', '\t', ' ', '\'',
    0xa1, '`', '+', '\\', 0xe7, 0xf1, 0xb4, 0xba, ',', '.', '-'
};
static unsigned char usb_kbd_numkey_shifted[] = {
//  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '\r', 0x1b, '\b', '\t', ' ', '_',
//  '+', '{', '}', '|', '|', ':', '"', '~', '<', '>', '?'
    '!', '"', 0xb7, '$', '%', '&', '/', '(', ')', '=', '\r', 0x1b, '\b', '\t', ' ', '?',
    0xbf, '^', '*', '|', 0xc7, 0xd1, 0xa8, 0xaa, ';', ':', '_'
};

static unsigned char keypad_locked[]={
  '/','*','-','+','\r','1','2','3','4','5','6','7','8','9','0','.',
};

/* Modifier bits */
#define LEFT_CNTR     0
#define LEFT_SHIFT    1
#define LEFT_ALT      2
#define LEFT_GUI      3
#define RIGHT_CNTR    4
#define RIGHT_SHIFT   5
#define RIGHT_ALT     6
#define RIGHT_GUI     7
#define NUM_LOCK      0x53
#define CAPS_LOCK     0x39
#define SCROLL_LOCK   0x47

#define CAPITAL_MASK  0x20

/* Special control chars */
#define CTRL_DIERESIS 1
#define CTRL_TILDE    2
#define CTRL_DOWN     3
#define CTRL_ARRWUP   4

#define MAX_CHAR_PER_LINE     20
#define MAX_LINE              4


//*****************************************************************************
//  Function        : AttachDevice
//  Description     : Wait for USB keyboard attaching
//  Input           : N/A
//  Return          : TRUE / FALSE
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN AttachDevice()
{
  DispLineMW("Pls connect USB kbd", MW_LINE4, MW_LEFT|MW_SMFONT);
  while (os_kbd_getkey() != K_KeyCancel)
  {
    if (os_usbhost_devicemap() & 0xfffffffe)
    {
      DispClrBelowMW(MW_LINE2);
      DispLineMW("USB kbd plugged!", MW_LINE4, MW_LEFT|MW_SMFONT);
      DispLineMW("Begin input", MW_LINE5, MW_LEFT|MW_SMFONT);
      memset(enterline, 0, sizeof(enterline));
      line = 2;
      col = 1;
      return TRUE;
    }
  }
  return FALSE;
}

int SpacesAtEnd(char *pChar, int aLen)
{
  int lines=0;
  int line_char=0;
  int spaces;
  
  while (aLen--) {
    if ((*pChar == '\n') || (*pChar == '\r')) {
      line_char = 0;
      lines++;
    }
    else {
      if (line_char >= MAX_CHAR_PER_LINE) {
        line_char = 0;
        lines++;
      }
      else
        line_char++;
    }
    pChar++;
  }
  spaces = (MAX_LINE - lines - 1)*MAX_CHAR_PER_LINE + (MAX_CHAR_PER_LINE - line_char - 1);
  return spaces;
}

void Change_EspChar(char *pChar, char aCtrl)
{
  switch(*pChar) {
    case 'a':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xe4;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xe1;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xe0;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xe2;
      break;
    case 'e':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xeb;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xe9;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xe8;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xea;
      break;
    case 'i':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xef;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xed;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xec;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xee;
      break;
    case 'o':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xf6;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xf3;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xf2;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xf4;
      break;
    case 'u':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xfc;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xfa;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xf9;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xfb;
      break;
    case 'A':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xC4;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xC1;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xC0;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xC2;
      break;
    case 'E':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xcb;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xc9;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xc8;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xca;
      break;
    case 'I':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xcf;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xcd;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xcc;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xce;
      break;
    case 'O':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xd6;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xd3;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xd2;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xd4;
      break;
    case 'U':
      if (aCtrl == CTRL_DIERESIS)
        *pChar = 0xdc;
      else
      if (aCtrl == CTRL_TILDE)
        *pChar = 0xda;
      else
      if (aCtrl == CTRL_DOWN)
        *pChar = 0xd9;
      else
      if (aCtrl == CTRL_ARRWUP)
        *pChar = 0xdb;
      break;
  }
}

void Handle_key(unsigned char scancode,unsigned char modifier)
{
  unsigned char keycode;
  int spaces;

  keycode=0;
  if((scancode>3) && (scancode<=0x1d))    /* alpha numeric values */
  {
    keycode=scancode-4 + 0x61;
    if(caps_lock)
      keycode&=~CAPITAL_MASK;        /* switch to capital Letters */
    if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))
    {
      if(keycode & CAPITAL_MASK)
        keycode&=~CAPITAL_MASK;      /* switch to capital Letters */
      else
        keycode|=CAPITAL_MASK;      /* switch to non capital Letters */
    }
    if (ctrl_active)
      Change_EspChar(&keycode, ctrl_active);
    ctrl_active = 0;
  }
  if((scancode>0x1d) && (scancode<0x39))    /* usb_kbd_numkey */
  {
    if ((scancode != 0x2F) && (scancode != 0x34))
    {
      // non-special control char input before, or non A,E,I,O,U
      if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))  /* shifted */
        keycode=usb_kbd_numkey_shifted[scancode-0x1e];
      else /* non shifted */
        keycode=usb_kbd_numkey[scancode-0x1e];
      ctrl_active = 0;
    }
    else
    {
      // special control char in Spanish kbd (0x2F or 0x34)
      if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))  /* shifted */
      {
        // shifted
        if (scancode==0x34)
          ctrl_active = CTRL_DIERESIS;
        else
          ctrl_active = CTRL_ARRWUP;
      }
      else
      {
        // not shifted
        if (scancode==0x34)
          ctrl_active = CTRL_TILDE;
        else
          ctrl_active = CTRL_DOWN;
      }
      return;
    }
  }
#define CONFIG_EXTENDED_KEYCODE  
#ifdef CONFIG_EXTENDED_KEYCODE
  if(((scancode >= 0x3a) && (scancode < 0x53))/*0x3a:F1,0x53:NumLock,0x63:keypad . and DEL*/
    ||(scancode > 0x63))//all the other keys,exclude the whole keypad(NumLock excluded also)
  {
    keycode = scancode + 0x80;
  }

  if((scancode <= 0x63)&&(scancode >= 0x54))//keypad exclude "NumLock"
  {
    if(!num_lock)
      ;//keycode = keypad[scancode-0x54];
    else
      keycode = keypad_locked[scancode-0x54];
  }
#endif

  if(scancode==NUM_LOCK) {
    num_lock=~num_lock;
    //return 1;
  }
  if(scancode==CAPS_LOCK) {
    caps_lock=~caps_lock;
    //return 1;
  }
  if(scancode==SCROLL_LOCK) {
    scroll_lock=~scroll_lock;
    //return 1;
  }

  // no escape, no tab, no num lock
  if(keycode != 0x1b && keycode != 0x09 && keycode > 0)
  {
    //printf("\f");
    printf("\x1B%c%c", K_Line1, 1);
    if (keycode == '\b')
    {
      if (keylen > 0) {
        keylen--;
        keybuf[keylen] = 0;
      }
      spaces = SpacesAtEnd(keybuf, keylen);
    }
    else
    {
      spaces = SpacesAtEnd(keybuf, keylen);
      if (spaces == 0) {
        memset(keybuf, 0, sizeof(keybuf));
        keylen = 0;
        spaces = MAX_CHAR_PER_LINE*MAX_LINE;
      }
      if ((spaces < MAX_CHAR_PER_LINE) && ((keycode == '\n') || (keycode == '\r'))) {
        memset(keybuf, 0, sizeof(keybuf));
        keylen = 0;
        spaces = MAX_CHAR_PER_LINE*MAX_LINE;
      }
      else {
        keybuf[keylen++] = keycode;
        spaces = SpacesAtEnd(keybuf, keylen);
      }
    }
    DispPutNCMW(keybuf, keylen);
    //printf("\flen=%d,space=%d", keylen, spaces); WaitKey();
    while (spaces--)
      printf(" ");
  }

//   os_disp_putc(K_PushCursor);
//   printf("\x1B\x01\x01\x1C%02x%02x %d %d\x5\x1D",modifier,scancode, line, col);
//   os_disp_putc(K_PopCursor);
}

//*****************************************************************************
//  Function        : Clear_LED
//  Description     : Clear all LED on the keyboard
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Clear_LED(void)
{
  num_lock = caps_lock = scroll_lock = 0;
}

//*****************************************************************************
//  Function        : CheckDevice
//  Description     : Wait for USB keyboard attaching
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CheckDevice(void)
{
  int ret;
  T_DevIterator dit;
  T_DeviceInfo dinfo;
  int filehandle, len;
  int i;

  os_device_search_init(&dit, ((BUS_ID_USB << 24)|DEV_KEYBOARD), 
    DI_FILTER_BUS|DI_FILTER_DEV);
  ret = os_device_search_next(&dit, &dinfo);
  if ((ret >= 0) && dinfo.dev_id)    // device found
  {
    AcceptBeep();
    Clear_LED();
    
    Delay10ms(100);      // wait for 3 seconds
    DispClrBelowMW(MW_LINE1);
    //DispCtrlMW(K_SelUser8x8);         /* JJJ */
    DispCtrlMW(K_SelUser8x16);        /* JJJ */
    memset(keybuf, 0, sizeof(keybuf));
    keylen = 0;
    
    filehandle = os_device_open(dinfo.dev_id, K_O_RDONLY);
    if (filehandle >= 0)
    {
      while(1)
      {
        if(os_kbd_getkey() == K_KeyCancel)
          break;
        
        len = os_file_read(filehandle, &i, sizeof(i));
        if(len < 0)
          break;
        else if(len == 0)      // nothing return from kbd
          continue;
        else            // character return from kbd
          Handle_key(i&0xff,(i>>8)&0xff);
      }

      os_file_close(filehandle); // close COM port  
      Clear_LED();
    }
    DispCtrlMW(MW_SMFONT);       /* JJJ */
  }
  else
  {
    LongBeep();
    printf("\x11\f\x1c%s\x5\x1d\x10","No Device Info!!!");
  }
}


//*****************************************************************************
//  Function        : UsbkbdTest
//  Description     : USB Keyboard Test
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void UsbkbdTest(void)
{
  os_disp_bl_control(0xFF);
  os_bus_start(BUS_ID_USB);
  if (AttachDevice())
  {
    Delay10ms(100);
    CheckDevice();
  }
  os_bus_stop(BUS_ID_USB);
}

////////////////////////////////FUNCIONES DANIEL PARA TECLADO EXTERNO//////////////////////////


BOOLEAN AttachDevice2()
{
  	int idTimer;

  	idTimer = os_timer_open();
  	os_timer_set(idTimer, 300);
	//DispPutStr("\x1B\x04\x01""Pls connect USB kbd");
  	showMessage(13, 1, "");
    while ((os_kbd_getkey() != K_KeyCancel) && (os_timer_get(idTimer)))
    {
        if (os_usbhost_devicemap() & 0xfffffffe)
        {
        	os_timer_close(idTimer);
        	return TRUE;
        }
    }
    os_timer_close(idTimer);
    return FALSE;
}


BOOLEAN openExtKeyboard(int *idPort){
	int id;
	os_disp_bl_control(0xFF);
	os_bus_start(BUS_ID_USB);
	if (AttachDevice2()){
		 Delay10ms(100);
		 if( checkExtKeyboard(&id) ){
			 *(idPort) = id;
			 return TRUE;
		 }
		 else
			 return FALSE;
	}
	else{
		showMessage(11, 3, "ERROR");
		return FALSE;
	}
}

BOOLEAN checkExtKeyboard(int *idPort)
{
	int ret;
	T_DevIterator dit;
	T_DeviceInfo dinfo;
	int filehandle;

	os_device_search_init(&dit, ((BUS_ID_USB << 24)|DEV_KEYBOARD),
			DI_FILTER_BUS|DI_FILTER_DEV);
	ret = os_device_search_next(&dit, &dinfo);
	if ((ret >= 0) && dinfo.dev_id)		// device found
	{
		AcceptBeep();
		Clear_LED();

		Delay10ms(100);			// wait for 3 seconds
		DispClrBelowMW(MW_LINE2);
		//printf("\x1B%c%c",K_Line2,1);
		filehandle = os_device_open(dinfo.dev_id, K_O_RDONLY);
		if (filehandle >= 0){
			*(idPort) = filehandle;
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		showMessage(12, 3, "ERROR");
	}
	return FALSE;
}


void closeExtKeyBoard(int idPort){
	os_file_close(idPort); // close COM port
	Clear_LED();
	os_bus_stop(BUS_ID_USB);
	Delay10ms(100);
}



BYTE getkeyPress(BYTE scancode, BYTE modifier, BYTE *titulo, BYTE *label, int *contChar, int *scroll,
		BYTE *buffer, BYTE colIni, BYTE colFin, BYTE min, BYTE max)
{
	unsigned char keycode;
	//BYTE i;
	BYTE lenField;
	BYTE fila = 5;
	BYTE linea[26];

	memset(linea, 0x00, sizeof(linea));
	memset(linea, ' ', sizeof(linea) - 2);
	//graphicWindow(titulo);
	//gotoxy(3, 2);printf("%s", label);

	lenField = (colFin-colIni) + 1;
	keycode=0;
	if((scancode>3) && (scancode<=0x1d))		/* alpha numeric values */
	{
		keycode=scancode-4 + 0x61;
		if(caps_lock)
			keycode&=~CAPITAL_MASK;				/* switch to capital Letters */
		if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))
		{
			if(keycode & CAPITAL_MASK)
				keycode&=~CAPITAL_MASK;			/* switch to capital Letters */
			else
				keycode|=CAPITAL_MASK;			/* switch to non capital Letters */
		}
//////////////KBD ESP//////////////////////////
	    if (ctrl_active)
	      Change_EspChar(&keycode, ctrl_active);
	    ctrl_active = 0;
//////////////////////////////////////////////
	}
	if((scancode>0x1d) && (scancode<0x39))		/* usb_kbd_numkey */
	{
		/*if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))  // shifted
			keycode=usb_kbd_numkey_shifted[scancode-0x1e];
		else // non shifted
			keycode=usb_kbd_numkey[scancode-0x1e];*/
	    if ((scancode != 0x2F) && (scancode != 0x34))
	    {
	      // non-special control char input before, or non A,E,I,O,U
	      if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))  /* shifted */
	        keycode=usb_kbd_numkey_shifted[scancode-0x1e];
	      else /* non shifted */
	        keycode=usb_kbd_numkey[scancode-0x1e];
	      ctrl_active = 0;
	    }
	    else
	    {
	      // special control char in Spanish kbd (0x2F or 0x34)
	      if(((modifier&(1<<LEFT_SHIFT))!=0)||((modifier&(1<<RIGHT_SHIFT))!=0))  /* shifted */
	      {
	        // shifted
	        if (scancode==0x34)
	          ctrl_active = CTRL_DIERESIS;
	        else
	          ctrl_active = CTRL_ARRWUP;
	      }
	      else
	      {
	        // not shifted
	        if (scancode==0x34)
	          ctrl_active = CTRL_TILDE;
	        else
	          ctrl_active = CTRL_DOWN;
	      }
	      return 0;
	    }
	}
#define CONFIG_EXTENDED_KEYCODE
#ifdef CONFIG_EXTENDED_KEYCODE
	if(((scancode >= 0x3a) && (scancode < 0x53))/*0x3a:F1,0x53:NumLock,0x63:keypad . and DEL*/
		||(scancode > 0x63))//all the other keys,exclude the whole keypad(NumLock excluded also)
	{
		keycode = scancode + 0x80;
	}

	if((scancode <= 0x63)&&(scancode >= 0x54))//keypad exclude "NumLock"
	{
		if(!num_lock)
			;//keycode = keypad[scancode-0x54];
		else
			keycode = keypad_locked[scancode-0x54];
	}
#endif

	if(scancode==NUM_LOCK) {
		num_lock=~num_lock;
		//return 1;
	}
	if(scancode==CAPS_LOCK) {
		caps_lock=~caps_lock;
		//return 1;
	}
	if(scancode==SCROLL_LOCK) {
		scroll_lock=~scroll_lock;
		//return 1;
	}


	//////////////////////aqui captura///////////////////////////////////////////////////////

	//graphicWindow(titulo);
	//gotoxy(3, 2);printf("%s", label);

	// no escape, no tab, no num lock
	if(/*keycode != 0x1b &&*/ keycode != 0x09 && keycode > 0 && keycode < 0x80)
	{
		/*if(line == K_Line1)
		{
			printf("\x1B%c%c",K_Line2,1);
			line = 2;
			col = 1;
			enterline[line-2] = col;
		}*/
		if (keycode == '\b')	//Backspace
		{
			if( *(contChar) > 0 ){
				*(contChar) -= 1;
				buffer[ *(contChar) ] = 0x00;
				if( *(scroll) > 0 ){
					*(scroll) -= 1;
				}
				if( *(contChar) == 0 )
					DispGotoMW( MW_LINE5 + 1, MW_SPFONT );printf("%s", linea);
				/*printf("\x1B%c%c",8,1);
				printf("|%s|", extKeyBoardBuffer);
				Delay10ms(100);*/
				//DispGotoMW( MW_LINE7 + 2, MW_SPFONT );printf("%d", *(contChar));APM_WaitKey(3000, 0);

			}
			/*if (col <= 1 && line == 2) // for line 2, col 1 use
			{
				col = 1;
				enterline[line-2] = col;
				printf("\x1B%c%c",line,col+1);
				printf("%c",keycode);
			}
			else if (col <= 1)			// for line > 2, col 1 use
			{
				line--;
				if (enterline[line-2] == 21)
				{
					col = 20;
					enterline[line-2] = col;
				}
				else
				{
					col = enterline[line-2];
					printf("\x1B%c%c",line,col);
					col--;
					enterline[line-2] = col;
				}
				printf("%c",keycode);
			}
			else						// for normal case
			{
				printf("%c",keycode);
				col--;
				enterline[line-2] = col;
			}*/
		}




		else if (keycode == '\n')	// Enter
		{
			if( *(contChar) >= min  )
				return 1;
			/*if (col == 21)
				printf("\x1B%c%c",line,col);
			line++;
			enterline[line-3] = col;		// update col of previous line
			printf("%c",keycode);
			col = 1;
			enterline[line-2] = col;		// update col of current line
			if(line == 10)
			{
				ClrBelow(2);
				line = 2;
				col = 1;
				enterline[line-2] = col;
				printf("\x1B%c%c",line,col);
			}*/
		}
		else if(keycode == 0x1b){	// Esc
			return 2;
		}
		else
		{
			if( *(contChar) < max ){
				buffer[*(contChar)] = keycode;
				*(contChar) += 1;
			}
			else
				return 0;
			//lenBuffer = strlen(extKeyBoardBuffer);
			if( *(contChar) > lenField ){
				*(scroll) += 1;
				//printf("%s", extKeyBoardBuffer+scroll);
			}
			/*else{
				printf("%s", extKeyBoardBuffer);
			}*/
			/*if (col == 21)
			{
				enterline[line-2] = 21;
				line++;
				col = 2;
			}
			else
				col++;
			enterline[line-2] = col;
			if (line == 10)
			{
				ClrBelow(2);
				line = 2;
				col = 1;
				enterline[line-2] = col;
				printf("\x1B%c%c",line,col);
			}
			printf("%c",keycode);*/
		}
		//printf("\f");
		//graphicWindow(titulo);
		//gotoxy(3, 2);printf("%s", label);
		/*os_disp_putc(MW_LSEL_SPFONT);
		os_disp_putc(0x1C);
		for(i=colIni; i<=colFin; i++){
			printf("\x1B%c%c",fila,i);
			printf("%c", " ");
		}*/
		/*gotoxy(5, 2);*/DispGotoMW( MW_LINE5 + 2, MW_SPFONT );printf("%s", linea);
		//printf("\x1B%c%c",fila,colIni);		//NOHORA 27Nov
		printf("\x1B%c%c", 5, 2); printf("_");	//NOHORA 27Nov
		if( *(contChar) > lenField ){
			//scroll++;
			printf("\x1B%c%c",fila,colIni);		//NOHORA 27Nov
			printf("%s", buffer + *(scroll));
		}
		else{
			printf("\x1B%c%c",fila,colIni);		//NOHORA 27Nov
			printf("%s", buffer);
		}
		//os_disp_putc(0x1D);
	}
	/*else if(keycode == 0x1b){
		printf("\x1B%c%c", 8, 1);
		printf("Press ESC key");
		WaitKey();
	}*/
	//os_disp_putc(K_PushCursor);
	//printf("\x1B\x01\x01\x1CRawCode:%02x%02x\x5\x1D",modifier,scancode);
	//printf("\x1B\x01\x01\x1C%02x%02x %d %d\x5\x1D",modifier,scancode, line, col);
	//os_disp_putc(K_PopCursor);
	return 0;
}



BOOLEAN getSExtKeyboard(int idPort, BYTE *buffer, BYTE *auxCed_kbdbuff){	//NOHORA 23Nov agregue el param 3 a esta funcion
	int len, i;
	BYTE retorno = 0;
	//BYTE verTitulo = TRUE;

	graphicWindow(miCapturaExt.titulo);
	/*gotoxy(3, 2);*/printf("\x1B%c%c", 3, 2);printf("%s", miCapturaExt.label);

	printf("\x1B%c%c", 5, 2); printf("_");	//NOHORA 27Nov
	printf("\x1B%c%c", 7, 2);printf("No de CC/CI:");	//NOHORA 23Nov
	printf("\x1B%c%c", 8, 2);printf("%s", &auxCed_kbdbuff[1]);	//NOHORA 23Nov

	while(TRUE)
	{
		/*if(verTitulo){
			graphicWindow(miCapturaExt.titulo);
			gotoxy(3, 2);printf("%s", miCapturaExt.label);
			verTitulo = FALSE;
		}*/
		os_sleep();
		if(os_kbd_getkey() == K_KeyCancel)
			break;

		len = os_file_read(idPort, &i, sizeof(i));
		if(len < 0)
			break;
		else if(len == 0)			// nothing return from kbd
			continue;
		else{						// character return from kbd
			retorno = getkeyPress( i&0xff, (i>>8)&0xff, miCapturaExt.titulo, miCapturaExt.label , &miCapturaExt.contChar, &miCapturaExt.scroll,
						buffer, miCapturaExt.colIni, miCapturaExt.colFin, miCapturaExt.minChar, miCapturaExt.maxChar);
		}
		if(retorno == 1)
			return TRUE;

		if(retorno == 2)
			return FALSE;
	}
	//printf("\fRetorno: %d", retorno);getch(0);
	return FALSE;
}
