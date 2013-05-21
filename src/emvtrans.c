//-----------------------------------------------------------------------------
//  File          : emvtrans.c
//  Module        :
//  Description   : Include routines for EMV transactions.
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |        Struct define : Leading T                                         |
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
#include <stdio.h>
#include <string.h>
#include "apm.h"
#include "util.h"
#include "sysutil.h"
#include "lptutil.h"
#include "message.h"
#include "corevar.h"
//#include "key2dll.h"
#include "constant.h"
#include "chkoptn.h"
#include "input.h"
#include "print.h"
#include "hostmsg.h"
#include "record.h"
#include "reversal.h"
#include "offline.h"
#include "sale.h"
#include "tranutil.h"
//#include "emvutil.h"
#include "tlvutil.h"
#include "emvtrans.h"

//-----------------------------------------------------------------------------
//    Globals Variables
//-----------------------------------------------------------------------------
static BOOLEAN   bAcceptReferral;

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------
#define bAction         gGDS->s_EMVOut.bMsg[1]

// state machine def (order may not be as follow)
#define EMV_COMPLETE      0x00
#define EMV_START         0x01
#define EMV_SELECT_APP    0x02
#define EMV_ONLINE_PIN    0x03
#define EMV_ONLINE        0x04
#define EMV_REFERRAL      0x05
#define EMV_AMOUNT_AIP    0x06
#define EMV_CONFIRM_CARD  0x07
#define EMV_AMOUNT        0x08
#define EMV_RESTART       0x09

//-----------------------------------------------------------------------------
//    Constant
//-----------------------------------------------------------------------------
const WORD wVMOnlineTags[] = { 
  0x5F2A,                                 /* Transaction Currency Code */
  0x5F34,                                 /* PAN Sequence Number */
  0x82,                                   /* AIP */
  0x84,                                   /* Dedicated File Name */
  0x95,                                   /* TVR */
  0x9A,                                   /* Transaction Date */
  0x9C,                                   /* Transaction Type */
  0x9F02,                                 /* Amount Authorised */
  0x9F09,                                 /* Application Version # */
  0x9F10,                                 /* Issuer Application Data */
  0x9F1A,                                 /* Terminal Country Code */
  0x9F1E,                                 /* IFD Serial Number */
  0x9F26,                                 /* Application Cryptogram */
  0x9F27,                                 /* CID */
  0x9F33,                                 /* Terminal Capabilities */
  0x9F34,                                 /* CVM Result */
  0x9F35,                                 /* Terminal Type */
  0x9F36,                                 /* Application Transaction Counter */
  0x9F37,                                 /* Unpredicatable Number */
  0x9F41,                                 /* Transaction Sequence Counter */
  0x9F53,                                 /* Transaction Category code */
  0
};
//*****************************************************************************
//  Function        : DeAss
//  Description     : decode and count TLV object from the message buffer
//  Input           : *pbMsg;   // pointer to message
//                    wLen;     // len of message
//  Return          : number of Tag in message
//  Note            : N/A
//  Globals Changed : decode TLV objects store to asTagC structure
//*****************************************************************************
BYTE DeAss(TLIST *aList, BYTE *pbMsg, WORD wLen)
{
  BYTE bIdx;
  BYTE * pbMark;
  WORD wInc;

  if (wLen) {
    bIdx = 0;
    do {
      if (*pbMsg == 0 || *pbMsg == 0xFF) {
        ++pbMsg;
        --wLen;
      } else {
        if (bIdx == MAX_TAG_LIST - 1)
          break;
        //get Tag
        aList[bIdx].wTag = TlvTag(pbMark = pbMsg);
        //get Length
        wInc = TlvLen(pbMsg);
        aList[bIdx].sLen = (BYTE) wInc;
        //get buffer pointer
        aList[bIdx].pbLoc = TlvVPtr(pbMsg);
        pbMsg += TlvSizeOf(pbMsg);
        if (pbMsg - pbMark <= wLen)
          wLen -= pbMsg - pbMark;
        else
          break;
        if (wInc > TLMax)               /* length out of range */
          break;
        ++bIdx;
      }
      if (wLen == 0) {
        aList[bIdx].wTag = 0;
        return bIdx;
      }
    } while (TRUE);
  }
  aList[0].wTag = 0;
  return 0;
}
//*****************************************************************************
//  Function        : DispLabel
//  Description     : Display Application Label.
//  Input           : TLIST pointer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static void DispLabel(TLIST *psLp)
{
  BYTE i;
  BYTE len;
  BYTE buffer[MW_MAX_LINESIZE+1];

  memcpy(buffer,psLp->pbLoc,len = ((psLp->sLen < MW_MAX_LINESIZE) ? psLp->sLen : MW_MAX_LINESIZE));
  for (i=0;i<len;i++)
    if ((buffer[i] & 0x7f) < 0x20)
      buffer[i] = ' ';
  buffer[len] = 0;
  DispClrBelowMW(MW_LINE7);
  DispLineMW(buffer, MW_LINE7, MW_BIGFONT);
}
//*****************************************************************************
//  Function        : EMVChoice
//  Description     : Selection Application
//  Input           : N/A
//  Return          : FALSE => CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN EMVChoice(void)
{
  BYTE bChoice;
  BYTE bIdx;
  DWORD keyin;

  //no of tag in response buffer not in group of 2 (AID and display label)
  if ((bChoice = DeAss(gGDS->s_TagList, &gGDS->s_EMVOut.bMsg[2],
                       (WORD)(gGDS->s_EMVOut.wLen - 2)) / 2) == 0) {
    EMVFatal();
  }
  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_EMV_SEL_APP), MW_LINE3, MW_BIGFONT);
  bIdx = 0;
  do {
    //pointer to the 2nd tag (display label), each step increment by 2
    DispLabel(&gGDS->s_TagList[bIdx * 2 + 1]);
    do {
      keyin = APM_WaitKey(KBD_TIMEOUT,0);
      if (keyin == MWKEY_CANCL)
        return FALSE;
      if (keyin == MWKEY_ENTER) {
        gGDS->s_EMVIn.bMsg[0] = bIdx;
        return TRUE;
      }
      if (keyin == MWKEY_CLR)
        break;
    } while (TRUE);
    if (++bIdx == bChoice) {
      if (bChoice == 1)
        return FALSE;
      bIdx = 0;
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : EMVInput
//  Description     : Get user input for EMV transaction.
//  Input           : N/A
//  Return          : FALSE => CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN EMVInput(BOOLEAN aSelAgain)
{
  if (aSelAgain) {
    DispErrorMsg(GetConstMsg(EDC_EMV_CANNOT_SEL));
  }
  //one application and confirmation required or more application for selection
  if ((gGDS->s_EMVIn.bMsg[0]=gGDS->s_EMVOut.bMsg[1])) {
    if (EMVChoice() == FALSE)
      return FALSE;
  }
  memcpy(&gGDS->s_EMVIn.bMsg[1], "\x9F\x41\x03", 3);
  memcpy(&gGDS->s_EMVIn.bMsg[4], STIS_TERM_DATA.sb_trace_no, 3);
  gGDS->s_EMVIn.wLen = 7;
  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
  return TRUE;
}
/******************************************************************************
 *  Function        : TagPut
 *  Description     : Save Tag to input buffer.
 *  Input           : aBuf;     // Data buffer
 *                    aTag;     // Tag
 *  Return          : new buffer pointer
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BYTE *TagPut(BYTE *aBuf, WORD aTag)
{
  if (aTag / 256)
    *aBuf++ = aTag / 256;
  *aBuf = (BYTE)aTag;
  return aBuf + 1;
}
/******************************************************************************
 *  Function        : TagSeek
 *  Description     : Tag List Seek
 *  Input           : Tag;
 *  Return          : TLIST pointer
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
TLIST *TagSeek(TLIST *aSrc, WORD aTag)
{
  while (aSrc->wTag) {
    if (aSrc->wTag == aTag)
      return aSrc;
    ++aSrc;
  }
  return NULL;
}
/******************************************************************************
 *  Function        : TagData
 *  Description     : Seek for Tag Data
 *  Input           : aTag;   // Tag
 *  Return          : Pointer to Tag data
 *  Note            : N/A
 *  Globals Changed : N/A
 ******************************************************************************
 */
BYTE *TagData(TLIST *aSrc, WORD aTag)
{
  while (aSrc->wTag) {
    if (aSrc->wTag == aTag)
      return aSrc->pbLoc;
    ++aSrc;
  }
  return NULL;
}
//*****************************************************************************
//  Function        : PackTagsData
//  Description     : Prepare tag data
//  Input           : aBuf;         // pointer to dest buffer.
//                    aTags;        // list of tags
//  Return          : End of buffer pointer;
//  Note            : N/A
//  Globals Changed : N/a
//*****************************************************************************
BYTE *PackTagsData(BYTE *aBuf, WORD *aTags)
{
  WORD tag;
  TLIST *psLp;

  while ((tag = *aTags) != 0) {
    if ((psLp = TagSeek(gGDS->s_TagList, tag)) != NULL) {
      aBuf = TagPut(aBuf, tag);
      *aBuf++ = psLp->sLen; // assume byte len
      memcpy(aBuf, psLp->pbLoc, psLp->sLen);
      aBuf += psLp->sLen;
    }
    aTags++;  // next tag word
  }
  return aBuf;
}
//*****************************************************************************
//  Function        : EMVFatal
//  Description     : EMV error handler.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
void EMVFatal(void)
{
  Disp2x16Msg(GetConstMsg(EDC_EMV_FATAL), MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  while (APM_WaitKey((DWORD)-1,0) != MWKEY_CANCL) SleepMW();
  ResetMW();
}
//*****************************************************************************
//  Function        : GetApplNameTag
//  Description     : Prepare app name tag if issuer code table is supported.
//  Input           : N/A.
//  Return          : tag.
//  Note            : Pack buffer must be setup before calling this func.
//  Globals Changed : N/A
//*****************************************************************************
WORD GetApplNameTag(void)
{
  TLIST * ptag;   // temp tag data ptr

  ptag = TagSeek(gGDS->s_TagList, 0x9F11);  // issuer code table
  if ( ptag != NULL )                // tag value is found
    if (*ptag->pbLoc==1)             // only supported these 1 issuer code tbl
      if (TagSeek(gGDS->s_TagList, 0x9F12)) // app preferred name exists
        return 0x9F12;               // app preferred name
  return 0x50;                       // app label
}
//*****************************************************************************
//  Function        : ePANExtract
//  Description     : Extract PAN data from EMV data.
//  Input           : N/A
//  Return          : FALSE => Error;
//  Note            : N/A
//  Globals Changed : gInput;
//*****************************************************************************
static BOOLEAN ePANExtract(void)
{
  TLIST *psLp;
  BYTE bLen;

  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, gGDS->s_EMVOut.wLen - 2)) {
    if ((psLp = TagSeek(gGDS->s_TagList, 0x5A)) != NULL) {
      if ((bLen = psLp->sLen) <= 10) {
        memset(INPUT.sb_pan, 0xFF, 10);
        memcpy(INPUT.sb_pan, psLp->pbLoc, bLen);
        //INPUT.sb_pan_len = bLen;
        if ((INPUT.sb_pan[9] & 0x0F) == 0x0F) {
          if (InCardTable()) {
            //if ((psLp = TagSeek(gGDS->s_TagList, 0x5F34)) != NULL)
            //  INPUT.sb_pan_seqno = psLp->pbLoc[0];
            //else
            //  INPUT.sb_pan_seqno = 0;
            return TRUE;
          }
        }
        RSP_DATA.w_rspcode = 'U'*256+'C';
        return FALSE;
      }
    }
  }
  EMVFatal();
  return FALSE;
}
//*****************************************************************************
//  Function        : ValidEMVData
//  Description     : Validate & Extract EMV Data
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : gGTS;
//*****************************************************************************
BOOLEAN ValidEMVData(void)
{
  TLIST * psLp;
  BYTE bLen;

  if (ePANExtract() == FALSE) {
    return FALSE;
  }

  memcpy(&INPUT.sb_exp_date, TagData(gGDS->s_TagList, 0x5F24), 2); //App. Exp. Date
  if ((psLp = TagSeek(gGDS->s_TagList, 0x5F20)) != NULL) {      //card holder name
    if ((bLen = psLp->sLen) > sizeof(INPUT.sb_holder_name))
      bLen = sizeof(INPUT.sb_holder_name);
    //FormatHolderName(INPUT.sb_holder_name, psLp->pbLoc, bLen);
    memcpy(INPUT.sb_holder_name, psLp->pbLoc, bLen);
  }

  INPUT.s_trk2buf.b_len = 0;
  if ((psLp = TagSeek(gGDS->s_TagList, 0x57)) != NULL) { /* make it compatable with swipe */
    split(INPUT.s_trk2buf.sb_content, psLp->pbLoc, bLen = psLp->sLen);
    INPUT.s_trk2buf.b_len = (BYTE) fndb(INPUT.s_trk2buf.sb_content, 'F', (BYTE)(bLen * 2));
    if (INPUT.s_trk2buf.b_len < (bLen*2) -1) {
      RSP_DATA.w_rspcode='U'*256+'C';
      RSP_DATA.b_response = TRANS_FAIL;
      return FALSE;
    }
  }

  memset(INPUT.s_icc_data.sb_label, ' ',sizeof(INPUT.s_icc_data.sb_label));
  psLp = TagSeek(gGDS->s_TagList, GetApplNameTag());
  if (psLp != NULL) {
    if ((bLen = psLp->sLen) > 16)
      bLen = 16;
    memcpy(INPUT.s_icc_data.sb_label, psLp->pbLoc, bLen);
  }

  ClearResponse();
  memcpy(&RSP_DATA.s_dtg.b_year, TagData(gGDS->s_TagList, 0x9A), 3);
  memcpy(&RSP_DATA.s_dtg.b_hour, TagData(gGDS->s_TagList, 0x9F21), 3);
  memset(RSP_DATA.sb_auth_code, ' ', sizeof(RSP_DATA.sb_auth_code));
  RSP_DATA.w_rspcode = '5'*256+'1';
  RSP_DATA.b_response = TRANS_FAIL;
  return TRUE;
}
//*****************************************************************************
//  Function        : emvPIN
//  Description     : Get EMV Online PIN.
//  Input           : N/A
//  Return          : FALSE => fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN emvPIN(void)
{
  #if 0  // !TODO
  if (edc_get_pin(INPUT.sb_pin,0)) {
    gGDS->s_EMVIn.wLen = 0;
    DispHeader(NULL);
    Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
    return TRUE;
  }
  #endif
  DispHeader(NULL);
  RSP_DATA.w_rspcode = 'C'*256+'N';
  return FALSE;
}
//*****************************************************************************
//  Function        : PackEMVData
//  Description     : Put EMV related data to  ICC DATA buffer
//  Input           : aDat;     // pointer to ICC DATA struct
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
void PackEMVData(struct ICC_DATA  *aDat)
{
  BYTE *ptr;

  aDat->w_misc_len = 0;
  ptr = PackTagsData(aDat->sb_misc_content, (WORD *)wVMOnlineTags);
  aDat->w_misc_len = (BYTE)((DWORD)ptr - (DWORD)aDat->sb_misc_content);
}
//*****************************************************************************
//  Function        : EMVOnline
//  Description     : Online Auth
//  Input           : N/A
//  Return          : FALSE: fail to go online, else TRUE
//  Note            : Zero length response => online failure
//  Globals Changed : N/A;
//*****************************************************************************
static BOOLEAN EMVOnline(void)
{
  static BYTE abResp[4]; //benny

  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, (WORD)(gGDS->s_EMVOut.wLen - 2)) == 0)
    EMVFatal();
  gGDS->s_EMVOut.wLen = 1;
  DispHeader(NULL);
  PackComm(INPUT.w_host_idx, FALSE);
  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK) {
    if (ReversalOK()) {
      OfflineSent(FALSE);
      memcpy(INPUT.sb_trace_no, TagData(gGDS->s_TagList, 0x9F41), 3); /* restore TSN */
      memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
      memcpy(INPUT.sb_roc_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));  // restore ROC
      IncAPMTraceNo();
      MoveInput2Tx();
      memcpy(TX_DATA.sb_pin, INPUT.sb_pin, sizeof(INPUT.sb_pin)); // online pin
      PackEMVData(&TX_DATA.s_icc_data);  // field 55 ICC data
      PackProcCode(TX_DATA.b_trans, TX_DATA.b_acc_ind);
      PackHostMsg();
      UpdateHostStatus(REV_PENDING);

      if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
        RSP_DATA.b_response = CheckHostRsp();
        if (RSP_DATA.b_response <= TRANS_REJ)
          UpdateHostStatus(NO_PENDING);
      }
      if (RSP_DATA.b_response <= TRANS_REJ) {
        UpdateHostStatus(NO_PENDING);
        memcpy(abResp, "\x8A\x02", 2);  //Authorisation Response Code, 2 bytes
        abResp[2] = (BYTE)(RSP_DATA.w_rspcode>>8);
        abResp[3] = (BYTE)(RSP_DATA.w_rspcode&0xFF);
        memcpy(gGDS->s_EMVIn.bMsg+1, abResp, sizeof(abResp));
        gGDS->s_EMVIn.wLen = sizeof(abResp) + 1;
        if (RSP_DATA.b_response == TRANS_ACP) {
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen, "\x89\x06", 2); //Auth. Code, 6 bytes
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen+2, RSP_DATA.sb_auth_code, 6);
          gGDS->s_EMVIn.wLen += 8;
        }
        //send issuer script and authentication data to EMV module
        if (RSP_DATA.s_icc_data.w_misc_len) {
          memcpy(gGDS->s_EMVIn.bMsg+gGDS->s_EMVIn.wLen, RSP_DATA.s_icc_data.sb_misc_content, RSP_DATA.s_icc_data.w_misc_len);
          gGDS->s_EMVIn.wLen += RSP_DATA.s_icc_data.w_misc_len;
        }
      }
    }
    APM_ResetComm();
    return TRUE;
  }
  return FALSE; // unable to go online
}
//*****************************************************************************
//  Function        : EMVReferral
//  Description     : Voice Referral
//  Input           : N/A
//  Return          : FALSE => ERROR
//  Note            : Zero length response => decline
//  Globals Changed : N/A
//******************************************************************************
static BOOLEAN EMVReferral(void)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];

  PackRspText();
  memcpy(tmpbuf, &RSP_DATA.text[1], 16);
  tmpbuf[16]=0;
  DispLineMW(tmpbuf, MW_LINE1, MW_CENTER|MW_BIGFONT);

  APM_ResetComm();
  RSP_DATA.w_rspcode = 'C'*256+'N';
  RSP_DATA.text[0] = 0;

  if (GetAuthCode()) {
    if (INPUT.b_trans >= SALE_SWIPE) {
      INPUT.b_trans = ESALE_COMPLETE;
      INPUT.b_trans_status = OFFLINE;
    }
    DispHeader(STIS_ISS_TBL(0).sb_card_name);
    RSP_DATA.b_response = TRANS_ACP;
  }
  else
    DispHeader(STIS_ISS_TBL(0).sb_card_name);

  if (RSP_DATA.b_response != TRANS_ACP){
    LongBeep();
    return FALSE;
  }
  PackDTGAA();
  return TRUE;
}
//*****************************************************************************
//  Function        : EMVAccepted
//  Description     : Store EMV Data into Batch.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gds.emvdll, input;
//*****************************************************************************
static void EMVAccepted(BOOLEAN aOnlTxn)
{
  BYTE *tag_ptr;
  TLIST * psLp;
  BYTE  *ptr;

  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
  memcpy(INPUT.sb_trace_no, TagData(gGDS->s_TagList, 0x9F41), 3);  // restore TSN
  // mark as offline for offline approved
  if (!(bAction & ACT_HOSTAPP)){  // offline approved
    if (INPUT.b_trans==SALE_ICC)
      INPUT.b_trans = ESALE_UNDER_LMT;
  }

  RSP_DATA.b_response = TRANS_ACP;
  tag_ptr = TagData(gGDS->s_TagList, 0x89); // locate auth code
  if (bAcceptReferral == TRUE) {
    if (tag_ptr!=NULL)
      memcpy(tag_ptr, RSP_DATA.sb_auth_code, 6); // update auth code
  }
  else {
    if (tag_ptr!=NULL)
      memcpy(RSP_DATA.sb_auth_code, tag_ptr, 6); // store Auth Code
    else
      GenAuthCode(RSP_DATA.sb_auth_code);
    if (!aOnlTxn) {
      memset(RSP_DATA.sb_auth_code, ' ', 6);
      memcpy(RSP_DATA.sb_auth_code, "Y1", 2);
      IncAPMTraceNo();
      memcpy(TX_DATA.sb_trace_no, INPUT.sb_trace_no, sizeof(TX_DATA.sb_trace_no));
    }
  }


  if ((psLp = TagSeek(gGDS->s_TagList, 0x9f26)) != NULL) // app Cryptogram
    memcpy(INPUT.s_icc_data.sb_tag_9f26, psLp->pbLoc, psLp->sLen);
  if ((psLp = TagSeek(gGDS->s_TagList, 0x9F06)) != NULL) { //AID for printing
    memcpy(INPUT.s_icc_data.sb_aid, psLp->pbLoc, psLp->sLen);
    INPUT.s_icc_data.b_aid_len = psLp->sLen;
  }

  SetRspCode('0'*256+'0');
  PackEMVData(&INPUT.s_icc_data);  // field 55 ICC data
  if (bAction & ACT_SIGNATURE)
    RSP_DATA.w_rspcode = 'V'*256+'S';
  else {
    RSP_DATA.w_rspcode = 'T'*256+'A';
    // !2007-05-23++
    // Check for PIN Verified
    if ((ptr = TagData(gGDS->s_TagList, 0x9F10)) != NULL) {
      if (ptr[4]&0x04)
        RSP_DATA.b_PinVerified = 1;
    }
    // !2007-05-23--
  }

  PackInputP();
  MoveInput2Tx();
  if (INPUT.b_trans != AUTH_ICC){
    if (bAcceptReferral == TRUE) {
      IncTraceNo();
    }
    SaveRecord();
    if ((INPUT.b_trans==ESALE_UNDER_LMT)||((INPUT.b_trans==ESALE_COMPLETE)&&(bAcceptReferral!=TRUE))){
      if (BlockAuthCode())
        memset(RSP_DATA.sb_auth_code, ' ', sizeof(RSP_DATA.sb_auth_code));
    }
  }
  else {  // fill record buf even if not storing for ECR resp
    Input2RecordBuf();
  }

  /*
  if (bAcceptReferral==TRUE){
    AcceptBeep();
    #if (LPT_SUPPORT|TMLPT_SUPPORT)  // Conditional Compile for Printer support
    PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
    #endif                    // PRINTER_SUPPORT
    MsgBufSetup();
    PackMsgBufLen();
  }
  */
}
//*****************************************************************************
//  Function        : EMVCancel
//  Description     : Handle EMV Cancel response
//  Input           : EMV process next state.
//  Return          : EMV process next state.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE EMVCancel(BYTE aNextState)
{
  DispClrBelowMW(MW_LINE3);  // clr Pls Select display
  RSP_DATA.w_rspcode = 'C'*256+'N';
  LongBeep();
  if (gGDS->s_EMVOut.wLen > 2)
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(REV_PENDING);
  if (IOCtlMW(gIccHandle, IO_ICC_CHANGE, NULL))
    aNextState = EMV_COMPLETE;  // no fallback

  RSP_DATA.b_response = TRANS_REJ; // for print msg disable
  return aNextState;
}
//*****************************************************************************
//  Function        : EMVError
//  Description     : Handle EMV error response
//  Input           : EMV process next state.
//  Return          : EMV process next state.
//  Note            : N/A
//  Globals Changed : N/A;
//*****************************************************************************
static BYTE EMVError(BYTE aNextState)
{
  DispHeader(NULL);
  switch (gGDS->s_EMVOut.bMsg[0] & 0x7F) {  //short error message response
    case ERR_SMCIO:
    case ERR_CARDL1:
    case ERR_DATA:
      RSP_DATA.w_rspcode = 'R'*256+'E';
      break;
    case ERR_CANCEL:
      DispClrBelowMW(MW_LINE3);
      RSP_DATA.w_rspcode = 'C'*256+'N';
      break;
    case ERR_SEQ:
      RSP_DATA.w_rspcode = 'S'*256+'C';
      break;
    case ERR_NOMORE:
      RSP_DATA.w_rspcode = 'U'*256+'C';
      break;
    case ERR_NOAPPL:  // not supported if no matching app
      RSP_DATA.w_rspcode = 'M'*256+'A'; // fallback
      break;
    case ERR_BLOCKED:
      RSP_DATA.w_rspcode = 'S'*256+'B';
      break;
    case ERR_CONFIG:
    case ERR_MEMORY:
    default:
      RSP_DATA.w_rspcode = 'S'*256+'E';
      IOCtlMW(gIccHandle, IO_ICC_OFF, NULL);
      break;
  }
  LongBeep();
  if (gGDS->s_EMVOut.wLen > 2)
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(REV_PENDING);
  if (IOCtlMW(gIccHandle, IO_ICC_CHANGE, NULL))
    aNextState = EMV_COMPLETE;  // no fallback

  RSP_DATA.b_response = TRANS_REJ; // for print msg disable
  return aNextState;
}
//*****************************************************************************
//  Function        : EMVComplete
//  Description     : Handle EMV COMPLETE response
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void EMVComplete(void)
{
  IOCtlMW(gIccHandle, IO_ICC_OFF, NULL);
  if (DeAss(gGDS->s_TagList, gGDS->s_EMVOut.bMsg+2, (WORD)(gGDS->s_EMVOut.wLen - 2)) == 0)
    EMVFatal();
  DispHeader(NULL);

  if (bAcceptReferral==TRUE){ // referral requested
    if (!EMVReferral())
      return;
    bAction |= ACT_APPROVED;  // so it will print & update batch
  }

  if (bAction & ACT_APPROVED) {
    EMVAccepted((BOOLEAN)(bAction & ACT_ONLINE)); // 07-06-05++ BW (1) for offline approved txn
    //RSP_DATA.w_rspcode = 'T'*256+'A';
    if (bAction & ACT_HOSTAPP)
      UpdateHostStatus(NO_PENDING); // clear reversal gen after host appr txn
    AcceptBeep();
  }
  else {
    if ((bAction&ACT_HOSTAPP) || (RSP_DATA.w_rspcode=='0'*256+'0'))
      RSP_DATA.w_rspcode = '5'*256+'1'; //rejected by ICC -> set rsp. ocde to declined
    LongBeep();
  }
}
//*****************************************************************************
//  Function        : EMVProcess
//  Description     : EMV Processing state machine.
//  Input           : N/A
//  Return          : next action
//  Note            : N/A
//  Globals Changed :
//*****************************************************************************
BYTE EMVProcess(void)
{
  BYTE next_state;
  BYTE tmp[10];
  BOOLEAN again = FALSE;

  // init vars
  memset(INPUT.sb_pin, 0, sizeof(INPUT.sb_pin));
  memset(INPUT.sb_ecr_ref, ' ', 16);
  bAcceptReferral = FALSE;
  next_state = EMV_START;
  if (INPUT.b_trans == SALE_ICC)
    gGDS->s_EMVIn.bMsg[0] = EGOOD_SALE;
  else
    gGDS->s_EMVIn.bMsg[0] = EAUTH;
  gGDS->s_EMVIn.wLen    = 1;

  do {
    //if (next_state != EMV_START) {
    if (EMV_DllMsg(next_state, &gGDS->s_EMVIn, &gGDS->s_EMVOut) == FALSE) {
      next_state=EMVError(next_state);
      return next_state;
    }
    next_state = gGDS->s_EMVOut.bMsg[0];
    //}
//    else {
//      next_state = gGDS->s_EMVOut.bMsg[0];
//    }
    switch (gGDS->s_EMVOut.bMsg[0]&0x7F) {
      case EMV_COMPLETE: /* completed */
        EMVComplete();
        return EMV_COMPLETE;
      case EMV_SELECT_APP: // select application
        if (EMVInput(again) == FALSE)
          return EMVCancel(next_state);
        again = TRUE;
        break;
      case EMV_AMOUNT_AIP: // IAP need amount
      case EMV_AMOUNT: // must have amount by now
        if (GetAmount(TIPsReqd()) == FALSE)
          return EMVCancel(next_state);
        DispHeader(NULL);
        memcpy(gGDS->s_EMVIn.bMsg+1, "\x9F\x02\x06", 3);
        dbin2bcd(tmp, INPUT.dd_amount);
        //dbin2bcd(gGDS->s_EMVIn.bMsg+4, INPUT.dd_amount);
        memcpy(gGDS->s_EMVIn.bMsg+4, &tmp[4], 6);
        gGDS->s_EMVIn.wLen = 10;
        gGDS->s_EMVIn.bMsg[0] = 0;
        if (!GetProductCode())
          return EMVCancel(next_state);
        DispHeader(NULL);
        Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING), MW_LINE5, MW_BIGFONT);
        break;
      case EMV_CONFIRM_CARD: /* validate application */
        if (!ValidEMVData()) {
          Disp2x16Msg(GetConstMsg(EDC_RSP_READ_ERROR), MW_LINE5, MW_BIGFONT);
          return FALSE;
        }

        //  exit from here for these txn
        if ( (INPUT.b_trans==REFUND)
             || (INPUT.b_trans==SALE_OFFLINE) ){
          RSP_DATA.w_rspcode = '0'*256+'0';
          return EMV_CONFIRM_CARD;
        }

        PackComm(INPUT.w_host_idx, FALSE);
        APM_PreConnect();

        if (!ConfirmCard()) {  // leave exp date chk to kernel
          RSP_DATA.w_rspcode = 'C'*256+'N';
          return EMV_COMPLETE; // 25-04-05++ txn end
        }

        if (INPUT.b_trans == AUTH_ICC) {// AUTH always online
          gGDS->s_EMVIn.bMsg[0] = 0x88;     // Override transaction type & force online
          gGDS->s_EMVIn.wLen = 1;
        }
        else {
          gGDS->s_EMVIn.bMsg[0] = 0;
          gGDS->s_EMVIn.wLen = 0;
        }
        break;
      case EMV_ONLINE_PIN: // online PIN
        if (emvPIN() == FALSE) {
          LongBeep();
          return EMV_ONLINE_PIN; // 25-04-05++ return next action code
        }
        break;
      case EMV_ONLINE: // online process
        // only proceed if unable to go online or txn approved
        if ( (EMVOnline() == FALSE)) {
          gGDS->s_EMVIn.bMsg[0] = 0;
          gGDS->s_EMVIn.wLen = 0;
          return EMV_ONLINE;
        }
        break;
      case EMV_REFERRAL: // referral
        gGDS->s_EMVIn.bMsg[0] = 0;
        gGDS->s_EMVIn.wLen = 1;   // request AAC
        bAcceptReferral = TRUE; //referral requested indicator
        break;
      default:
        EMVFatal();
    }
  } while (TRUE);
}
//*****************************************************************************
//  Function        : EMVTrans
//  Description     : EMV trans handler.
//  Input           : trans;    // trans type
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
DWORD EMVTrans(DWORD aTrans)
{
  INPUT.b_trans = (BYTE) aTrans;
  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (BlockLocalTrans()) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  if (INPUT.b_trans==AUTH_ICC) {
    if (AuthBlocked()) {
      DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
      return FALSE;
    }
  }

  if (HostSettlePending() || APM_BatchFull(INPUT.w_host_idx)) {
    DispErrorMsg(GetConstMsg(EDC_TU_CLOSE_BATCH));
    return FALSE;
  }

  INPUT.b_entry_mode = ICC;
  RSP_DATA.b_response = TRANS_FAIL;
  RSP_DATA.w_rspcode = '5'*256+'1';

  DispHeader(NULL); // show correct header

  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(GetConstMsg(EDC_EMV_PROCESSING),MW_LINE5, MW_BIGFONT);
  Short1Beep();
  // no fallback if get processing option step performed
  EMVProcess();

  if (RSP_DATA.w_rspcode != '0'*256+'0')
    TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  return TRUE;
}
