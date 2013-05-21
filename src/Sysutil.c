//-----------------------------------------------------------------------------
//  File          : sysutil.c
//  Module        :
//  Description   : Include Enhance System utitilies function.
//  Author        : Lewis
//  Notes         : No UI is include in the routines & call SYSTEM or UTILIB
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
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "midware.h"
#include "util.h"
#include "hardware.h"
#include "sysutil.h"
#include "Corevar.h"
#include "window.h"
#include "SPGLib.h"
#include "tranutil.h"
#include "Constant.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define DEBUG_POINT           1
#define EDITIP_CURSOR_MAX     3
#define EDITIP_COL            0

#define EDITNII_CURSOR_MAX    4
#define EDITNII_COL           5

#define IP_LEN                4

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------
static const BYTE KMonthText[13][3] = {
		/*{' ',' ',' '},
  {'J','A','N'},
  {'F','E','B'},
  {'M','A','R'},
  {'A','P','R'},
  {'M','A','Y'},
  {'J','U','N'},
  {'J','U','L'},
  {'A','U','G'},
  {'S','E','P'},
  {'O','C','T'},
  {'N','O','V'},
  {'D','E','C'}*/

		{ ' ', ' ', ' ' },
		{ 'E', 'N', 'E' },
		{ 'F', 'E', 'B' },
		{ 'M', 'A', 'R' },
		{ 'A', 'B', 'R' },
		{ 'M', 'A', 'Y' },
		{ 'J', 'U', 'N' },
		{ 'J', 'U', 'L' },
		{ 'A', 'G', 'O' },
		{ 'S', 'E', 'P' },
		{ 'O', 'C', 'T' },
		{ 'N', 'O', 'V' },
		{ 'D', 'I', 'C' }

};

#define FLASH_START_ADDRESS         0x00100000
#define SRAM_END_ADDRESS            0x002FFFFF

//*****************************************************************************
//  Function        : LongBeep
//  Description     : Generate long beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 30ms;
//  Globals Changed : N/A
//*****************************************************************************
void LongBeep(void)
{
  BeepMW(30, 9, 1);
}
//*****************************************************************************
//  Function        : AcceptBeep
//  Description     : Generate a beep sound for accept.
//  Input           : N/A
//  Return          : N/A
//  Note            : 3 X 100ms
//  Globals Changed : N/A
//*****************************************************************************
void AcceptBeep(void)
{
  BeepMW(10, 10, 3);
}
//*****************************************************************************
//  Function        : Short2Beep
//  Description     : Generate two short beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 2 X 20ms
//  Globals Changed : N/A
//*****************************************************************************
void Short2Beep(void)
{
  BeepMW(2, 2, 2);
}
//*****************************************************************************
//  Function        : Short1Beep
//  Description     : Generate one short beep sound.
//  Input           : N/A
//  Return          : N/A
//  Note            : 1 X 20ms
//  Globals Changed : N/A
//*****************************************************************************
void Short1Beep(void)
{
  BeepMW(2, 2, 1);
}
//*****************************************************************************
//  Function        : Delay1Sec
//  Description     : Delay for certain second or key pressed.
//  Input           : time;         // seconds to Delay
//                    beep_reqd;    // beep required ?
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Delay1Sec(DWORD aTime, BOOLEAN aBeepReqd)
{
  if (aBeepReqd)
    LongBeep();

  aTime *= 200;  // convert to 5ms
  while (aTime) {
    SleepMW();
    if (GetCharMW()==MWKEY_CANCL)
      break;
    aTime--;
  }
}
//*****************************************************************************
//  Function        : ErrorDelay
//  Description     : Special Delay routine for error.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ErrorDelay(void)
{
  Delay1Sec(10, 1);
}
//*****************************************************************************
//  Function        : Delay10ms
//  Description     : Delay a number of 10ms .
//  Input           : cnt; // count of 10ms to Delay
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Delay10ms(DWORD aCnt)
{
  aCnt *= 2;
  while (aCnt--) SleepMW();
}
//*****************************************************************************
//  Function        : SetRTC
//  Description     : Set Real Time clock.
//  Input           : aTDtg;     // pointer to structure DATETIME.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetRTC(struct DATETIME *aTDtg)
{
  BYTE buffer[14];

  aTDtg->b_century = (aTDtg->b_year>0x50)?0x19:0x20;
  split(buffer,(BYTE*)aTDtg,7);
  RtcSetMW(buffer);
}
//*****************************************************************************
//  Function        : ReadRTC
//  Description     : Get Real Time clock.
//  Input           : dtg;     // pointer to structure struct DATETIME buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ReadRTC(struct DATETIME *aTDtg)
{
  BYTE buffer[14];

  RtcGetMW(buffer);
  compress((BYTE*)aTDtg,buffer,7);
}
//*****************************************************************************
//  Function        : ConvDateTime
//  Description     : Convert input DATETIME to specify string.
//  Input           : aBuf;       // pointer to output buf
//                    aTDtg;       // pointer struct DATETIME value
//                    aLongYear;  // convert to LONG year fmt when TRUE.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConvDateTime(BYTE *aBuf, struct DATETIME *aTDtg, BOOLEAN aLongYear)
{
  memset(aBuf,' ',18);
  memcpy(aBuf, KMonthText[(BYTE)bcd2bin(aTDtg->b_month)], 3); /* month */
  split(&aBuf[4], &aTDtg->b_day, 1); /* day */
  if (aBuf[4] == '0')
    aBuf[4] = ' ';
  aBuf[6] = ',';
  split(&aBuf[8],&aTDtg->b_century,2);
  split(&aBuf[13], &aTDtg->b_hour, 1);
  aBuf[15] = ':';
  split(&aBuf[16], &aTDtg->b_minute, 1);
  if (!aLongYear)
    memmove(&aBuf[8],&aBuf[10], 8);
}
//*****************************************************************************
//  Function        : SetDTG
//  Description     : Sync RTC with input date time.
//  Input           : aDataTime;   // pointer to 6 byte date_time
//                                 // in bcd YYMMDDHHMMSS
//  Return          : System Year BCD value;
//  Note            : Year is adjust when necessary.
//  Globals Changed : N/A
//*****************************************************************************
BYTE SetDTG(BYTE *aDateTime)
{
  BYTE tmp;
  struct DATETIME dtg;

  ReadRTC(&dtg);
  tmp = 1;
  if ((aDateTime[1] == 0x12) && (dtg.b_month == 0x01))
    bcdsub(aDateTime,&tmp,1);
  else
    if ((aDateTime[1] == 0x01) && (dtg.b_month == 0x12))
      bcdadd(aDateTime,&tmp,1);
  memcpy(&dtg.b_year,aDateTime,6);
  SetRTC(&dtg);

  return dtg.b_year;
}
//*****************************************************************************
//  Function        : ByteCopy
//  Description     : Same as memcpy. Use to avoid compiler bug.
//  Input           : dest;     // pointer to destination buffer
//                    src;      // pointer to source buffer
//                    len;      // number of bytes to copy
//  Return          : N/A
//  Note            : typedef struct {
//                      BYTE abc;
//                      WORD C;
//                    } a; b;
//                    memcpy(&a, &b, sizeof(a));
//                    IAR fail to generate correct code for about memcpy statement.
//  Globals Changed : N/A
//*****************************************************************************
void ByteCopy(BYTE *aDest, BYTE *aSrc, DWORD aLen)
{
  memcpy(aDest, aSrc, aLen);
}
//*****************************************************************************
//  Function        : CompressInputData
//  Description     : Fill the data with '0' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : N/A
//  Note            : Max len for input is 12.
//  Globals Changed : N/A
//*****************************************************************************
void CompressInputData(void *aDest, BYTE *aSrc, BYTE aLen)
{
  BYTE buffer[12];

  memset(buffer, '0', 12); /* 12 is the max len */
  memcpy(&buffer[aLen*2-aSrc[0]], &aSrc[1], aSrc[0]);
  compress(aDest, buffer, aLen);
}
//*****************************************************************************
//  Function        : CompressInputFData
//  Description     : Fill the data with 'F' & compress it.
//  Input           : aDest;    // pointer to destinate buffer;
//                    aSrc;     // pointer to src buffer.
//                              // 1st byte is len of input
//                    aLen;     // number of byte to compress.
//  Return          : N/A
//  Note            : Max len for input is 24.
//  Globals Changed : N/A
//*****************************************************************************
void CompressInputFData(void *aDest, BYTE *aSrc,  BYTE aLen)
{
  BYTE tmp[25];

  memcpy(tmp, aSrc, aSrc[0]+1);
  memset(&tmp[tmp[0]+1], 'F', 24-tmp[0]);
  compress(aDest, &tmp[1], aLen);
}
//*****************************************************************************
//  Function        : GetMonthText
//  Description     : Return pointer to three byte month text.
//  Input           : mm;    // month
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BYTE *GetMonthText(BYTE aMM)
{
  return(BYTE *)KMonthText[aMM];
}
//*****************************************************************************
//  Function        : MemFatalErr
//  Description     : Check for valid memory allocated pointer.
//                    Err: Prompt Error Message & Reset.
//  Input           : aMemPtr;          // pointer to allocated memory
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void MemFatalErr(void *aMemPtr)
{
  if (aMemPtr == NULL) {
    DispLineMW("Memory Error!", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
    LongBeep();
    while (GetCharMW() == 0) SleepMW();
    ResetMW();
  }
}
//*****************************************************************************
//  Function        : DebugPoint
//  Description     : Display Msg on line 9 and wait for key press.
//  Input           : aStr;         // pointer to message string.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DebugPoint(char *aStr)
{
#ifdef DEBUG_POINT
  DispLineMW(aStr, MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
  while (GetCharMW() == 0) SleepMW();
#endif // DEBUG_POINT
}
//*****************************************************************************
//  Function        : ConvAmount
//  Description     : Converts a formated amount to a format easy for display
//                    and printing.
//  Input           : aAmt;       // DDWORD amount
//                    aDispAmt;   // pointer to sturct DISP_AMT buffer.
//                    aDecPos;    // Decimal Position
//                    aCurrSym;   // Currency Symbol.
//  Return          : TURE  => convertion ok
//                    FALSE => conversion fail, return all '9'
//  Note            : result = > len byte + $NNNN.NN
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ConvAmount(DDWORD aAmt, struct DISP_AMT *aDispAmt, DWORD aDecPos, BYTE aCurrSym)
{
  BYTE buffer[21];
  BYTE lead0len;
  BOOLEAN overflow;

  overflow = FALSE;
  dbin2asc(buffer,aAmt);

  if (memcmp(buffer, "00000000", 8)!=0) {
    overflow = TRUE;
    memset(&buffer[8], '9', 12);
  }
  lead0len = (BYTE)skpb(buffer, '0', 17);      /* leading zero count */
  aDispAmt->content[0] = aCurrSym;
  aDispAmt->len = 19 -lead0len;          /* excluding decimal digits */
  memcpy(&aDispAmt->content[1], &buffer[lead0len], aDispAmt->len - 1);
  if (aDecPos != 0) {
    buffer[17] = '.';
    memcpy(&aDispAmt->content[aDispAmt->len], &buffer[17], 3);
    aDispAmt->len += 3;
  }
  return overflow;
}
//*****************************************************************************
//  Function        : Disp2x16Msg
//  Description     : Display 2x16 message on lcd using DispLine
//  Input           : aMsg;         // pointer to 2x16 message
//                    aOffset;      // DispLine's offset
//                    aCtrl;        // DispLine's control 
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Disp2x16Msg(const BYTE *aMsg, DWORD aOffset, DWORD aCtrl)
{
  DWORD next_line;
  BYTE  tmpbuf[17];

  memset(tmpbuf, 0, sizeof(tmpbuf));
  memcpy(tmpbuf, aMsg, 16);
  DispLineMW(tmpbuf,  aOffset, aCtrl|MW_CENTER);
  next_line = (aCtrl&MW_BIGFONT)?(MW_LINE3-MW_LINE1):(MW_LINE2-MW_LINE1);
  aCtrl &= ~MW_CLRDISP;
  memcpy(tmpbuf, &aMsg[16], 16);
  DispLineMW(tmpbuf, aOffset+next_line, aCtrl|MW_CENTER);
}



DWORD GetChar(void)
{
  DWORD ch;

  // Clear key buffer
  KbdFlushMW();

  // Wait key
  do
  {
    SleepMW();
    ch = GetCharMW();
  } while (ch == 0);

  return ch;
}


//*****************************************************************************
//  Function        : LTrim
//  Description     : Remove character 'junk' to the left of the string
//  Input           : char* aStr1, char* aStr2
//  Return          : string without 'junk'
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
BYTE *LTrim( BYTE *string, BYTE junk )
{
  BYTE* original = string;
  char *p = original;
  int trimmed = 0;
  do
  {
    if (*original != junk || trimmed)
    {
        trimmed = 1;
        *p++ = *original;
    }
  }
  while (*original++ != '\0');
  return string;
}



//*****************************************************************************
//  Function        : PromptPowerOff
//  Description     : Prompt user to Power Off.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PromptPowerOff(void)
{
  DispLineMW("Apagar ?", MW_LINE4, MW_CLRDISP|MW_BIGFONT|MW_CENTER);
  if (GetChar() == MWKEY_ENTER) {
    fCommitAllMW();
    PowerOffMW();
  }
  else
    return FALSE;

  return TRUE;
}



//*****************************************************************************
//  Function        : PromptReboot
//  Description     : Prompt user to reboot after save config.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN PromptReboot(void)
{
  DispLineMW("Reiniciar ?", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  DispLineMW("Y/N", MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (GetChar() == MWKEY_ENTER) {
    ResetMW();
  }
  else
    return FALSE;

  return TRUE;
}


void byteToBcd(BYTE valor, BYTE * buffer) {

	buffer[0] = (valor / 10) << 4;
	buffer[0] |= valor % 10;
}



void worToBcd(int valor, BYTE * buffer) {

    BYTE valor1;
    valor1 = valor / 100;
    byteToBcd(valor1, buffer);
    valor1 = valor % 100;
    byteToBcd(valor1, buffer + 1);
}
int ByteBcd2Int (BYTE byteBcd)
{
	int valorInt;
	BYTE aux;

	valorInt = (byteBcd & 0x0F);
	aux = (byteBcd & 0xF0);
	aux >>=4;
	valorInt += aux * 10;
	return (valorInt);
}


void MsgError( BYTE *aSrc )
{
#if(NEW_MIDWARE == TRUE)
	CPutCharMW(MW_LPUSH_CURSOR);
#else
  DispPutCMW(K_PushCursor);
#endif
  os_disp_textc(COLOR_RED);
  DispLineMW( aSrc, MW_LINE4, MW_SMFONT|MW_CENTER|MW_CLRDISP );
#if(NEW_MIDWARE == TRUE)
  CPutCharMW(MW_LPOP_CURSOR);
#else
  DispPutCMW(K_PopCursor);
#endif
  Delay1Sec( 3, TRUE );
}



//*****************************************************************************
//  Function        : StrCmp
//  Description     : Compares the C string str1 to the C string str2.
//  Input           : char* aStr1, char* aStr2
//  Return          : 0 = indicates that both strings are equal
//                    1 = Different
//  Note            : Esta funcion fue implementada porque memcmp does'nt ...
//                    works with string char
//  Globals Changed : N/A
//*****************************************************************************
int StrCmp( char* aStr1, char* aStr2 )
{
  while( *aStr1 != '\0' || *aStr2 != '\0' )
  {
    if( *aStr1 != *aStr2 )
      return 1;

    aStr1++;
    aStr2++;
  }
  return 0;
}


//*****************************************************************************
//  Function        : CheckPointerAddr
//  Description     : Check whether pointer is syster pointer (stack or malloc)
//                    or not
//  Input           : aPtr
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void
CheckPointerAddr(void *aPtr)
{
#ifndef WIN32
  DWORD address;

  address = (DWORD) aPtr;
  if ((address < FLASH_START_ADDRESS) || (address > SRAM_END_ADDRESS))
    return;

  DispLineMW("Not A Valid Pointer!!", MW_MAX_LINE, MW_REVERSE | MW_CENTER
      | MW_SPFONT);
  LongBeep();
  while (GetCharMW() == 0)
    SleepMW();
#endif
}


DWORD SiNo( void )
{
/*#if(NEW_MIDWARE == TRUE)
  CPutCharMW(MW_LPUSH_CURSOR);
#else*/
  DispPutCMW(K_PushCursor);
//#endif
  os_disp_backc(COLOR_GREEN);
  os_disp_textc(COLOR_BLACK);
  DispLineMW( "SI-VERDE", MW_LINE9, MW_LEFT|MW_SMFONT );
  DispPutCMW(K_PopCursor);
  DispPutCMW(K_PushCursor);
  os_disp_backc(COLOR_RED);
  DispLineMW( "NO-ROJO", MW_LINE9 + 13, MW_SMFONT );
/*#if(NEW_MIDWARE == TRUE)
  CPutCharMW(MW_LPOP_CURSOR);
#else*/
  DispPutCMW(K_PopCursor);
//#endif
  switch( APM_YesNo() )
  {
    case 0: // 0 Cancel
      return -1;
      break;
    case 1: // 1 No
      return -1;
      break;
    case 2: // 2 Enter
      break;
  }
  return 2;     // Press Enter
}


//*****************************************************************************
//  Function        : RTrim
//  Description     : Remove character 'junk' to the Right of the string
//  Input           : char* aStr1, char* aStr2
//  Return          : string without 'junk'
//                    1 = Different
//  Note            :
//  Globals Changed : N/A
//*****************************************************************************
BYTE *RTrim( BYTE *string, BYTE junk )
{
  BYTE* original = string + strlen(string);
  while(*--original == junk);
  *(original + 1) = '\0';
  return string;
}






void ReplaceLF( BYTE *aStr )
{
    DWORD i;
    BYTE *ptr;

    ptr = aStr;

    for( i = 0; i < strlen(aStr); i++ )
    {
        if( ptr[i] == '@' )
        {
            *(ptr + i) = '\n';
        }
        else
        {
            continue;
        }
    }
}


void packField4(void)
{
	BYTE lent=0,montAcssi[12];
	memset(montAcssi,0x30,sizeof(montAcssi));
	memset(glocAmount,0x00,sizeof(glocAmount));
	lent = 12 - strlen(gAmount);
	memcpy(&montAcssi[lent],gAmount,strlen(gAmount));
	compress(glocAmount,montAcssi,6);
	pack_mem(glocAmount, 6);
}


void packField42(void)
{
	BYTE spaceNum, aux_42[16];
  memset(aux_42,0x00,sizeof(aux_42));

  if(TX_DATA.b_trans == EDC_REV){
    memcpy(aux_42, gRevTable.r_User, sizeof(aux_42));  
	}
  else{
    memcpy(aux_42, gUsers_tbl.sb_user, sizeof(aux_42)); 
  }
  spaceNum = 15 - strlen(aux_42);
  pack_mem(aux_42, strlen(aux_42)); 
	pack_space(spaceNum);
}


//*****************************************************************************
//  Function        : RSetStr
//  Description     : Set the caracter 'aCar' to the right of the string = aStr
//  Input           : char* aStr1, char* aStr2
//  Return          : Nothing
//  Note            : By Jorge Numa
//  Globals Changed : N/A
//*****************************************************************************
void RSetStr( BYTE *aStr, WORD aLen, BYTE aCar )
{
  WORD i;

  if (strlen(aStr) >= aLen) {
      return;
  }

  for (i = strlen(aStr); i < aLen; i++)
  {
      *(aStr + i) = aCar;
  }
}



//*****************************************************************************
//  Function        : ShowIp
//  Description     : Show ip addr.
//  Input           : aIp;        // pointer to 4 byte ip.
//                    aPos;       // line pos.
//                    aReverse;   // TRUE=> reverse on.
//  Return          : -1;         // Cancel;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ShowIp(unsigned char *aIp, int aPos, int aReverse)
{
  BYTE buf[128];

  DispGotoMW(aPos, MW_SPFONT);
  if (aReverse) DispCtrlMW(MW_REV_ON);
  sprintf(buf, "%3d.%3d.%3d.%3d", aIp[0], aIp[1], aIp[2], aIp[3]);
  DispLineMW(buf, aPos, MW_SPFONT);
  if (aReverse) DispCtrlMW(MW_REV_OFF);
}
//*****************************************************************************
//  Function        : EditIp
//  Description     : Edit IP address.
//  Input           : aip;    // pointer to 4 bytes IP address buffer.
//                    aPos;   // position to display.
//  Return          : TRUE;
//                    FALSE;    // USER CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EditIp(BYTE *aIp, DWORD aPos)
{
  DWORD curr_field;
  DWORD cursor=0;
  BYTE data[IP_LEN], buf[128];
  DWORD i;
  DWORD keyin;

  memcpy(data,aIp,IP_LEN);

  curr_field=IP_LEN;
  while (1) {
    if (curr_field<IP_LEN) {
      /* Show all data */
      ShowIp(data, aPos, FALSE);
      /* Show editing field */
      sprintf(buf, "%-3d", data[curr_field]);
      DispLineMW(buf, aPos+(curr_field*4)+EDITIP_COL, MW_REVERSE|MW_SPFONT);
    }
    else
      ShowIp(data,aPos,TRUE);

    keyin = GetChar();

    /* Valid numeric key */
    if ((keyin>=MWKEY_0)&&(keyin<=MWKEY_9)) {
      /* Start edit 1st field at all field selected */
      if (curr_field>=IP_LEN)
        curr_field=0;
      /* Editing digit */
      if (cursor==0) {
        data[curr_field]=(BYTE) (keyin-'0');
        cursor++;
      }
      else
        if (cursor<EDITIP_CURSOR_MAX) {
          i=data[curr_field]*10+(BYTE) (keyin-'0');
          if (i>255)
            i=255;
          data[curr_field]=(BYTE)i;
          cursor++;
        }
      /* Last digit, auto ENTER */
      if (cursor==EDITIP_CURSOR_MAX)
        keyin=MWKEY_ENTER;
    }

    /* Other keys */
    switch (keyin) {
      case MWKEY_ENTER:
        if (curr_field>=IP_LEN) {
          memcpy(aIp,data,IP_LEN);
          ShowIp(data,aPos,FALSE);
          return TRUE;
        }
        curr_field++;
        cursor=0;
        break;
      case MWKEY_CLR:
        if (curr_field >= IP_LEN) {
          curr_field = 0;
        }
        else if ((cursor == 0)&&(curr_field>0))
          curr_field--;
        else if (curr_field<IP_LEN) {
          data[curr_field]=0;
          cursor=0;
        }
        break;
      case MWKEY_CANCL:
        if (curr_field<IP_LEN) {
          memcpy(data,aIp,IP_LEN);
          curr_field=IP_LEN;
          cursor=0;
        }
        else
          return FALSE;
        break;
    }
  }  // while
  return FALSE;
}



//*****************************************************************************
//  Function        : GetString
//  Description     : Get a String input.
//  Input           : aDest;      // output buffer
//                    aPos;       // line pos.
//                    aMaxLen;    // max len;
//  Return          : -1;         // Cancel;
//                    other;      // len of input.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetString(char *aDest, DWORD aPos, int aMaxLen)
{
  int cursor;

  cursor = strlen (aDest);
  while (1)
  {
    DWORD ch;

    aDest[cursor] = 0;
    DispClrLineMW(aPos);
    DispLineMW(aDest, aPos, MW_SMFONT);
    DispLineMW(" ", aPos+strlen(aDest), MW_REVERSE|MW_SMFONT);

    ch = GetChar();

    if ((ch >= '0') && (ch <= '9'))
    {
      if (cursor < (aMaxLen - 1))
      {
        aDest[cursor] = (char)ch;
        cursor ++;
      }
    }

    if (ch == MWKEY_CANCL)
      return -1;

    if (ch == MWKEY_ENTER)
    {
      DispClrLineMW(aPos);
      DispLineMW(aDest, aPos, MW_SMFONT);
      return cursor;
    }

    if (ch == MWKEY_CLR)
    {
      if (cursor)
        cursor --;
    }
  }
  return FALSE;
}



void showMessage(BYTE idMessage, BYTE time, BYTE *title){
	graphicWindow(title);
	switch(idMessage){
	case 1:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "INICIALIZACION", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "FALLIDA", MW_LINE5, MW_SPFONT|MW_CENTER/*|MW_CLRDISP*/ );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 2:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "TIEMPO DE ESPERA", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "AGOTADO", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 3:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "MONTO INFERIOR AL", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "MINIMO PERMITIDO", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 4:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "MONTO SUPERIOR AL", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "MAXINO PERMITIDO", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 5:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "NO FUE POSIBLE", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "ESTABLECER CONEXION", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 6:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "TRANSACCION", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "NO PROCESADA", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 7:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "NO SE HAN REALIZADO", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "TRANSACCIONES", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 8:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "NO FUE POSIBLE LEER", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "EL REVERSO", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 9:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "TRANSACCION", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "NO PROCESADA", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 10:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "Minimo 10 digitos!", MW_LINE4, MW_SPFONT|MW_CENTER );
		//DispLineMW( "NO PROCESADA", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 11:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "Teclado externo", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "No Conectado", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 12:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "Teclado externo", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "No Detectado", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 13:
		AcceptBeep();
		//DispPutCMW(K_PushCursor);
		//os_disp_textc(COLOR_RED);
		DispLineMW( "Configurando", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "Teclado Externo", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispLineMW( "Por Favor Espere...", MW_LINE7, MW_SPFONT|MW_CENTER );
		//DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 14:
		LongBeep();
		DispLineMW( "TRANSACCION", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "CANCELADA", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 15:
		LongBeep();
		DispPutCMW(K_PushCursor);
		os_disp_textc(COLOR_RED);
		DispLineMW( "Respuesta del Servidor", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "Incorrecta!!!", MW_LINE5, MW_SPFONT|MW_CENTER );
		DispPutCMW(K_PopCursor);
		Delay1Sec( time, TRUE );
		break;

	case 16:
		LongBeep();
		DispLineMW( "NO TIENE FACTURAS", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "PENDIENTES", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 17:
		LongBeep();
		DispLineMW( "LA NUEVA NO PUEDE", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "SER IGUAL A LA ANTERIOR", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 18:
		DispLineMW( "DESCARGA FINALIZADA", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "EXITOSAMENTE", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 19:
		LongBeep();
		DispLineMW( "APLICACION", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "EXISTENTE", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 20:
		LongBeep();
		DispLineMW( "ERROR DE", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "AUTENTICACION", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 21:
		LongBeep();
		DispLineMW( "ERROR EN LOS", MW_LINE4, MW_SPFONT|MW_CENTER );
		DispLineMW( "DATOS DESCARGADOS", MW_LINE5, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;

	case 22:
		LongBeep();
		DispLineMW( "ERROR DESCONOCIDO", MW_LINE4, MW_SPFONT|MW_CENTER );
		Delay1Sec( time, TRUE );
		break;
	}
}



void graphicWindow(BYTE *title){
	printf("\f");
	GIFLoadMem( win_logo, sizeof(win_logo));
	GIFPlayOn( 0, 0 );
	GIFPlay();
	GIFStop(0);
	GIFUnLoad();
	DispLineMW(title, MW_LINE1+((26 - strlen(title)) / 2 + 1), MW_REVERSE|MW_SPFONT);
}




BOOLEAN validateAmount(BYTE *title,  int iputAmount, int minAmount, int maxAmount){

	if( iputAmount < minAmount ){
		showMessage(3, 3, title);
		return FALSE;
	}

	if( iputAmount > maxAmount ){
		showMessage(4, 3, title);
		return FALSE;
	}
	return TRUE;
}




//*****************************************************************************
//  Function        : ValidaCedulaRuc
//  Description     : Valida si la Cedula/Ruc Ecuatoriana es Verdadera o Falsa
//                    por su digito de chequeo
//  Input           : BYTE *aCedulaRuc
//  Return          : 0 = Cedulo o Ruc INVALIDO
//                    1 = Cedula o Ruc VALIDO
//  Note            : Es una modificacion del MOD10 y MOD11 para Ced/Ruc Ecuador
//  Note 2          : By Jorge Numa
//  Globals Changed : N/A
//*****************************************************************************
int ValidaCedulaRuc( char *aCedulaRuc )
{
    char prov[3];
    int d[10];
    int i, res;
    int suma = 0;
    int imp = 0;
    int par = 0;
    int checkDigit;
    char tmpD10[3];
    int MODULO;
    int sustraendo;
    int coeficientesJE[] = {4,3,2,7,6,5,4,3,2}; // Coeficientes Juridicos y Extranjero
    int coeficientesP[] = {3,2,7,6,5,4,3,2}; // Coeficientes Empresas Publicas


    memset(prov, 0, sizeof(prov));
    memcpy(prov, aCedulaRuc, 2);

    printf("Cedula/RUC:%s\n", aCedulaRuc);

    if ( !( (atoi(prov) > 0 ) && (atoi(prov) <= 24) ) ) // 24 = Numero de provincias
        return 0;

    /* Verifica si el œltimo d’gito de la cŽdula es v‡lido */

    // copiamos la cedula a un array int
    //for (i = 0; i < strlen(cedula); i++)
    for (i = 0; i < 10; i++)
    {
        d[i] = aCedulaRuc[i] - 48;
        //printf("%d", d[i]);
    }

    if( d[2] == 7 || d[2] == 8 )
        return 0;

    if (d[2] < 6 )  // RUC Persona Natural - (Cedula) - Digito de Chequeo en Pos 10
    {
        MODULO = 10;
        // Se suman los duplos de posicion impar
        for (i = 0; i < 10; i += 2)
        {
            d[i] = ((d[i] * 2) > 9) ? ((d[i] * 2) - 9 ) : (d[i] * 2);
            imp += d[i];
        }

        // Se suman los digitos de la posicion par
        for (i = 1; i < 9; i += 2)
        {
            par += d[i];
        }

        suma = imp + par;

        memset(tmpD10, '\0', sizeof(tmpD10));

        // Se obtiene la decena superior

        sprintf( tmpD10, "%d", (suma + 10));
        memcpy(&tmpD10[1], "0", 1);

        checkDigit = atoi(tmpD10) - suma;

        checkDigit = (checkDigit == 10) ? 0 : checkDigit;
        // Si el digito10 calculado es igual al digito 10 de la cŽdula, Áes correcta!
        //printf("CheckDigit:[%d]\n", checkDigit );
        return (checkDigit == d[9]);

    }
    else if (d[2] == 6 ) // RUC Empresas Publicas - Digito de Chequeo en Pos 9
    {
        MODULO = 11;
        for (i = 0; i < 8; i++)
        {
            d[i] = d[i] * coeficientesP[i];
            suma += d[i];
        }
        res = suma % MODULO;
        sustraendo = res * MODULO;
        checkDigit = MODULO - (suma - sustraendo);
        //printf("CheckDigit:[%d]\n", checkDigit );
        // Si el digito10 calculado es igual al digito 9 de la cŽdula, Áes correcta!
        return (checkDigit == d[8]);

    }
    else if (d[2] == 9)
    {
        MODULO = 11;
        for (i = 0; i < 9; i++) // RUC Juridica / Extrajera - Digito de Chequeo en Pos 10
        {
            d[i] = d[i] * coeficientesJE[i];
            suma += d[i];
        }
        res = suma % MODULO;
        checkDigit = MODULO - res;
        checkDigit = (checkDigit == 10) ? 0 : checkDigit;
        //printf("CheckDigit:[%d]\n", checkDigit );
        // Si el digito10 calculado es igual al digito 10 de la cŽdula, Áes correcta!
        return (checkDigit == d[9]);

    }

    return -1;

}



//*****************************************************************************
//  Function        : LSetStr
//  Description     : Set the caracter 'aCar' to the left of the string = aStr
//  Input           : char* aStr1, char* aStr2
//  Return          : Nothing
//  Note            : By Jorge Numa
//  Globals Changed : N/A
//*****************************************************************************
void LSetStr(BYTE *aStr, WORD aLen, BYTE aCar)
{
	WORD i;
	WORD j;
	BYTE tmpStr[aLen + 1];
	BYTE tmp[aLen + 1];

	if (strlen(aStr) >= aLen)
		return;

	memset(tmpStr, 0, sizeof(tmpStr));
	memcpy(tmpStr, aStr, strlen(aStr));

	for (i = 0; i < aLen - strlen(aStr); i++)
	{
		*(tmp + i) = aCar;
	}

	for (i = aLen - strlen(aStr), j = 0; i < aLen; i++, j++)
	{
		*(tmp + i) = *(tmpStr + j);
	}
	memcpy(aStr, tmp, aLen);
}



