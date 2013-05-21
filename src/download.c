//-----------------------------------------------------------------------------
//  File          : download.c
//  Module        :
//  Description   : App download test program.
//  Author        : Lewis
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
//  01 Apr  2009  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midware.h"
#include "coremain.h"
#include "sysutil.h"
//#include "kbdutil.h"
//#include "apm.h"
//#include "menu.h"
//#include "PPPConnect.h"
#include "download.h"
//#include "gui.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
#if(T700)
const BYTE KAuthKey[16] = {
  0x99, 0x7e, 0xe6, 0x5a, 0xfa, 0x83, 0x39, 0xf1,
  0x53, 0x01, 0xb8, 0x80, 0x04, 0x7d, 0x95, 0x78
};
#endif
#if (T1000)
const BYTE KAuthKey[16] = {
//		0x34,0xBC,0xF3,0x6E,0xFC,0xEB,0x00,0x5B,0x53,0x01,0xB8,0x80,0x04,0x7D,0x95,0x78
		0x88,0x98,0xD7,0x21,0x27,0xCC,0xFD,0x49,0x4C,0xDF,0x92,0x49,0x18,0x4C,0x77,0x97
};
#endif
const BYTE *KTempFile = "dltemp.tms";
/*const BYTE KAuthKey[16] = {
  0xee, 0xa0, 0xea, 0x6f, 0x6b, 0xca, 0x98, 0xf8,
  0x53, 0x01, 0xb8, 0x80, 0x04, 0x7d, 0x95, 0x78
};*/
//const BYTE *KTempFile = "dltemp.tms";

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
/*const struct FuncTable KFuncMain[] =    // Func Test Menu
{
  {"From Local",        DownloadLocal},
  {"From FTP",          DownloadFtp},
  {"PPP Setup",         PPPSetup},
  {"PPP Status",        PPPStatus},
  {"PPP Connect",       PPPConnect},
  {"PPP DisConnect",    PPPDisConnect},
  {"Return Main",       NULL},
  {NULL, NULL}
};*/

//*****************************************************************************
//  Function        : DownloadAuth
//  Description     : Auth for app download.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int DownloadAuth(void)
{
  BYTE challenge[8];
  DWORD crypto;

  os_get_challenge(challenge);
  os_3deskey(KAuthKey);
  os_3des(challenge);

  crypto = conv_bl(challenge);
  if (os_auth(K_AuthAppDll, crypto) == FALSE)
    return -1;

  return 0;
}


//*****************************************************************************
//  Function        : DownloadWrite
//  Description     : Write application after downloading.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int DownloadWrite(BYTE *aFilename)
{
  int ret;
  //clear_area_screen(2,2,7,20,FALSE);
 // clrscr();
  graphicWindow("RESULTADO");
  //CPrintfMW("\x1B\x07\x03");
#if(T700)
  os_disp_putc(MW_SMFONT);
#else
//  os_disp_putc(MW_LSEL_SMFONT);
#endif
  // write app into application file
  ret = os_appdl_write_app(aFilename);
  switch(ret) {
    case K_APPDL_RESULT_OKAY:
   //     centerS(4,1,16,"PROCESO O.K.");
    	showMessage(18, 3, "DESCARGA FTP");
        AcceptBeep();
        AcceptBeep();
        AcceptBeep();
      //CPrintfMW("DLWR Completed");
      break;
    case K_APPDL_RESULT_SAME:
    	showMessage(19, 3, "DESCARGA FTP");
//      centerS(4,1,16,"APLICACION");
//      centerS(5,1,16,"EXISTENTE");
      //CPrintfMW("DLWR App Exist");
      break;
    case K_APPDL_RESULT_AUTHERR:
    	showMessage(20, 3, "DESCARGA FTP");
//      centerS(4,1,16,"ERROR DE");
//      centerS(5,1,16,"AUTENTICACION");
      //CPrintfMW("DLWR Auth Err");
      break;
    case K_APPDL_RESULT_DATAERR:
    	showMessage(21, 3, "DESCARGA FTP");
//      centerS(4,1,16,"ERROR DE");
//      centerS(5,1,16,"DATOS!!");
      //CPrintfMW("DLWR Data Err");
      break;
    case K_APPDL_RESULT_UNKNOWN:
    default:
    	showMessage(22, 3, "DESCARGA FTP");
//      centerS(4,1,16,"ERROR");
//      centerS(5,1,16,"DESCONOCIDO");
#if DEMO_FTP
      gotoxy(6,10);printf("%d",ret);
#endif
      //CPrintfMW("DLWR Unknown Err[%d]", ret);
      break;
  }
//  getch(500);
  Delay10ms(500);
#if(T700)
  os_disp_putc(MW_SPFONT);
#else
//  os_disp_putc(MW_LSEL_SPFONT);
#endif
  return ret;
}

//*****************************************************************************
//  Function        : DownloadMain
//  Description     : Main logic for app download program.
//  Input           : N/A
//  Return          : -1;       // Reset Selected
//                    others;   // normal return.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*int DownloadMain(void)
{
  // Application start here!!
  int selection;
  BYTE buf[32], *ptr;

  // Main Menu
  selection = 0;
  while (1)
  {
    selection = ShowMenu ((const struct FuncTable*)&KFuncMain, (const char*)KSoftName, 1, selection);
    if (selection >= 0) {
      if (KFuncMain[selection].pFunc != NULL) {
        // Print Header Line
        ptr = buf;
        memcpy(ptr, "\x10\f\x1C", 3);
        ptr += 3;
        FormatLine(ptr, KFuncMain[selection].pcName, 20);
        ptr += 20;
        *ptr++= 0x1D;
        PutNCharMW(buf, ptr-buf);
        KFuncMain[selection].pFunc();
      }
      else {
        break;
      }
    }
  }

  return 0;
}*/

