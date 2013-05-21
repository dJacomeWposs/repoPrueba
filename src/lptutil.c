//-----------------------------------------------------------------------------
//  File          : lptutil.c
//  Module        :
//  Description   : Lpt utilities functions.
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
#include <string.h>
#include "midware.h"
#include "sysutil.h"
#include "message.h"
#include "logo.h"
#include "lptutil.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
// assume space between 2 bookmark is 33 lines * 12 dot per line
#define MAX_BOOKMARK_GAP     33 * 12
#define BOOKMARK_STEPS       20

// Printer Related
#if (LPT_SUPPORT)
#define MAX_PRN_WIDTH        360
#else
#define MAX_PRN_WIDTH        384
#endif
#define LINE_STEPS           12

#define MAX_ENG_FONT         3
#define MAX_DOT_MODE         2

// Test Print state
enum {
  PRINT_LOGO         = 0,
  PRINT_CONSTLINE       ,
  PRINT_ATTR            ,
  PRINT_ERR             ,
  PRINT_END             ,
};

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
static int gLptHandle=-1;
static BYTE  gDispChgd;

//*****************************************************************************
//  Function        : WaitPrnFinish
//  Description     : Wait Print finish or printer error.
//  Input           : N/A
//  Return          : current printer status.
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
void WaitPrnFinish(void)
{
  DWORD status;

  if (gLptHandle == -1)
    return;

  Delay10ms(50);
  status = StatMW(gLptHandle, 0, NULL);
  if (status & MW_LPT_PRINTING) {
    do {
      SleepMW();
      status = StatMW(gLptHandle, 0, NULL);
      if ((status & 0xFF) != 0)    // Error
        break;
    } while ((status & MW_LPT_FINISH) == 0);
  }
}
//*****************************************************************************
//  Function        : LptClose
//  Description     : Close the printer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
void LptClose(void)
{
  if (gLptHandle >= 0) {
    WaitPrnFinish();
    CloseMW(gLptHandle);
    gLptHandle = -1;
  }
}
//*****************************************************************************
//  Function        : LptOpen
//  Description     : Open and reset the printer.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD LptOpen(void *aCfg)
{
  char filename[32];

  LptClose();
  #if (TMLPT_SUPPORT)  
  strcpy(filename, DEV_LPT2);
  #elif (LPT_SUPPORT)
  strcpy(filename, DEV_LPT1);
  #endif  
  gLptHandle = OpenMW(filename, MW_WRONLY);
  if (gLptHandle >= 0) {
    return TRUE;
  }
  return FALSE;
}
//*****************************************************************************
//  Function        : LptPutN
//  Description     : Send the chars to printer.
//  Input           : aDat;   // pointer to byte
//                    aLen;   // len of data
//  Return          : lpt status;
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD LptPutN(const BYTE *aDat, DWORD aLen)
{
  DWORD status=0;
  BYTE *ptr = (BYTE *) MallocMW(aLen);

  if (gLptHandle == -1)
    return 0;
  MemFatalErr(ptr);
  memcpy(ptr, aDat, aLen);
  if (WriteMW(gLptHandle, ptr, aLen)!=aLen) {
    //status = StatMW(gLptHandle, LPT_STS_GET, NULL);
    //sprintf(tmp, "LptWrite: %04X", status);
    //DispLineMW(tmp, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    //ErrorDelay();
  }
  FreeMW(ptr);
  status = StatMW(gLptHandle, LPT_STS_GET, NULL);
  return status;
}
//*****************************************************************************
//  Function        : LptPutS
//  Description     : Print a string.
//  Input           : ptr;    // pointer to string buffer
//  Return          : lpt status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD LptPutS(const BYTE *aStr)
{
  return LptPutN(aStr, strlen(aStr));
}
//*****************************************************************************
//  Function        : Adv2Bookmark
//  Description     : Align the paper to the next book mark.
//  Input           : aMaxStep; // Max Advance steps to find the bookmark
//  Return          : 0:Fail, 1:Paper Out, 2:Aligned
//  Note            : N/A
//  Globals Changed : N/A
//****************************************************************************
DWORD Adv2Bookmark(DWORD aMaxSteps)
{
  DWORD status, ret;
  BYTE  cmd[3];

  if (gLptHandle == -1)
    return 0;

  WaitPrnFinish();

  ret = 0; // assume fail
  do {
    cmd[0] = 0x1B;
    cmd[1] = 'J';
    cmd[2] = 0x08;
    status = LptPutN(cmd, 3);
    if (status&MW_LPT_PAPER_OUT) {  // bookmark or paper out
      ret = 1;
      cmd[2] = (BYTE) BOOKMARK_STEPS;
      status = LptPutN(cmd, 3);
      break;
    }
  } while (aMaxSteps--);

  // Double Check
  status = StatMW(gLptHandle, LPT_STS_GET, NULL);
  if (!(status & MW_LPT_STAT_PAPER_OUT) && (ret == 1))
    ret = 2;
  return ret;
}
//*****************************************************************************
//  Function        : PrintLogo
//  Description     : Print logo.
//  Input           : aWidth;     // in number of Byte
//                    aHeight;    // in number of Byte
//                    *aLogoPtr;  // pointer to constant data
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD PrintLogo(DWORD aWidth, DWORD aHeight, const BYTE *aLogoPtr)
{
  DWORD status, logobuf_size;
  BYTE *ptr;
  BYTE cmd[4] = { 0x1B, 'K', 0x00, 0x00 };

  cmd[2] = (BYTE) (aWidth/8);
  cmd[3] = (BYTE) (aHeight>248?248:aHeight);

  status = LptPutN("\x0a", 1);
  status = LptPutN(cmd, 4);
  if ((status & 0xFF) != MW_LPT_STAT_START_FINISH)
    return status;

  logobuf_size = aWidth/8 * cmd[3];
  ptr = MallocMW(logobuf_size);
  if (ptr != NULL) {
    memcpy(ptr, aLogoPtr, logobuf_size);
    status = LptPutN(ptr, logobuf_size);
    FreeMW(ptr);
  }
  return status;
}
//*****************************************************************************
//  Function        : ResetLpt
//  Description     : Reset Printer and Beep
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void ResetLpt(void)
{
  IOCtlMW(gLptHandle, IO_LPT_RESET, NULL);
  LongBeep();
  Delay10ms(300);
}
//*****************************************************************************
//  Function        : PrintBuf
//  Description     : Print buffer.
//  Input           : aBuf;        // pointer to receipt content buffer
//                    aLen;        // len of content
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PrintBuf(BYTE *aBuf, DWORD aLen, BOOLEAN aLoopPrint)
{
  DWORD status, status2, i;
  
  if (aLen == 0)
    return TRUE;

  LptOpen(NULL);
  /*#if (TMLPT_SUPPORT)
  LptPutN("\x1B\x44\x01", 3); // Half Dot Mode On
  PrintLogo(384, 48, KPrintLogo);
  LptPutN("\x1B\x44\x00\n", 4); // Half Dot Mode On
  #endif*/
  DispClrBelowMW(MW_LINE1);
  Disp2x16Msg(GetConstMsg(EDC_PN_PRINTING), MW_LINE5, MW_BIGFONT);
  gDispChgd = FALSE;
 // APM_WaitKey(3000, 0);
  
  
    status = LptPutN(aBuf, aLen);
    
  do {
    for(i=0;i<3;i++){ 
    status2 = StatMW(gLptHandle, 0, NULL);
    Delay10ms(50);
    }
   // printf("\f status %d %d %d", status2 & 0xff, status2, MW_LPT_PAPER_OUT);APM_WaitKey(3000,0);
    switch (status2) {
      case MW_LPT_FINISH :
        if (!aLoopPrint) {
          IOCtlMW(gLptHandle, IO_LPT_RESET, NULL);
        }
        return TRUE;
      case 0x101://MW_LPT_TEMP_HIGH : // high temperature
        Disp2x16Msg(GetConstMsg(EDC_PN_HIGHTEMP), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;
       case 0x100://MW_LPT_TEMP_HIGH : // high temperature
        Disp2x16Msg(GetConstMsg(EDC_PN_HIGHTEMP), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE; 

      #if (PR608D==0)
      case 0x402://MW_LPT_LOW_VOLT :
        Disp2x16Msg(GetConstMsg(EDC_PN_PLS_REPRINT), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;
      case 0x400://MW_LPT_LOW_VOLT :
        Disp2x16Msg(GetConstMsg(EDC_PN_PLS_REPRINT), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;  
      #endif
      case 0x200://MW_LPT_PAPER_OUT : // no paper error
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;  
      case 0x203://MW_LPT_PAPER_OUT : // no paper error
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;
      case MW_LPT_STAT_NOT_START : //wait print job
        SleepMW();
      case MW_LPT_PRINTING :    //printing in progress
        if (gDispChgd) {
          Disp2x16Msg(GetConstMsg(EDC_PN_PRINTING), MW_LINE5, MW_BIGFONT);
          gDispChgd = FALSE;
        }
        break;
      default :
        Disp2x16Msg(GetConstMsg(EDC_PN_PLS_REPRINT), MW_LINE5, MW_BIGFONT);
        ResetLpt();
        return FALSE;
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : PrintRcptLF
//  Description     : Print Receipt LineFeed
//  Input           : aCount;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintRcptLF(DWORD aCount)
{
  BYTE tmp[] = { "\n\x00" };
  while (aCount--) LptPutS(tmp);
}
//*****************************************************************************
//  Function        : PrintRcptFF
//  Description     : Print Receipt FormFeed
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PrintRcptFF(void)
{
  #if (LPT_SUPPORT)
  LptPutN("\x0C", 1);
  #else
  PrintRcptLF(6);
  #endif
}
