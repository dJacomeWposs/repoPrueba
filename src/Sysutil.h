//-----------------------------------------------------------------------------
//  File          : sysutil.h
//  Module        :
//  Description   : Declrartion & Defination for SYSUTIL.C
//  Author        : Lewis
//  Notes         : No UI is include in the routines, call util lib
//                  Only.
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
#ifndef _INC_SYSUTIL_H_
#define _INC_SYSUTIL_H_
#include "common.h"

//-----------------------------------------------------------------------------
//  System Timer Usage
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Beep & delay Functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Display Handling Functions.
//-----------------------------------------------------------------------------
#if (T1000)
// RRRRRGGGGGIBBBBB
#define RGB(r,g,b)      (((r&0xF8)<<8) | ((g&0xF8)<<3) | ((b&0xF8)>>3) | ((r&g&b&4)<<3))
#define T800_TO_T1000(x) ((((x&0x001F)<<11) | ((x&0x03E0)<<1) | ((x&0x7C00)>>10) | ((x&0x8000)>>10)))
#else  // T800
// IBBBBBGGGGGRRRRR
#define RGB(r,g,b)      (((r&0xF8)>>3) | ((g&0xF8)<<2) | ((b&0xF8)<<7) | ((r&g&b&4)<<13))
#endif

//  Color Defines (2 bytes)
#define COLOR_BLACK    RGB(0x00, 0x00, 0x00)
#define COLOR_WHITE    RGB(0xFF, 0xFF, 0xFF)
#define COLOR_RED      RGB(0xFF, 0x00, 0x00)
//#define COLOR_YELLOW   RGB(0x00, 0xFF, 0xFF)
#define COLOR_YELLOW   RGB(0xFF, 0xFF, 0x00)
#define COLOR_GREEN    RGB(0x00, 0xFF, 0x00)
#define COLOR_BLUE     RGB(0x00, 0xFF, 0x00)
#define COLOR_BACKGR   RGB(0xB0, 0xC1, 0xF0)


extern void LongBeep(void);
extern void AcceptBeep(void);
extern void Short2Beep(void);
extern void Short1Beep(void);
extern void Delay1Sec(DWORD aTime, BOOLEAN aBeepReqd);
extern void ErrorDelay(void);
extern void Delay10ms(DWORD aCnt);

//-----------------------------------------------------------------------------
//  Date/Time Handling Functions
//-----------------------------------------------------------------------------
struct DATETIME {
  BYTE  b_century;
  BYTE  b_year;
  BYTE  b_month;
  BYTE  b_day;
  BYTE  b_hour;
  BYTE  b_minute;
  BYTE  b_second;
};
extern void SetRTC(struct DATETIME *aTDtg);
extern void ReadRTC(struct DATETIME *aTDtg);
extern void ConvDateTime(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear);
extern BYTE SetDTG(BYTE *aDateTime);

//-----------------------------------------------------------------------------
//  Display Handling Functions.
//-----------------------------------------------------------------------------
extern void Disp2x16Msg(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl);

//-----------------------------------------------------------------------------
//    Data Buffer related Functions
//-----------------------------------------------------------------------------
extern void ByteCopy(BYTE *dest, BYTE *src, DWORD len);
extern void CompressInputData(void *aDest, BYTE *aSrc, BYTE aLen);
extern void CompressInputFData(void *aDest, BYTE *aSrc,  BYTE aLen);

//-----------------------------------------------------------------------------
//    Misc Functions
//-----------------------------------------------------------------------------
extern BYTE *GetMonthText(BYTE aMM);
extern void MemFatalErr(void *aMemPtr);
extern void DebugPoint(char *aStr);
extern DWORD GetChar(void);
extern BYTE *LTrim( BYTE *string, BYTE junk );
extern BOOLEAN PromptPowerOff(void);
extern BOOLEAN PromptReboot(void);
extern void worToBcd(int valor, BYTE * buffer);
extern int ByteBcd2Int (BYTE byteBcd);
extern void MsgError( BYTE *aSrc );
extern int StrCmp( char* aStr1, char* aStr2 );
extern void CheckPointerAddr(void *aPtr);
extern DWORD SiNo( void );
extern BYTE *RTrim( BYTE *string, BYTE junk );
extern void ReplaceLF( BYTE *aStr );
extern void packField42(void);
extern void packField4(void);
extern void RSetStr( BYTE *aStr, WORD aLen, BYTE aCar );
extern void ShowIp(unsigned char *aIp, int aPos, int aReverse);
extern BOOLEAN EditIp(BYTE *aIp, DWORD aPos);
extern int GetString(char *aDest, DWORD aPos, int aMaxLen);
extern void showMessage(BYTE idMessage, BYTE time, BYTE *title);
extern void graphicWindow(BYTE *title);
extern BOOLEAN validateAmount(BYTE *title,  int iputAmount, int minAmount, int maxAmount);
extern int ValidaCedulaRuc( char *aCedulaRuc );
extern void LSetStr(BYTE *aStr, WORD aLen, BYTE aCar);
//-----------------------------------------------------------------------------
//     Amount related functions.
//-----------------------------------------------------------------------------
struct DISP_AMT {
  BYTE len;
  BYTE content[21];
};
extern BOOLEAN ConvAmount(DDWORD aAmt, struct DISP_AMT *aDispAmt, DWORD aDecPos, BYTE aCurrSym);

#endif // _SYSUTIL_H_

