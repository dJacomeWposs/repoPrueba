/*-----------------------------------------------------------------------------
 *  File          : keytrans.c
 *  Module        : T1000
 *  Description   : Include routines for sale/offline transaction.
 *  Author        : John
 *  Notes         :
 *
 *  Date          Who         Action
 *  ------------  ----------- -------------------------------------------------
 *  19 Mar  2012  John        Port from CREON/PR608.
 *-----------------------------------------------------------------------------
 */
#include <string.h>
#include <stdio.h>
#include "constant.h"
#include "corevar.h"
#include "sysutil.h"
#include "tranutil.h"
#include "message.h"
#include "keytrans.h"
#include "key2dll.h"
#include "midware.h"
#include "util.h"
#include "input.h"


static BYTE dhost_idx=0;

/******************************************************************************
 *  Function        : key_ready
 *  Description     : Check if key loaded in key bank
 *  Input           : idx;            // key index
 *  Return          : TRUE;           // key loaded
 *                    FALSE;          // key not loaded
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
/*BOOLEAN key_ready(BYTE idx)
{
  BYTE keysts[40];

  KDLL_KeyStatus(keysts);
  if (keysts[idx] & KEY_STATUS_LOADED)
    return TRUE;
  else
    return FALSE;
}*/
/******************************************************************************
 *  Function        : ResetKey
 *  Description     : Reset terminal key
 *  Input           : idx;            // key index
 *  Return          : TRUE;           // key reset ok
 *                    FALSE;          // key reset failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void ResetKey(BYTE idx)
{
  DispLineMW("Resetting Keys..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  KDLL_KeyReset(idx);
  AcceptBeep();
}
/******************************************************************************
 *  Function        : LoadTmk
 *  Description     : Load terminal master key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN LoadTmk(void)
{
  BYTE keycfg[20];
  WORD ret;
  BYTE buf[32];

  // injecting the TMK
  keycfg[0] = KEYMODE_MKEY | KEYMODE_DOUBLE;
  keycfg[1] = MKEY_IDX;
  memcpy(&keycfg[2], "\xFF\x00", 2);
  memcpy(&keycfg[4], \
    "\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10", 16);
  
  ret = KDLL_KeyInject(keycfg);
  if (ret) {
    DispLineMW("TMKI Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
    SprintfMW(buf, "Ret = %d", ret);
    DispLineMW(buf, MW_LINE7, MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(200, 0);
    return FALSE;
  }
  AcceptBeep();
  return TRUE;
}
/******************************************************************************
 *  Function        : LoadKeys
 *  Description     : Load data encrytion/decryption keys
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void LoadKeys(void)
{
  BYTE keycfg[20];

  DispClrBelowMW(MW_LINE1);
  
  // injecting the encryption key
  keycfg[0] = KEYMODE_ENC | KEYMODE_DOUBLE;   // double key length
  keycfg[1] = EKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;
  // plain key: "00001111222233334444555566667777"
  memcpy(&keycfg[4], "\x50\xF2\xCF\xA3\x61\x9F\xFA\x33", 8);
  memcpy(&keycfg[12], "\xD8\x65\x77\xE7\x45\x18\x2B\x3A", 8);
  
  if (KDLL_KeyInject(keycfg)) {
    DispLineMW("Inject Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  else {
    DispLineMW("Inject EKEY OK", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  AcceptBeep();
  APM_WaitKey(9000, 0);
  
  // injecting the decryption key
  keycfg[0] = KEYMODE_DEC | KEYMODE_DOUBLE;   // double key length
  keycfg[1] = DKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;
  // plain key: "88889999AAAABBBBCCCCDDDDEEEEFFFF"
  memcpy(&keycfg[4], "\x71\xDF\x52\x1B\x70\x9B\xF7\x8E", 8);
  memcpy(&keycfg[12], "\x08\xF2\x3E\x2D\x0C\x79\xC3\x6A", 8);
  
  if (KDLL_KeyInject(keycfg)) {
    DispLineMW("Inject Failed!", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  else {
    DispLineMW("Inject DKEY OK", MW_LINE5, MW_CENTER|MW_BIGFONT);
  }
  AcceptBeep();
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : EncData
 *  Description     : Encrypt data for testing
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void EncData(void)
{
  BYTE kbdbuf[48], buf[128];
  WORD ret;
  
  while (1) {
    kbdbuf[0] = 0;
    DispLineMW("I/P Data [Enc]:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    if (!APM_GetKbd(HEX_INPUT+ECHO+MW_LINE3, 16, kbdbuf))
      return;
    if ((kbdbuf[0]%16) == 0)
      break;
    LongBeep();
  }
  compress(&kbdbuf[1], &kbdbuf[1], kbdbuf[0]/2);
  ret = KDLL_Key3Encrypt(EKEY_IDX, &kbdbuf[1], kbdbuf[0]/2);
  if (ret) {
    SprintfMW(buf, "Enc Err: %d", ret);
    DispLineMW(buf, MW_LINE5, MW_BIGFONT);
  }
  else {
    DispLineMW("Encrypted:", MW_LINE5, MW_SMFONT);
    split(buf, &kbdbuf[1], kbdbuf[0]/2);
    DispGotoMW(MW_LINE6, MW_SMFONT);
    DispPutNCMW(buf, kbdbuf[0]);
    DispCtrlMW(K_SelBigFont);
  }
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : DecData
 *  Description     : Decrypt data for testing
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void DecData(void)
{
  BYTE kbdbuf[36], buf[32];
  WORD ret;
  
  while (1) {
    kbdbuf[0] = 0;
    DispLineMW("I/P Data [Dec]:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    if (!APM_GetKbd(HEX_INPUT+ECHO+MW_LINE3, 16, kbdbuf))
      return;
    if ((kbdbuf[0]%16) == 0)
      break;
    LongBeep();
  }
  compress(&kbdbuf[1], &kbdbuf[1], kbdbuf[0]/2);
  ret = KDLL_Key3Decrypt(DKEY_IDX, &kbdbuf[1], kbdbuf[0]/2);
  if (ret) {
    SprintfMW(buf, "Dec Err: %d", ret);
    DispLineMW(buf, MW_LINE5, MW_BIGFONT);
  }
  else {
    DispLineMW("Decrypted:", MW_LINE5, MW_SMFONT);
    split(buf, &kbdbuf[1], kbdbuf[0]/2);
    DispGotoMW(MW_LINE6, MW_SMFONT);
    DispPutNCMW(buf, kbdbuf[0]);
    DispCtrlMW(K_SelBigFont);
  }
  APM_WaitKey(9000, 0);
}
/******************************************************************************
 *  Function        : SetMode
 *  Description     : Set ECB/CBC mode for encryption/decryption
 *  Input           : N/A
 *  Return          : N/A
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
void SetMode(void)
{
  BYTE key;
  BYTE kbdbuf[18];
  
  memset(kbdbuf, 0, sizeof(kbdbuf));
  DispLineMW("ENC/DEC Mode:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
  DispLineMW("[ENTER] - ECB", MW_LINE3, MW_BIGFONT);
  DispLineMW("[CLEAR] - CBC", MW_LINE5, MW_BIGFONT);
  switch (key=APM_WaitKey(9000, 0)) {
    case MWKEY_ENTER:
      KDLL_SetOpMode(MODE_ECB, kbdbuf);
      AcceptBeep();
      break;
    case MWKEY_CLR:
      DispLineMW("Init Vector:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
      if (!APM_GetKbd(HEX_INPUT+ECHO+MW_LINE3+RIGHT_JUST, IMIN(16)+IMAX(16), kbdbuf))
        return;
      compress(&kbdbuf[1], &kbdbuf[1], 8);
      KDLL_SetOpMode(MODE_CBC, &kbdbuf[1]);
      AcceptBeep();
      break;
    default:
      break;
  }
}
/******************************************************************************
 *  Function        : GetPIN
 *  Description     : Get PIN from user
 *  Input           : N/A
 *  Return          : TRUE;           // get pin ok
 *                    FALSE;          // get pin failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN GetPIN(void)
{
  BYTE pinblk[10];
  BYTE msg[] = "\f\x11\nENTER PIN";
  BYTE buf[32];
  
  memset(gGTS.s_input.sb_pin, ' ', 8);

  if (!key_ready(EKEY_IDX)) {
    DispLineMW("NO PIN KEY", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000,0);
    return FALSE;
  }

#if 0
  // Online PIN
  memcpy(pinblk, gGTS.s_input.sb_pan, 10);
  if (KDLL_GetX98PIN(pinblk, msg, EKEY_IDX))
    return FALSE;
  else
    memcpy(gGTS.s_input.sb_pin, pinblk, 8);
#else
  // GetPIN demo
  memcpy(pinblk, "\x40\x12\x34\x56\x78\x90\x9F\xFF\xFF\xFF", 10); //X9.24
  if (KDLL_GetX98PIN(pinblk, msg, EKEY_IDX))
    return FALSE;
  DispClrBelowMW(MW_LINE6);
  split(buf, pinblk, 8);
  DispGotoMW(MW_LINE7, MW_SPFONT);
  DispPutNCMW(buf, 16);
  DispCtrlMW(K_SelBigFont);
  APM_WaitKey(9000, 0);
#endif

  return TRUE;
}
/******************************************************************************
 *  Function        : SetDhostIdx
 *  Description     : Reset terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN SetDhostIdx(void)
{
  BYTE kbdbuf[4];
  
  kbdbuf[0] = 1;
  kbdbuf[1] = '0' + dhost_idx;
  while (1) {
    DispLineMW("DHost Idx(0-4):", MW_LINE5, MW_CLRDISP|MW_BIGFONT);
    if (!APM_GetKbd(NUMERIC_INPUT+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMIN(1)+IMAX(1), kbdbuf))
      return FALSE;
    if (kbdbuf[1] < '5')
      break;
    else
      LongBeep();
  }
  dhost_idx = kbdbuf[1] - '0';
  return TRUE;
}
/******************************************************************************
 *  Function        : ResetDukptIkey
 *  Description     : Reset terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN ResetDukptIkey(void)
{
  DispLineMW("Resetting Keys..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  if (!KDLL_DukptKeyReset(dhost_idx)) {
    DispLineMW("DUKI Rst Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  Short2Beep();
  return TRUE;
}
/******************************************************************************
 *  Function        : LoadDukptIkey
 *  Description     : Load terminal DUKPT init key
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN LoadDukptIkey(void)
{
  BYTE keycfg[32];

  // injecting the initial key
  keycfg[0] = KEYMODE_DOUBLE;
  keycfg[1] = dhost_idx;
  // initial key (16 bytes)     6AC292FAA1315B4D 858AB3A3D7D5933A
  memcpy(&keycfg[2], "\x6A\xC2\x92\xFA\xA1\x31\x5B\x4D\x85\x8A\xB3\xA3\xD7\xD5\x93\x3A", 16);
  // KSN (10 bytes)             FFFF9876543210E00000  
  memcpy(&keycfg[18], "\xFF\xFF\x98\x76\x54\x32\x10\xE0\x00\x00", 10);
  
  DispLineMW("Loading IKey..", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  if (KDLL_DukptKeyInject(keycfg)) {
    DispLineMW("DUKI Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  Short2Beep();
  return TRUE;
}
/******************************************************************************
 *  Function        : GetDukptPin
 *  Description     : Get DUKPT encrypted pin block
 *  Input           : N/A
 *  Return          : TRUE;           // key load ok
 *                    FALSE;          // key load failed
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN GetDukptPin(void)
{
  const BYTE msg[]="\f\x11==== DUKPT =====\nENTER PIN:\x0";
  BYTE pinblk[32];
  BYTE buf[32];

  // format the PAN
  memcpy(pinblk, "\x40\x12\x34\x56\x78\x90\x9F\xFF\xFF\xFF", 10); //X9.24
  
  if (KDLL_DukptGetPin(pinblk, msg, dhost_idx)) {
    DispLineMW("DGetPIN Failed!", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
    LongBeep();
    APM_WaitKey(9000, 0);
    return FALSE;
  }
  DispClrBelowMW(MW_LINE6);
  split(buf, pinblk, 8);
  DispGotoMW(MW_LINE7, MW_SPFONT);
  DispPutNCMW(buf, 16);
  split(buf, &pinblk[8], 10);
  DispGotoMW(MW_LINE8, MW_SPFONT);
  DispPutNCMW(buf, 20);
  DispCtrlMW(K_SelBigFont);
  APM_WaitKey(9000, 0);
  return TRUE;
}



/********************FUNCIONES PICHINCHA******************************************/



BOOLEAN LoadEWKPin( BYTE *aEWK)
{
  BYTE keycfg[20];
  WORD ret;
//  BYTE wKey[16];

  // injecting the EWK
  keycfg[0] = KEYMODE_ENC | KEYMODE_DOUBLE;
  keycfg[1] = EKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;

 // memcpy( wKey, "\x45\x94\x24\x00\x00\x00\x28\x53\xff\xff\x45\x94\x24\x00\x00\x00", 16 );
  memcpy(&keycfg[4], aEWK, 16);
 // memcpy(&keycfg[4], wKey, 16);


  /*printf("\f %02x%02x%02x%02xKEY:|%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x|",  keycfg[0], keycfg[1], keycfg[2],  keycfg[3],  keycfg[4], keycfg[5], keycfg[6],
      keycfg[7], keycfg[8], keycfg[9], keycfg[10], keycfg[11], keycfg[12], keycfg[13], keycfg[14], keycfg[15],
      keycfg[16], keycfg[17], keycfg[18],keycfg[19]);

WaitKey(3000,0);*/

  //memcpy( wKey, "\x45\x94\x24\x00\x00\x00\x28\x53\xff\xff\x45\x94\x24\x00\x00\x00", 16 );

  ret = KDLL_KeyInject(keycfg);
  //printf("\fret1: %d", ret);WaitKey(3000, 0);
  if( ret )
  {
    DispLineMW( "ERROR EWKPIN INJECT",  MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT );
    Delay1Sec( 200, TRUE );
    //printf("\nret:|%d|", ret);
  // WaitKey(3000,0);
    return FALSE;
  }

  //AcceptBeep();
  return TRUE;
}

BOOLEAN LoadEWKCard( BYTE *aEWK )
{
  BYTE keycfg[20];
  WORD ret;

  // injecting the EWK
  keycfg[0] = KEYMODE_ENC | KEYMODE_DOUBLE;
  keycfg[1] = CARDKEY_IDX;
  keycfg[2] = MKEY_IDX;
  keycfg[3] = 0x00;

  memcpy(&keycfg[4], aEWK, 16);

  /*printf("\fKEY2:|%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x|", keycfg[4], keycfg[5], keycfg[6],
      keycfg[7], keycfg[8], keycfg[9], keycfg[10], keycfg[11], keycfg[12], keycfg[13], keycfg[14], keycfg[15],
      keycfg[16], keycfg[17], keycfg[18],keycfg[19]);

  WaitKey(3000, 0);*/


  ret = KDLL_KeyInject(keycfg);
  //printf("\fret: %d", ret);WaitKey(3000, 0);
  if( ret )
  {
    DispLineMW( "ERROR EWKCARD INJECT",  MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT );
    Delay1Sec( 200, TRUE );
   // printf("\nret:|%d|", ret);
  //  GetChar();
    return FALSE;
  }

  //AcceptBeep();
  return TRUE;
}



void IccTmkInject(void)
{
  BYTE key;

  DispLineMW("ICC TMK INJECT:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
  DispLineMW("[ENTER] - Normal", MW_LINE3, MW_BIGFONT);
  DispLineMW("[CLEAR] - DUKPT", MW_LINE5, MW_BIGFONT);
  switch (key=APM_WaitKey(9000, 0)) {
    case MWKEY_ENTER:
      ResetKey(MKEY_IDX);// Daniel Jacome 28 de Febrero de 2013
      KDLL_IccMKeyInject(MKEY_IDX, 0);    // Terminal Master Key
      break;
    case MWKEY_CLR:
      KDLL_IccMKeyInject(MKEY_IDX, 1);    // DUKPT Initial Key
      break;
    default:
      break;
  }
  WaitICCRemove();
}


/******************************************************************************
 *  Function        : key_ready
 *  Description     : Check if key loaded in key bank
 *  Input           : idx;            // key index
 *  Return          : TRUE;           // key loaded
 *                    FALSE;          // key not loaded
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BOOLEAN key_ready(BYTE idx)
{
  BYTE keysts[40];

  KDLL_KeyStatus(keysts);
  if (keysts[idx] & KEY_STATUS_LOADED){
	  //printf("\fSale pro true...");WaitKey(3000, 0);
    return TRUE;
  }
  else{
    return FALSE;
  }
}



void SetMasterKey( void )
{
  while( TRUE )
  {
    os_sleep();
	if( !key_ready( MKEY_IDX ) )
    {
      printf("\f");
      DispLineMW("INYECTE", MW_LINE3, MW_SMFONT|MW_CENTER);
      DispLineMW("MASTERKEY", MW_LINE4, MW_SMFONT|MW_CENTER);
      LongBeep( );
      WaitKey( 300, 0 );
    }
    else
    {
      break;
    }
    KDLL_IccMKeyInject( MKEY_IDX, 0 );
  }
}


