//-----------------------------------------------------------------------------
//  File          : tranutil.c
//  Module        :
//  Description   : Utilities function for transactions.
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
//  10 May  2006  Lewis       Initial Version for new CONTROL
//  15 Jan  2009  Lewis       Bug Fixed.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "corevar.h"
#include "chkoptn.h"
#include "constant.h"
#include "input.h"
#include "message.h"
#include "print.h"
#include "util.h"
#include "record.h"
#include "ecr.h"
#include "lptutil.h"
#include "tranutil.h"
#include "rs232.h"
#include "keytrans.h"
#include "key2dll.h"
#include "coremain.h"

//-----------------------------------------------------------------------------
//      Globals Variables
//-----------------------------------------------------------------------------
struct TRANS_DATA gGTS;

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
#define TYPE_VISA1       0x00
#define TYPE_SDLC        0x01
#define TYPE_ASYN        0x02
#define TYPE_VOICE       0x03

#define MODE_BELL        0x00
#define MODE_CCITT       0x01

#define BPS_300          0x00
#define BPS_1200         0x01
#define BPS_2400         0x02
#define BPS_9600         0x03
#define BPS_AUTO         0x04

static const struct {
  BYTE b_protocol;
  BYTE b_mode;
  BYTE b_speed;
} KCommParam[8] = {
  {TYPE_SDLC, MODE_BELL,  BPS_1200},
  {TYPE_ASYN, MODE_BELL,  BPS_1200},
  {TYPE_SDLC, MODE_CCITT, BPS_1200},
  {TYPE_ASYN, MODE_CCITT, BPS_1200},
  {TYPE_ASYN, MODE_BELL,  BPS_300},
  {TYPE_ASYN, MODE_CCITT, BPS_300},
  {TYPE_SDLC, MODE_CCITT, BPS_2400},
  {TYPE_ASYN, MODE_CCITT, BPS_2400}
};

static const struct MDMSREG KDefaultSReg =  {
  sizeof(struct MW_MDM_PORT),                 // len
  {
    0x01,                                     // Protocol = SDLC
    0x01,                                     // Modem Type = CCITT
    0x01,                                     // Speed = 1200
    12,                                       // Inactivity Timeout in unit of 2.5 sec, total 30sec
    15,                                       // Redial Pause time in 100 msec, total 1.5secs */
    0x00,                                     // Primary Tel Len
    {'F','F','F','F','F','F','F','F','F','F', // Primary Phone #
      'F','F','F','F','F','F','F','F','F','F',
      'F','F','F','F','F','F','F','F','F','F',
      'F','F','F','F'},
    0x03,                                     // Primary Dial Attempts
    20,                                       // Primary Connection Time
    0x00,                                     // Secondary Tel Len
    {'F','F','F','F','F','F','F','F','F','F', // Secondary Phone #
     'F','F','F','F','F','F','F','F','F','F',
     'F','F','F','F','F','F','F','F','F','F',
     'F','F','F','F'},
    0x03,                                     // Secondary Dial Attempts
    20,                                       // Secondary Connection Time
    3,                                        // CD on qualify time
    4,                                        // CD off qualify time
    10,                                       // Async Message qualify time in 10msec, total 100ms
    15,                                       // Wait Online time in sec, total 15 secs */
    10,                                       // (DTMF) tone duration in 10msec, total 100ms */
    5,                                        // hold line time in minutes */
    MW_SHORT_DIALTONE,                        // busy tone + check line + blink dialing + short dial tone */
  }
};

//*****************************************************************************
//  Function        : OverMargin
//  Description     : Check for over adjustment margin.
//  Input           : N/A
//  Return          : TRUE;     // over limit;
//                    FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN OverMargin(void)
{
  DDWORD amount;

  if (STIS_ACQ_TBL(0).sb_reserved[2] == 0) /* zero for no limit */
    return(FALSE);

  amount  = INPUT.dd_amount * bcd2val(STIS_ACQ_TBL(0).sb_reserved[2]);
  amount /= 100;
  return (INPUT.dd_tip > amount);
}
//*****************************************************************************
//  Function        : PackDTGAA
//  Description     : Pack date/time & accept response code.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackDTGAA(void)
{
  ReadRTC(&RSP_DATA.s_dtg);
  RSP_DATA.w_rspcode = 'A'*256+'P';
  memset(RSP_DATA.sb_rrn, ' ', 12);
}
//*****************************************************************************
//  Function        : PackDateTime
//  Description     : Pack DATE/TIME on receipt.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackDateTime(struct DATETIME *aDtg)
{
  BYTE tmp[32];

  ConvDateTime(tmp, aDtg, 1);
  pack_mem("Date/Time:", 10);
  pack_space(1);
  pack_mem(tmp, 12);
  pack_space(1);
  if (PrintTimeReqd())
    pack_mem(&tmp[12], 6);
  pack_lf();
}
//*****************************************************************************
//  Function        : PackContent
//  Description     : Pack receipt contents.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackContent(void)
{
  DDWORD no_tips = -1;
  BYTE trans, offset;
  BYTE tmp[32];
  struct DISP_AMT disp_amt;
  struct DESC_TBL prod;

  //PackLogo();             // Header line
  //pack_lf();
  PackRcptHeader();

  // Card Name
  pack_space(9);
  pack_mem(STIS_ISS_TBL(0).sb_card_name, 10);
  pack_lf();

  // PAN
  split(tmp, P_DATA.sb_pan, 10);
  if (MaskCardNo()) 
    memset(&tmp[6],'X', 6);     // mask PAN
  offset = fndb(tmp, 'F', 19);
  pack_mem(tmp, offset);
  pack_byte('(');
  pack_byte(P_DATA.b_entry_mode);
  pack_byte(')');
  pack_lf();

  // Card Holder Name
  pack_mem(P_DATA.sb_holder_name, 26);
  pack_lf();

  // Trans Name & Expiry date
  trans = (P_DATA.b_trans_status & VOIDED) ? VOID : P_DATA.b_trans;
  memcpy(tmp, GetConstMsg(KTransHeader[trans]),16);
  // locate start pos of English transaction name
  for (offset = 4; offset < 13; offset++) {
    if (tmp[offset]!=' ')
      break;
  }
  pack_mem((unsigned char *)tmp,4);
  pack_mem((unsigned char *)&tmp[offset], (BYTE)(16 - offset));
  if (P_DATA.sb_exp_date[0] != 0) {
    pack_space(MAX_CHAR_LINE_NORMAL - 29 + offset);
    pack_mem("Exp:", 4);
    split_data((BYTE *)&P_DATA.sb_exp_date[1], 1);
    pack_byte('/');
    split_data((BYTE *)&P_DATA.sb_exp_date[0], 1);
  }
  pack_lf();

  // Batch & Trace
  pack_mem("Batch:", 6);
  split_data(STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
  pack_space(MAX_CHAR_LINE_NORMAL - 24);
  pack_mem("Trace:", 6);
  split_data(P_DATA.sb_trace_no, 3);
  pack_lf();
  PackDateTime(&P_DATA.s_dtg);

  // RRN & APPV CODE
  pack_mem("RRN:", 4);
  pack_mem(P_DATA.sb_rrn, 12);
  pack_space(MAX_CHAR_LINE_NORMAL-27);
  pack_mem("APPV:", 5);
  pack_mem(P_DATA.sb_auth_code, 6);

  // Product descriptor
  offset = 0;
  while ((offset < APM_GetDescCount()) && (P_DATA.sb_product[offset] != 0xFF)) {
    APM_GetDescTbl(P_DATA.sb_product[offset], &prod);
    pack_lf();
    pack_mem((BYTE *)&prod.sb_text, 20);
    offset += 1;
  }
  for (; offset< 4; offset++)
    pack_lf();

  // Amount
  if (P_DATA.dd_tip != no_tips) {
    pack_mem("   BASE",7);
    pack_space(4);
    PackAmt(P_DATA.dd_base_amount, MAX_CHAR_LINE_NORMAL-11);
    pack_lf();
    pack_mem("    TIP", 7);
    if (P_DATA.dd_tip) {
      pack_space(4);
      PackAmt(P_DATA.dd_tip, MAX_CHAR_LINE_NORMAL-11);
    }
    else {
      pack_space(4);
      pack_mem(STIS_TERM_CFG.sb_currency_name, 3);
    }
    pack_lf();
    pack_space(MAX_CHAR_LINE_NORMAL-13);
    pack_nbyte('-', 13);
    pack_lf();
  }

  pack_mem("TOTAL", 5);
  pack_space(MAX_CHAR_LINE_NORMAL-30);
  if ((P_DATA.dd_tip == 0) && ((P_DATA.b_trans_status&VOIDED) == 0)) {
    pack_mem(STIS_TERM_CFG.sb_currency_name, 3);
  }
  else {
    ConvAmount(P_DATA.dd_amount, &disp_amt, STIS_TERM_CFG.b_decimal_pos, STIS_TERM_CFG.b_currency);
    pack_space((BYTE)(22 - disp_amt.len));
    pack_mem(STIS_TERM_CFG.sb_currency_name, 3);
    pack_mem(disp_amt.content, disp_amt.len);
  }
  pack_lf();
  pack_space(MAX_CHAR_LINE_NORMAL-19);
  pack_nbyte('=', 19);
  pack_lf();

  // EMV Trans data
  if (P_DATA.b_entry_mode == ICC) {
    pack_mem("AID    : ", 9);
    split_data(P_DATA.s_icc_data.sb_aid, P_DATA.s_icc_data.b_aid_len); // !2009-01-15
    pack_lf();
  }
  pack_lfs(2);

  #if (TMLPT_SUPPORT)
  if ((RSP_DATA.w_rspcode == 'V'*256+'S')||(P_DATA.b_entry_mode!=ICC)) {
    pack_lfs(4);
    pack_nbyte('_', MAX_CHAR_LINE_NORMAL-2);
    pack_lf();
    pack_mem("X CardHolder Signature", 22);
  }
  #endif
}
//*****************************************************************************
//  Function        : PackTxnSlip
//  Description     : Ready transaction receipts.
//  Input           : aReprint;     // TRUE => pack Reprint Line
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackTxnSlip(BOOLEAN aReprint)
{
  if (!PrintReqd())
    return;

  MsgBufSetup();
  PackContent();
  if (aReprint) {
    pack_lf();
    pack_mem("\x1BW1****REPRINT****\x1BW0", 21);
  }
  PackFF();
  PackMsgBufLen();
  if (aReprint == FALSE)
    SaveLastRcpt(MSG_BUF.sb_content, MSG_BUF.d_len);
}
//*****************************************************************************
//  Function        : PackRecordP
//  Description     : Pack data from RECORD_BUF to p_data & ready receipt.
//  Input           : aReprint;     // TRUE=>for reprint.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : p_data;
//*****************************************************************************
void PackRecordP(BOOLEAN aReprint)
{
  DDWORD no_tips = -1;

  ByteCopy((BYTE *)&P_DATA, (BYTE *)&RECORD_BUF, (BYTE *)&P_DATA.b_type-(BYTE *)&P_DATA.b_trans);
  ByteCopy((BYTE *)&P_DATA.s_icc_data, (BYTE *)&RECORD_BUF.s_icc_data, sizeof(struct ICC_DATA));
  P_DATA.dd_base_amount=P_DATA.dd_amount;
  RSP_DATA.w_rspcode = 'A'*256+'P';
  // !2007-05-23++
  P_DATA.w_rspcode = RECORD_BUF.w_rspcode;
  P_DATA.b_PinVerified = RECORD_BUF.b_PinVerified;
  // !2007-05-23--

  if (TIPsReqd() && SaleType(RECORD_BUF.b_trans))
    P_DATA.dd_base_amount -= P_DATA.dd_tip;
  else
    P_DATA.dd_tip = no_tips;

  PackTxnSlip(aReprint);
}
//*****************************************************************************
//  Function        : PackInputP
//  Description     : Pack data from input to p_data & ready receipt.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : p_data;
//*****************************************************************************
void PackInputP(void)
{
  DDWORD no_tips = -1;

  ByteCopy((BYTE *)&P_DATA.b_trans, (BYTE *)&INPUT.b_trans, (DWORD)((DWORD)&P_DATA.b_type-(DWORD)&P_DATA.b_trans));
  ByteCopy((BYTE *)&P_DATA.s_icc_data, (BYTE *)&INPUT.s_icc_data, sizeof(struct ICC_DATA));
  ByteCopy((BYTE *)&P_DATA.s_dtg, (BYTE *)&RSP_DATA.s_dtg, sizeof(struct DATETIME));
  memcpy(P_DATA.sb_rrn, &RSP_DATA.sb_rrn, sizeof(P_DATA.sb_rrn));
  memcpy(P_DATA.sb_auth_code, &RSP_DATA.sb_auth_code, sizeof(P_DATA.sb_auth_code));
  P_DATA.w_rspcode = RSP_DATA.w_rspcode;
  P_DATA.b_PinVerified = RSP_DATA.b_PinVerified;
  P_DATA.b_type = CUS_SLIP;
  P_DATA.dd_base_amount = INPUT.dd_amount; /* base amount */
  if (TIPsReqd() && SaleType(INPUT.b_trans)) {
    P_DATA.dd_tip = INPUT.dd_tip;
    P_DATA.dd_base_amount -= INPUT.dd_tip;
  }
  else
    P_DATA.dd_tip = no_tips;

  PackTxnSlip(FALSE);
}
//*****************************************************************************
//  Function        : DispHeader
//  Description     : Display transaction header.
//  Input           : aCardName;  // pointer to card name
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispHeader(BYTE *aCardName)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];

  DispLineMW(GetConstMsg(KTransHeader[INPUT.b_trans]), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);

  if (aCardName != NULL) {
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, aCardName, 10);
    DispLineMW(tmpbuf, MW_LINE3, MW_BIGFONT);
  }
}
//*****************************************************************************
//  Function        : SetRspCode
//  Description     : Update the rspdata.w_rspcode with INPUT.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : hst_rsp_data;
//*****************************************************************************
void SetRspCode(WORD aRspCode)
{
  RSP_DATA.w_rspcode = aRspCode;
}
//*****************************************************************************
//  Function        : ClearResponse
//  Description     : Clear host gGTS.b_response data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : hst_rsp_data;
//*****************************************************************************
void ClearResponse(void)
{
  RSP_DATA.text[0] = 0;
  memset(&RSP_DATA.text[1], ' ', 69);
  memset(RSP_DATA.sb_rrn, ' ', 12);
  RSP_DATA.b_response = TRANS_FAIL;
  RSP_DATA.w_rspcode = 'N'*256+'C';
}
//*****************************************************************************
//  Function        : ConfirmCard
//  Description     : Prompt user to comfirm card data.
//  Input           : N/A
//  Return          : TRUE;   // confirmed
//                    FALSE;  // CANCEL
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ConfirmCard(void)
{
  BYTE buffer[4];
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  DWORD i, keyin;
  if ((INPUT.b_entry_mode == MANUAL) || !DispMSRReqd())
    return TRUE;

  DispClrBelowMW(MW_LINE3);
  memset(tmpbuf, 0, sizeof(tmpbuf));
  memcpy(tmpbuf, STIS_ISS_TBL(0).sb_card_name,10);
  DispLineMW(tmpbuf, MW_LINE4, MW_SMFONT);

  split(&tmpbuf[1],INPUT.sb_pan, 10);
  tmpbuf[0] = (BYTE) fndb(&tmpbuf[1], 'F', 19);
  tmpbuf[1+tmpbuf[0]] = 0;
  DispLineMW(&tmpbuf[1], MW_LINE5, MW_SPFONT);

  if (ExpDateReqd()) {
    split(buffer, INPUT.sb_exp_date, 2);
    sprintf(tmpbuf, "EXP DATE : %c%c/%c%c", buffer[2], buffer[3], buffer[0], buffer[1]);
    DispLineMW(tmpbuf, MW_LINE6, MW_SMFONT);
  }

  for (i = 20; i ; i--) {
    if (INPUT.sb_holder_name[i] != ' ')
      break;
  }
  memset(tmpbuf, 0, sizeof(tmpbuf));
  memcpy(tmpbuf, INPUT.sb_holder_name, (BYTE) (i + 1));
  DispLineMW(tmpbuf, MW_LINE7, MW_BIGFONT);

  do {
    keyin=APM_WaitKey(KBD_TIMEOUT,0);
    if (keyin == MWKEY_CANCL)
      return(FALSE);
  } while (keyin != MWKEY_ENTER);


  return(TRUE);
}
//*****************************************************************************
//  Function        : TransAllowed
//  Description     : Check Whether Transaction is allowed
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TransAllowed(DWORD aInputMode)
{
  BOOLEAN tips_req;

  if (STIS_TERM_DATA.b_stis_mode < TRANS_MODE) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }

  switch (INPUT.b_trans) {
    case AUTH_SWIPE:
    case AUTH_MANUAL:
      if (AuthBlocked()) {
        DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
        return FALSE;
      }
      break;
    case OFFLINE:
      if (!OfflineAllowed()) {
        DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
        return FALSE;
      }
      break;
    case REFUND:
      if (RefundBlocked()) {
        DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
        return FALSE;
      }
      break;
  }

  if (BlockLocalTrans()) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }
  if ((aInputMode == MANUAL) &&
      (BlockLocalManual()||!ManualEntryAllow()) ) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }
  if ((aInputMode == SWIPE)&&BlockLocalSwipe()) {
    DispErrorMsg(GetConstMsg(EDC_TU_NOT_ALLOW));
    return FALSE;
  }
  if (HostSettlePending() || APM_BatchFull(INPUT.w_host_idx)) {
    DispErrorMsg(GetConstMsg(EDC_TU_CLOSE_BATCH));
    return FALSE;
  }

  if (INPUT.b_trans != SALE_OFFLINE) {
    PackComm(INPUT.w_host_idx, FALSE);
    APM_PreConnect();
  }

  while (TRUE) {
    if (INPUT.b_entry_mode != MANUAL)
      Short1Beep();

    if (!ConfirmCard())
      break;

    if (!GetExpDate())
      break;

    //if ((INPUT.b_entry_mode == MANUAL) && ManualPSWReqd()) {
    //  if (A21_PSWGet(NULL, P_MANUAL) != PASSWORD_OK)
    //    break;
    //  DispHeader(NULL);
    //}

    if (INPUT.b_trans == REFUND)
      tips_req = 0;
    else
      tips_req = TIPsReqd();

    if (!GetAmount(tips_req))
      break;

    if (INPUT.b_trans != AUTH_SWIPE) {
      if (!GetProductCode())
        break;
      DispHeader(NULL);
    }
    else
      memset(INPUT.sb_product, 0xFF, sizeof(INPUT.sb_product));

    if ((Prompt4DBC())&&
        (INPUT.b_trans != SALE_OFFLINE)) {
      if (!Get4DBC())
        break;
    }
    else
      memset(INPUT.sb_amex_4DBC, 0x00, sizeof(INPUT.sb_amex_4DBC));

    GetEcrRef();

    DispHeader(NULL);
    return TRUE;
  }

  APM_ResetComm();
  return FALSE;
}
//*****************************************************************************
//  Function        : PackRspText
//  Description     : Update response text base on rspcode.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackRspText(void)
{
  BYTE var_i;
  BYTE *pMsg;

  if (RSP_DATA.text[0] != 0)
    return;

  var_i = 0;
  while ((KRspText[var_i].w_idx != RSP_DATA.w_rspcode) &&
         (KRspText[var_i].w_idx != '*'*256+'*'))
    var_i += 1;

  memset(&RSP_DATA.text[1], ' ', 40);

  pMsg = GetConstMsg(KRspText[var_i].d_text_id);
  memcpy(&RSP_DATA.text[1], pMsg, 16);
  memcpy(&RSP_DATA.text[21],&pMsg[16], 16);
  if (RSP_DATA.w_rspcode == 'A'*256+'P')
    memcpy(&RSP_DATA.text[31], RSP_DATA.sb_auth_code, 6);
  else if (RSP_DATA.w_rspcode == '0'*256+'2')
    memcpy(&RSP_DATA.text[11], RSP_DATA.sb_auth_code, 6);
  RSP_DATA.text[0] = 40;
}
//*****************************************************************************
//  Function        : DispRspText
//  Description     : Display Response text.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispRspText(BOOLEAN aDispHdr)
{
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  PackRspText();
  DispClrBelowMW(MW_LINE3);
  if (aDispHdr)
    DispHeader(STIS_ISS_TBL(0).sb_card_name);
  memset(tmpbuf, 0, sizeof(tmpbuf));
  memcpy(tmpbuf, &RSP_DATA.text[1], 16);
  DispLineMW(tmpbuf, MW_LINE5, MW_CENTER|MW_BIGFONT);
  memcpy(tmpbuf, &RSP_DATA.text[21], 16);
  DispLineMW(tmpbuf, MW_LINE7, MW_CENTER|MW_BIGFONT);
}
//*****************************************************************************
//  Function        : DispErrorMsg
//  Description     : Display Error Message on MW_LLINE5 and pause 3 sec.
//  Input           : aMsg;     // pointer to constant message.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispErrorMsg(const BYTE *aMsg)
{
  DispClrBelowMW(MW_LINE3);
  Disp2x16Msg(aMsg, MW_LINE5, MW_BIGFONT);
  Delay1Sec(2, 1);
}





//*****************************************************************************
//  Function        : TransEnd
//  Description     : Transaction end process.
//  Input           : aShowCard;  // Show card name when TRUE
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TransEnd(BOOLEAN aShowCard)
{

	//printf("\fSalio de CheckHostRsp");WaitKey(3000, 0);
	if (RSP_DATA.b_response == TRANS_ACP)
		AcceptBeep();
	else
		LongBeep();



  APM_ResetComm();
  //#if (LPT_SUPPORT|TMLPT_SUPPORT)    // Conditional Compile for Printer support
  PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
  //#endif                    // PRINTER_SUPPORT
  //DispRspText(aShowCard);

  EcrResp();
  APM_ClearKeyin();  // flush key to avoid display refresh caused by key event
  MsgBufSetup();
  PackMsgBufLen();
  fCommitAllMW();
 /* #if (T800)
  os_disp_bl_control(10);  // off disp after 10 seconds
  #endif*/
  RefreshDispAfter(5);
}
//*****************************************************************************
//  Function        : DispAmount
//  Description     : Display the formated amount line_nbr. (right justify)
//  Input           : aAmount;    // DDWORD amount
//                    aLineNo;    // aLineNo & 0xF0 is the line number
//                                // to be displayed.
//                                // Bit 0 : 1 - minus sign needed
//                                // Bit 1 : 1 - OVERWRITE
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispAmount(DDWORD aAmount, DWORD aLineNo, DWORD aFont)
{
  struct DISP_AMT disp_amt;
  BOOLEAN minus   = aLineNo & 0x0001;
  BOOLEAN clr_eol = aLineNo & 0x0002;
  BYTE tmpbuf[MW_MAX_LINESIZE+1], *ptr;

  aLineNo &= ~0x0003;  // remove ctrl flag.

  ConvAmount(aAmount, &disp_amt, STIS_TERM_CFG.b_decimal_pos, STIS_TERM_CFG.b_currency);

  memset(tmpbuf, 0, sizeof(tmpbuf));
  ptr = tmpbuf;
  if (minus)  *ptr++ = '-';
  memcpy(ptr, disp_amt.content, disp_amt.len);
  if (clr_eol) aFont |= MW_CLREOL;
  DispLineMW(tmpbuf, aLineNo, MW_RIGHT|aFont);
}
//*****************************************************************************
//  Function        : AddVoidedTotals
//  Description     : Add RECORD_BUF.dd_amount & count to global voided_totals.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void AddTotal(struct TOTAL *aTot, DDWORD aAmount)
{
  aTot->dd_amount += aAmount;
  aTot->w_count++;
}
//*****************************************************************************
//  Function        : AddTotals
//  Description     : Add RECORD_BUF amount to term_tot.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void AddToTotals(void)
{
  if (RECORD_BUF.b_trans_status & ADJUSTED)
    TERM_TOT.d_adjust_count++;

  if (RECORD_BUF.b_trans_status & VOIDED) {
    if (SaleType(RECORD_BUF.b_trans))
      AddTotal(&TERM_TOT.s_void_sale, RECORD_BUF.dd_amount);
    else
      AddTotal(&TERM_TOT.s_void_refund, RECORD_BUF.dd_amount);
    return;
  }

  if (SaleType(RECORD_BUF.b_trans)) {
    AddTotal(&TERM_TOT.s_sale_tot, RECORD_BUF.dd_amount);
    if (RECORD_BUF.dd_tip!=0)
      AddTotal(&TERM_TOT.s_tips_tot, RECORD_BUF.dd_tip);
  }
  else
    AddTotal(&TERM_TOT.s_refund_tot, RECORD_BUF.dd_amount);
}
//*****************************************************************************
//  Function        : CalTotals
//  Description     : Calculate to transaction totals.
//  Input           : all;      // TRUE : total for all transaction.
//                              // FALSE: acquirer[0] ONLY.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : ...
//*****************************************************************************
void CalTotals(BOOLEAN aAll)
{
  DWORD i = 0;

  memset(&TERM_TOT, 0, sizeof(struct TOTAL_STRUCT));
  while (TRUE) {
    if (!APM_GetBatchRec(i, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF)))
      break;
    i++;
    if (aAll || (RECORD_BUF.w_host_idx == INPUT.w_host_idx))
      AddToTotals();
  }
}
//*****************************************************************************
//  Function        : CalIssuerSum
//  Description     : Calculate transaction totals by issuer.
//  Input           : aIssuerIdx;   // issuer index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : TERM_TOT;
//*****************************************************************************
static void CalIssuerSum(WORD aIssuerIdx)
{
  DWORD i = 0;

  while (TRUE) {
    if ((!APM_GetBatchRec(i, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF))))
      break;
    i++;
    if (RECORD_BUF.w_issuer_idx == aIssuerIdx)
      AddToTotals();
  }
}
//*****************************************************************************
//  Function        : ValidAcquirer
//  Description     : Validate whether the Acquirer is Support.
//  Input           : aAcqID;
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN ValidAcquirer(BYTE aAcqID)
{
  BYTE i;
  WORD max_acq = APM_GetAcqCount();

  for (i = 0; i < max_acq; i++) {
    if (!APM_GetAcqTbl(i, &STIS_ACQ_TBL(1)))
      continue;
    if (STIS_ACQ_TBL(1).b_status == NOT_LOADED)
      return (FALSE);
    if (aAcqID == STIS_ACQ_TBL(1).b_id)
      return (CorrectHost(GetHostType(1)));
  }
  return(FALSE);
}
//*****************************************************************************
//  Function        : PackIssuerTotals
//  Description     : Pack total information by issuer.
//  Input           : aIssuerIdx;   // issuer index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackIssuerTotals(WORD aIssuerIdx)
{
  CalIssuerSum(aIssuerIdx);
  pack_mem(STIS_ISS_TBL(1).sb_card_name, 10);
  pack_lf();
  PackTotals(&TERM_TOT);
  PackSeperator('-');
}
//*****************************************************************************
//  Function        : PackVoidTotals
//  Description     : Pack void transaction totals by ISSUER.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void PackVoidTotals(void)
{
  static const BYTE c_voided_sales[]   = {"VOIDED SALES"};
  static const BYTE c_voided_refund[]  = {"VOIDED REFUND"};

  pack_mem((BYTE *)c_voided_sales, 12);
  pack_space(1);
  bindec_data(TERM_TOT.s_void_sale.w_count, 3);
  PackAmt(TERM_TOT.s_void_sale.dd_amount, 14);
  pack_lf();
  pack_mem((BYTE *)c_voided_refund, 13);
  bindec_data(TERM_TOT.s_void_refund.w_count, 3);
  PackAmt(TERM_TOT.s_void_refund.dd_amount, 14);
  pack_lf();
}
//*****************************************************************************
//  Function        : PackIssuerSum
//  Description     : Pack ISSUER transaction sum.
//  Input           : all;        // TRUE :- pack for all issuers.
//                                // FALSE:- ONLY pack for STIS_ISS_TBL(0).
//  Return          : N/A
//  Note            : vico
//  Globals Changed : N/A
//*****************************************************************************
void PackIssuerSum(BOOLEAN aAll)
{
  WORD issuer_idx, card_idx;
  WORD max_issuer = APM_GetIssuerCount();
  WORD max_card   = APM_GetCardCount();

  PackSeperator('-');
  pack_str("\x1bW1 SUMMARY REPORT \x1bW0");
  pack_lf();
  PackSeperator('-');

  memset(&TERM_TOT, 0, sizeof(struct TOTAL_STRUCT));
  for (issuer_idx = 0; issuer_idx < max_issuer; issuer_idx++) {
    if (!APM_GetIssuer(issuer_idx, &STIS_ISS_TBL(1)))
      continue;
    if (STIS_ISS_TBL(1).b_id == 0xff)
      continue;

    for (card_idx = 0; card_idx < max_card; card_idx++) {
      if (!APM_GetCardTbl(card_idx, &STIS_CARD_TBL(0)))
        continue;
      if (STIS_CARD_TBL(0).b_issuer_id != STIS_ISS_TBL(1).b_id)
        continue;
      if (!ValidAcquirer(STIS_CARD_TBL(0).b_acquirer_id))
        continue;
      if (aAll || (STIS_CARD_TBL(0).b_acquirer_id == STIS_ACQ_TBL(0).b_id)) {
        PackIssuerTotals(issuer_idx);
        break;
      }
    }
    // clear total except voided sum
    memset(&TERM_TOT, 0, (BYTE *)&TERM_TOT.s_void_sale-(BYTE *)&TERM_TOT.d_adjust_count);
  }

  PackVoidTotals();
  PackSeperator('-');
  pack_lf();
  PackEndOfRep();
}
//*****************************************************************************
//  Function        : FormatHolderName
//  Description     : Format the card holder name as per AMEX request.
//  Input           : aDest;        // pointer to output buffer.
//                    aName;        // pointer to name buffer from CARD.
//                    len;          // len of name buffer.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : input.sb_holder_name;
//*****************************************************************************
void FormatHolderName(BYTE *aDest, BYTE *aName, BYTE aLen)
{
  BYTE i, tIdx, mIdx;

  memset(aDest, ' ', sizeof(INPUT.sb_holder_name));
  tIdx = aLen;
  mIdx = aLen;
  // locate title
  for (i=0; i < aLen; i++) {
    if (aName[i]== '.') {
      tIdx = i;
      break;
    }
    if (aName[i] == '/')
      mIdx = i;
  }
  // Exception case handle for only exist title or title come first than middle
  if (tIdx < mIdx)
    tIdx = aLen;
  if (aLen > tIdx) {  // title exist
    memcpy(aDest, &aName[tIdx+1], aLen - tIdx-1);
    aDest+= (aLen-tIdx);
  }
  if (tIdx > mIdx) {  // Middle name exist
    memcpy(aDest, &aName[mIdx+1], tIdx - mIdx - 1);
    aDest += (tIdx - mIdx - 1);
  }
  memcpy(aDest, aName, mIdx);
}
//*****************************************************************************
//  Function        : IncTraceNo
//  Description     : Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncTraceNo(void)
{
  memcpy(INPUT.sb_trace_no, STIS_TERM_DATA.sb_trace_no, 3);
  memcpy(INPUT.sb_roc_no, INPUT.sb_trace_no, 3);
  IncAPMTraceNo();
}
//*****************************************************************************
//  Function        : ClearRspData
//  Description     : Clear RSP_DATA.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals         : RSP_DATA;
//*****************************************************************************
void ClearRspData(void)
{
  RSP_DATA.b_response = TRANS_FAIL;
  RSP_DATA.w_rspcode  = 'C'*256+'N';
  RSP_DATA.text[0] = 0;
  memset(&RSP_DATA.text[1], ' ', 69);
  memset(RSP_DATA.sb_rrn, ' ', 12);
  memset(RSP_DATA.sb_auth_code, ' ', 6);
}
//*****************************************************************************
//  Function        : ResetTerm
//  Description     : Reset terminal globals & hardware.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals         : gRspData;
//                    gInput.
//*****************************************************************************
void ResetTerm(void)
{
  IOCtlMW(gMsrHandle, IO_MSR_RESET, NULL);
  gGDS->b_disp_chgd = TRUE;
  MSG_BUF.d_len = 0;
  ClearRspData();
  memset(&INPUT, 0, sizeof(INPUT));
 /* #if (T800)
  os_disp_bl_control(0x10);
  #endif */
  APM_ClearKeyin();
}
//******************************************************************************
//  Function        : ConvB2Comma
//  Description     : Convert the special character in buffer.
//  Input           : aPtr;    // pointer to buffer need to be convert.
//  Return          : N/A
//  Note            : 'B' => ','
//                    'D' => '*'
//                    'E' => '#'
//  Globals Changed : N/A
//******************************************************************************
/*static void ConvB2Comma(BYTE *aPtr)
{
  while (*aPtr) {
    if (*aPtr == 'B') *aPtr = ',';
    else if (*aPtr == 'D') *aPtr = '*';
    else if (*aPtr == 'E') *aPtr = '#';
    aPtr++;
  }
}*/
//******************************************************************************
//  Function        : PackTelNo
//  Description     : Pack telephone number together AT cmd & pabx.
//  Input           : aTermData
//                    aTelBuf;       // pointer to result buffer.
//                    aTel;          // pointer to tel num with 'F' packed.
//  Return          : length of packed buffer.
//  Note            : N/A
//  Globals Changed : N/A
//******************************************************************************
/*static BYTE PackTelNo(struct TERM_DATA aTermData, BYTE *aTelBuf, BYTE *aTel)
{
  BYTE count;

  memset(aTelBuf,0,34);
  count = 0;
  aTelBuf[count++] = aTermData.b_dial_mode == 0 ? 'T' : 'P';
  split(&aTelBuf[count], aTermData.sb_pabx, 4);
  count = (BYTE)fndb(aTelBuf,'F', 9);
  split(&aTelBuf[count],aTel,12);
  count = (BYTE) fndb(aTelBuf, 'F', (BYTE)(count+24));
  aTelBuf[count] = 0;
  ConvB2Comma(aTelBuf);

  return(count);
}*/
//*****************************************************************************
//  Function        : PackComm
//  Description     : Pack EDC host communication parameter
//  Input           : aHostIdx;     // host index
//                    aSettle;      // TRUE => Settlement 
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
/*void PackComm(DWORD aHostIdx, BOOLEAN aForSettle)
{
  struct COMMPARAM comm_param;
  struct ACQUIRER_TBL acq_tbl;
  struct TERM_DATA term_data;

  // get terminal data and acquirer table
  APM_GetTermData(&term_data);
  APM_GetAcqTbl(aHostIdx, &acq_tbl);

  memset(&comm_param, 0, sizeof(comm_param));
  comm_param.wLen           = sizeof(comm_param);
  comm_param.bCommMode      = APM_COMM_AUX;
  comm_param.bHostConnTime  = 3;    // 3 Sec
  comm_param.bTimeoutVal    = 3;    // 3 Sec

  if (memcmp(acq_tbl.sb_ip, "\x00\x00\x00\x00", 4) == 0) {  // ip = 0.0.0.0
    comm_param.bCommMode = APM_COMM_SYNC;
  } else if (memcmp(acq_tbl.sb_ip, "\xFF\xFF\xFF\xFF", 4) == 0) {  // ip = 255.255.255.255
    comm_param.bCommMode = APM_COMM_AUX;
  } else
    comm_param.bCommMode = APM_COMM_TCPIP;

  // modem setting
  memcpy((BYTE *)&comm_param.sMdm, (BYTE *)&KDefaultSReg, sizeof(struct MDMSREG));
  // common parameters
  if ((comm_param.bCommMode == APM_COMM_SYNC) || (comm_param.bCommMode == APM_COMM_ASYNC)) {
    comm_param.bAsyncDelay    = term_data.b_async_delay;
    comm_param.bHostConnTime  = 20;
    comm_param.bTimeoutVal    = acq_tbl.b_timeout_val;
  }
  else if (comm_param.bCommMode == APM_COMM_TCPIP) {
    comm_param.bHostConnTime  = 20;
    comm_param.bTimeoutVal    = acq_tbl.b_timeout_val;
  }
  // settlement host
  if (aForSettle) {
    memcpy(&comm_param.sMdm.sMdmCfg.b_protocol, &KCommParam[acq_tbl.b_settle_mdm_mode].b_protocol, 3);
    comm_param.sMdm.sMdmCfg.b_ptel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_ptel, acq_tbl.sb_pri_settle_tel);
    comm_param.sMdm.sMdmCfg.b_pconn_tval  = acq_tbl.b_pri_settle_conn_time;
    comm_param.sMdm.sMdmCfg.b_pconn_limit = acq_tbl.b_pri_settle_redial;
    comm_param.sMdm.sMdmCfg.b_stel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_stel, acq_tbl.sb_sec_settle_tel);
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_settle_conn_time;
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_settle_redial;
  }
  // transaction host
  else {
    memcpy(&comm_param.sMdm.sMdmCfg.b_protocol, &KCommParam[acq_tbl.b_trans_mdm_mode].b_protocol, 3);
    comm_param.sMdm.sMdmCfg.b_ptel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_ptel, acq_tbl.sb_pri_trans_tel);
    comm_param.sMdm.sMdmCfg.b_pconn_tval  = acq_tbl.b_pri_trans_conn_time;
    comm_param.sMdm.sMdmCfg.b_pconn_limit = acq_tbl.b_pri_trans_redial;
    comm_param.sMdm.sMdmCfg.b_stel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_stel, acq_tbl.sb_sec_trans_tel);
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_trans_conn_time;
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_trans_redial;
  }
  if (comm_param.bCommMode == APM_COMM_SYNC) {
    if (comm_param.sMdm.sMdmCfg.b_protocol == TYPE_ASYN)
      comm_param.bCommMode = APM_COMM_ASYNC;
    else
      comm_param.bCommMode = APM_COMM_SYNC;
  }

  // tcp setting
  comm_param.sTcp.bLen                = sizeof(comm_param.sTcp);
  memcpy(&comm_param.sTcp.sTcpCfg.d_ip, acq_tbl.sb_ip, 4);
  comm_param.sTcp.sTcpCfg.w_port     = acq_tbl.sb_port[0]*256+acq_tbl.sb_port[1];
  comm_param.sTcp.sTcpCfg.b_option   = (acq_tbl.b_ssl_key_idx&0x7F)?MW_TCP_SSL_MODE:0;
  comm_param.sTcp.sTcpCfg.b_sslidx   = acq_tbl.b_ssl_key_idx & 0x7F;
  comm_param.sTcp.bAdd2ByteLen       = acq_tbl.b_reserved1? 1 : 0;    // ExtraMsgLen
  if (acq_tbl.b_ssl_key_idx&0x80) {
    comm_param.sTcp.sTcpCfg.b_option  |= MW_TCP_PPP_MODE;
    if (comm_param.bCommMode == APM_COMM_TCPIP)
      comm_param.bCommMode = APM_COMM_GPRS;
  }

  // aux setting
  comm_param.sAux.bLen              = sizeof(comm_param.sAux);
  comm_param.sAux.bPort             = acq_tbl.sb_port[0];
  comm_param.sAux.sAuxCfg.b_len     = sizeof(struct MW_AUX_CFG);
  comm_param.sAux.sAuxCfg.b_mode    = MW_AUX_NO_PARITY;
  comm_param.sAux.sAuxCfg.b_speed   = acq_tbl.sb_port[1];
  comm_param.sAux.sAuxCfg.b_rx_gap  = 10;   // 100 ms
  comm_param.sAux.sAuxCfg.b_rsp_gap = 10;
  comm_param.sAux.sAuxCfg.b_tx_gap  = 10;
  comm_param.sAux.sAuxCfg.b_retry   = 0;

  // Config PPP parameter
  comm_param.sPPP.bKeepAlive  = TRUE; // Never disconnect
  if (comm_param.bCommMode == APM_COMM_GPRS) 
    comm_param.sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
  else
    comm_param.sPPP.dDevice      = MW_PPP_DEVICE_NONE;
  comm_param.sPPP.dSpeed       = 230400;
  comm_param.sPPP.dMode        = MW_PPP_MODE_NORMAL;
  memset(comm_param.sPPP.scUserID, 0, sizeof(comm_param.sPPP.scUserID));
  memset(comm_param.sPPP.scPwd,    0, sizeof(comm_param.sPPP.scPwd));
  comm_param.sPPP.psLogin      = NULL;
  comm_param.sPPP.dLoginPair   = 0;
  comm_param.sPPP.psDialUp     = NULL;
  comm_param.sPPP.dDialupPair  = 0;
  comm_param.sPPP.psDialParam  = NULL;

  APM_PackComm(&comm_param);
}*/


//*****************************************************************************
//  Function        : PackComm
//  Description     : Pack EDC host communication parameter
//  Input           : aHostIdx;     // host index
//                    aSettle;      // TRUE => Settlement
//  Return          : N/A
//  Note            : N/A
//  Globals         : N/A
//*****************************************************************************
void PackComm(DWORD aHostIdx, BOOLEAN aForSettle)
{
  struct COMMPARAM comm_param;
  //struct ACQUIRER_TBL acq_tbl;
  //struct TERM_DATA term_data;

  // get terminal data and acquirer table
  //APM_GetTermData(&term_data);
  //APM_GetAcqTbl(aHostIdx, &acq_tbl);

  memset(&comm_param, 0, sizeof(comm_param));
  comm_param.wLen           = sizeof(comm_param);
  comm_param.bCommMode      = APM_COMM_AUX;
  comm_param.bHostConnTime  = 120;    // 120 Sec
  comm_param.bTimeoutVal    = 90;    // 40 Sec para la cx PPP
  comm_param.sTcp.sTcpCfg.b_option |= 0x10;

//  if (memcmp(acq_tbl.sb_ip, "\x00\x00\x00\x00", 4) == 0) {  // ip = 0.0.0.0
//    comm_param.bCommMode = APM_COMM_SYNC;
//  } else if (memcmp(acq_tbl.sb_ip, "\xFF\xFF\xFF\xFF", 4) == 0) {  // ip = 255.255.255.255
//    comm_param.bCommMode = APM_COMM_AUX;
//  } else
    //comm_param.bCommMode = APM_COMM_TCPIP;
  	//printf("\fModo: %d", gTcpIp.commMode);
  	//WaitKey(3000, 0);
#if(JTRACE == TRUE)
  	comm_param.bCommMode = APM_COMM_AUX;
#else
    comm_param.bCommMode = gTcpIp.commMode;/*APM_COMM_GPRS;*/
#endif
/*
  // modem setting
  memcpy((BYTE *)&comm_param.sMdm, (BYTE *)&KDefaultSReg, sizeof(struct MDMSREG));
  // common parameters
  if ((comm_param.bCommMode == APM_COMM_SYNC) || (comm_param.bCommMode == APM_COMM_ASYNC)) {
    comm_param.bAsyncDelay    = term_data.b_async_delay;
    comm_param.bHostConnTime  = 20;
    comm_param.bTimeoutVal    = acq_tbl.b_timeout_val;
  }
  else if (comm_param.bCommMode == APM_COMM_TCPIP) {
    comm_param.bHostConnTime  = 20;
    comm_param.bTimeoutVal    = acq_tbl.b_timeout_val;
  }
  // settlement host
  if (aForSettle) {
    memcpy(&comm_param.sMdm.sMdmCfg.b_protocol, &KCommParam[acq_tbl.b_settle_mdm_mode].b_protocol, 3);
    comm_param.sMdm.sMdmCfg.b_ptel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_ptel, acq_tbl.sb_pri_settle_tel);
    comm_param.sMdm.sMdmCfg.b_pconn_tval  = acq_tbl.b_pri_settle_conn_time;
    comm_param.sMdm.sMdmCfg.b_pconn_limit = acq_tbl.b_pri_settle_redial;
    comm_param.sMdm.sMdmCfg.b_stel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_stel, acq_tbl.sb_sec_settle_tel);
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_settle_conn_time;
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_settle_redial;
  }
  // transaction host
  else {
    memcpy(&comm_param.sMdm.sMdmCfg.b_protocol, &KCommParam[acq_tbl.b_trans_mdm_mode].b_protocol, 3);
    comm_param.sMdm.sMdmCfg.b_ptel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_ptel, acq_tbl.sb_pri_trans_tel);
    comm_param.sMdm.sMdmCfg.b_pconn_tval  = acq_tbl.b_pri_trans_conn_time;
    comm_param.sMdm.sMdmCfg.b_pconn_limit = acq_tbl.b_pri_trans_redial;
    comm_param.sMdm.sMdmCfg.b_stel_len    = PackTelNo(term_data, comm_param.sMdm.sMdmCfg.s_stel, acq_tbl.sb_sec_trans_tel);
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_trans_conn_time;
    comm_param.sMdm.sMdmCfg.b_sconn_tval  = acq_tbl.b_sec_trans_redial;
  }
  if (comm_param.bCommMode == APM_COMM_SYNC) {
    if (comm_param.sMdm.sMdmCfg.b_protocol == TYPE_ASYN)
      comm_param.bCommMode = APM_COMM_ASYNC;
    else
      comm_param.bCommMode = APM_COMM_SYNC;
  }
*/
  // tcp setting
  comm_param.sTcp.bLen                = sizeof(comm_param.sTcp);

  //memcpy(&comm_param.sTcp.sTcpCfg.d_ip, acq_tbl.sb_ip, 4);
  //comm_param.sTcp.sTcpCfg.w_port     = acq_tbl.sb_port[0]*256+acq_tbl.sb_port[1];

  /*printf("IP:%d.%d.%d.%d", gTcpIp.sb_ip[0], gTcpIp.sb_ip[1], gTcpIp.sb_ip[2], gTcpIp.sb_ip[3]);
  WaitKey(3000, 0);*/

  if(gTcpIp.cxPrimaria == TRUE){
	  //printf("\fIP 1: %02d.%02d.%02d.%02d", gTcpIp.sb_ip[0], gTcpIp.sb_ip[1], gTcpIp.sb_ip[2], gTcpIp.sb_ip[3]);WaitKey(3000, 0);
	  memcpy(&comm_param.sTcp.sTcpCfg.d_ip, gTcpIp.sb_ip, 4);
	  comm_param.sTcp.sTcpCfg.w_port = gTcpIp.w_port;
  }
  else{
	  //printf("\fIP 1: %02d.%02d.%02d.%02d", gTcpIp.sb_ip2[0], gTcpIp.sb_ip2[1], gTcpIp.sb_ip2[2], gTcpIp.sb_ip2[3]);WaitKey(3000, 0);
	  memcpy(&comm_param.sTcp.sTcpCfg.d_ip, gTcpIp.sb_ip2, 4);
	  comm_param.sTcp.sTcpCfg.w_port = gTcpIp.w_port2;
  }


  //comm_param.sTcp.sTcpCfg.b_option   = (acq_tbl.b_ssl_key_idx&0x7F)?MW_TCP_SSL_MODE:0;
  comm_param.sTcp.sTcpCfg.b_option   = 0;
  /* JJJ */
  if (comm_param.bCommMode == APM_COMM_GPRS)
    comm_param.sTcp.sTcpCfg.b_option |= MW_TCP_PPP_MODE;
  /* JJJ */

  //comm_param.sTcp.sTcpCfg.b_sslidx   = acq_tbl.b_ssl_key_idx & 0x7F;
  comm_param.sTcp.sTcpCfg.b_sslidx   = 0;
  //comm_param.sTcp.bAdd2ByteLen       = acq_tbl.b_reserved1? 1 : 0;    // ExtraMsgLen
  comm_param.sTcp.bAdd2ByteLen       = 1;    // ExtraMsgLen
  /*
  if (acq_tbl.b_ssl_key_idx&0x80)
  {
    comm_param.sTcp.sTcpCfg.b_option  |= MW_TCP_PPP_MODE;
    if (comm_param.bCommMode == APM_COMM_TCPIP)
      comm_param.bCommMode = APM_COMM_GPRS;
  }
*/
  // aux setting
  comm_param.sAux.bLen              = sizeof(comm_param.sAux);
  //comm_param.sAux.bPort             = acq_tbl.sb_port[0];
  comm_param.sAux.sAuxCfg.b_len     = sizeof(struct MW_AUX_CFG);
  comm_param.sAux.sAuxCfg.b_mode    = MW_AUX_NO_PARITY;
  comm_param.sAux.sAuxCfg.b_speed   = MW_AUX_115200;
  comm_param.sAux.sAuxCfg.b_rx_gap  = 10;   // 100 ms
  comm_param.sAux.sAuxCfg.b_rsp_gap = 10;
  comm_param.sAux.sAuxCfg.b_tx_gap  = 10;
  comm_param.sAux.sAuxCfg.b_retry   = 0;

  // Config PPP parameter
  comm_param.sPPP.bKeepAlive  = TRUE; // Never disconnect
  //comm_param.sPPP.bKeepAlive  = FALSE; // disconnect PPP into APM_ResetComm() function;
  if (comm_param.bCommMode == APM_COMM_GPRS)
    comm_param.sPPP.dDevice      = MW_PPP_DEVICE_GPRS;
  else
    comm_param.sPPP.dDevice      = MW_PPP_DEVICE_NONE;
  comm_param.sPPP.dSpeed       = 230400;
  comm_param.sPPP.dMode        = MW_PPP_MODE_NORMAL;
  memset(comm_param.sPPP.scUserID, 0, sizeof(comm_param.sPPP.scUserID));
  memset(comm_param.sPPP.scPwd,    0, sizeof(comm_param.sPPP.scPwd));
  memset(comm_param.sPPP.scApn, 0, sizeof(comm_param.sPPP.scApn));  // DDD
  //strcpy(comm_param.sPPP.scApn, "internet.movistar.com.co");
  if(gTcpIp.cxPrimaria == TRUE){
	  //printf("\fAPN 1: %s", gTcpIp.sb_APN);WaitKey(3000, 0);
	  strcpy(comm_param.sPPP.scApn, gTcpIp.sb_APN);    // JJJ
	  strcpy(comm_param.sPPP.scUserID , gTcpIp.sb_UserName);
	  strcpy(comm_param.sPPP.scPwd , gTcpIp.sb_Password);
  }
  else{
	  //printf("\fAPN 2: %s", gTcpIp.sb_APN2);WaitKey(3000, 0);
	  strcpy(comm_param.sPPP.scApn, gTcpIp.sb_APN2);    // JJJ
	  strcpy(comm_param.sPPP.scUserID , gTcpIp.sb_UserName2);
	  strcpy(comm_param.sPPP.scPwd , gTcpIp.sb_Password2);
  }
  comm_param.sPPP.psLogin      = NULL;
  comm_param.sPPP.dLoginPair   = 0;
  comm_param.sPPP.psDialUp     = NULL;
  comm_param.sPPP.dDialupPair  = 0;
  comm_param.sPPP.psDialParam  = NULL;

  APM_PackComm(&comm_param);
}




//*****************************************************************************
//  Function        : IncTraceNoCNB
//  Description     : Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncTraceNoCNB(void)
{
  memcpy(INPUT.sb_trace_no, gAppDat.sb_trace_no, 3);
  memcpy(INPUT.sb_roc_no, INPUT.sb_trace_no, 3);
  bcdinc(gAppDat.sb_trace_no, 3);

  if( memcmp(gAppDat.sb_trace_no, "\x00\x00\x00", 3) == 0 )
    bcdinc(gAppDat.sb_trace_no, 3);

  if( memcmp(gAppDat.sb_trace_no, "\x99\x99\x99", 3) == 0 )	// Agrego Daniel Jacome 14/07/12
    memcpy(gAppDat.sb_trace_no, "\x00\x00\x01", 3);

  SaveDataFile( &gAppDat );
}



//*****************************************************************************
//  Function        : IncTraceNoCNB
//  Description     : Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncTraceNoCNBReverse(void)
{
  memcpy(TX_DATA.sb_trace_no, gAppDat.sb_trace_no, 3);
  memcpy(TX_DATA.sb_roc_no, TX_DATA.sb_trace_no, 3);
  bcdinc(gAppDat.sb_trace_no, 3);

  if( memcmp(gAppDat.sb_trace_no, "\x00\x00\x00", 3) == 0 )
    bcdinc(gAppDat.sb_trace_no, 3);

  if( memcmp(gAppDat.sb_trace_no, "\x99\x99\x99", 3) == 0 )	// Agrego Daniel Jacome 14/07/12
    memcpy(gAppDat.sb_trace_no, "\x00\x00\x01", 3);

  SaveDataFile( &gAppDat );
}






//*****************************************************************************
//  Function        : DispAmnt
//  Description     : Display the formated amount line_nbr. (right justify)
//  Input           : aAmount;    // DDWORD amount
//                    aLineNo;    // aLineNo & 0xF0 is the line number
//                                // to be displayed.
//                                // Bit 0 : 1 - minus sign needed
//                                // Bit 1 : 1 - OVERWRITE
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispAmnt(DDWORD aAmount, DWORD aLineNo, DWORD aFont)
{
  struct DISP_AMT disp_amt;
//  BOOLEAN minus   = aLineNo & 0x0001;
  //BOOLEAN clr_eol = aLineNo & 0x0002;
  BYTE tmpbuf[MW_MAX_LINESIZE+1], *ptr;

  aLineNo &= ~0x0003;  // remove ctrl flag.

  ConvAmount(aAmount, &disp_amt, 2,'$');

  memset(tmpbuf, 0, sizeof(tmpbuf));
  ptr = tmpbuf;
  //if (minus)  *ptr++ = '-';
  memcpy(ptr, disp_amt.content, disp_amt.len);
  //if (clr_eol) aFont |= MW_CLREOL;

  DispLineMW(tmpbuf, aLineNo, MW_RIGHT|aFont);
}



void PackVoucher(BOOLEAN aReprint)
{
  MsgBufSetup();

  //PackContentCNB();
  pack_mem( MW_LPT_FONT_SMALL, 3 );
  ReplaceLF( gToken_8.cupon );
  //serialSend2(gToken_8.cupon, strlen(gToken_8.cupon));
  //printf("\fLen: %d", strlen(gToken_8.cupon));APM_WaitKey(3000, 0);
  pack_mem( gToken_8.cupon, strlen(gToken_8.cupon) );

  if( aReprint )
  {
    pack_lf();
    pack_mem("\x1BW1****REPRINT****\x1BW0", 21);
  }
  PackFF();
  PackMsgBufLen();
  if (aReprint == FALSE)
    SaveLastRcpt(MSG_BUF.sb_content, MSG_BUF.d_len);
}



void procesarMensajePOS(void){
	BYTE proceso;

	if( gMsg60Show == FALSE )
		return;

	AcceptBeep();
	AcceptBeep();
	AcceptBeep();

	proceso = gMsgPOS[0];

	if(proceso == 'T'){
		MsgBufSetup();
		pack_mem( MW_LPT_FONT_SMALL, 3 );
		pack_mem( &gMsgPOS[1], strlen(gMsgPOS) - 1 );
		PackFF();
		PackMsgBufLen();
		DispLineMW("MENSAJE", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
		printf("\f\n%s\n", &gMsgPOS[1]);
		APM_WaitKey(3000,0);

		PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
		APM_ClearKeyin();  // flush key to avoid display refresh caused by key event
		MsgBufSetup();
		PackMsgBufLen();
		fCommitAllMW();
		RefreshDispAfter(5);
	}
	else if(proceso == 'P'){
		DispLineMW("MENSAJE", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
		printf("\n%s\n", &gMsgPOS[1]);
		APM_WaitKey(3000,0);
	}
	else if(proceso == 'I'){
		MsgBufSetup();
		pack_mem( MW_LPT_FONT_SMALL, 3 );
		pack_mem( &gMsgPOS[1], strlen(gMsgPOS) - 1);
		PackFF();
		PackMsgBufLen();
		PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
		APM_ClearKeyin();  // flush key to avoid display refresh caused by key event
		MsgBufSetup();
		PackMsgBufLen();
		fCommitAllMW();
		RefreshDispAfter(5);
	}
	gMsg60Show = FALSE;
}





void PackLimoper(void)
{
	BYTE actualDateTime[14 + 1];
	BYTE date[9 + 1];
	BYTE time[7 + 1];
	BYTE valMinRetiros[10 + 1];
	BYTE valMinDepositos[10 + 1];
	BYTE costoServicio[10 + 1];
	int montos;

	/*memcpy(gToken_80.valMinRetiro, "\x00\x10", 2);
	memcpy(gToken_80.valMinDeposito, "\x00\x12", 2);
	memcpy(gToken_80.costoServicio, "\x00\x35", 2);*/

	MsgBufSetup();

	memset(actualDateTime, 0x00, sizeof(actualDateTime));
	memset(date, 0x00, sizeof(date));
	memset(time, 0x00, sizeof(time));
	memset(valMinRetiros, 0x00, sizeof(valMinRetiros));
	memset(valMinDepositos, 0x00, sizeof(valMinDepositos));
	memset(costoServicio, 0x00, sizeof(costoServicio));

	split( valMinRetiros, gToken_80.valMinRetiro, 2 );
	split( valMinDepositos, gToken_80.valMinDeposito, 2 );
	split( costoServicio, gToken_80.costoServicio, 2 );

	/*printf("\f|%s|\n", valMinRetiros);
	printf("|%s|\n", valMinDepositos);
	printf("|%s|", costoServicio);
	WaitKey(3000, 0);*/

	montos = atoi(valMinRetiros);
	memset(valMinRetiros, 0x00, sizeof(valMinRetiros));
	sprintf(valMinRetiros, "%02d", montos);

	montos = atoi(costoServicio);
	memset(costoServicio, 0x00, sizeof(costoServicio));
	sprintf(costoServicio, "00.%02d", montos);

	montos = atoi(valMinDepositos);
	memset(valMinDepositos, 0x00, sizeof(valMinDepositos));
	sprintf(valMinDepositos, "%02d", montos);

	RtcGetMW(actualDateTime);

	sprintf(date + 1, "%c%c%c%c/%c%c/%c%c", actualDateTime[0], actualDateTime[1], actualDateTime[2], actualDateTime[3], actualDateTime[4],
			actualDateTime[5], actualDateTime[6], actualDateTime[7]);
	sprintf(time, "%c%c:%c%c:%c%c", actualDateTime[8], actualDateTime[9], actualDateTime[10], actualDateTime[11], actualDateTime[12],
			actualDateTime[13]);

	pack_lf();
	pack_mem("INFORME DE LIMITE DE OPERACIONES", 32);
	pack_lf();
	pack_mem("Fecha: ", 7);
	pack_mem(&date[1], 10);
	pack_lf();
	pack_mem("Hora: ", 6);
	pack_mem(time, 8);
	pack_lf();
	pack_nbyte('=', 32);
	pack_lf();
	pack_mem("             RETIRO             ", 32);
	pack_mem("Limite Minimo: US$ ", 19);
	pack_mem(valMinRetiros, strlen(valMinRetiros));
	pack_lf();
	pack_mem("Costo de servicio: US$ ", 23);
	pack_mem(costoServicio, strlen(costoServicio));
	pack_lfs(2);
	pack_mem("            DEPOSITO            ", 32);
	pack_mem("Limite Minimo: US$ ", 19);
	pack_mem(valMinDepositos, strlen(valMinDepositos));
	pack_lf();
	pack_nbyte('=', 32);
	pack_lfs(2);
	PackFF();
	PackMsgBufLen();

	PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
	MsgBufSetup();
	PackMsgBufLen();
}






int PrintTransTest(void){
    BYTE DateTime[14];
    //BYTE DateTimeRec[14 + 1];
    BYTE stan[6 + 1];
    BYTE rrn[12 + 1];
    BYTE procCode[6 + 1];
    int n;

    memset(DateTime, 0x00, sizeof(DateTime));
    memset(stan, 0x00, sizeof(stan));
    memset(rrn, 0x00, sizeof(rrn));
    memset(procCode, 0x00, sizeof(procCode));
    RtcGetMW(DateTime);
    for (n = 0; n < 5; n++) // 5 es el maximo de transacciones (ver gMaxRec en el APM)
    {
        APM_GetBatchRec(n, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
        //split( DateTimeRec, (BYTE*) &RECORD_BUF.s_dtg, 7 );
//         if( memcmp(DateTimeRec, DateTime, 8) != 0 ){
//           printf("\fFecha no es igual");WaitKey(3000, 0);
//           continue;
//         }
        split( stan, RECORD_BUF.sb_trace_no, 3 );
        split( procCode, RECORD_BUF.procCode, 3 );
        memcpy(rrn, RECORD_BUF.sb_rrn, 12);
        printf("\fSTAN: %s\n", stan);
        printf("RRN: %s\n", rrn);
        printf("PROC CODE: %s\n", procCode);
        printf("MONTO: %d\n", RECORD_BUF.dd_org_amount);
        printf("n = %d", n);
        WaitKey(3000, 0);
    }
  return TRUE;
}

  

void printParam(void)
{
    //BYTE Buffer[16];
	//BYTE BufEnc[32];
	//WORD ret=0;
	struct MW_APPL_INFO applicationInfo;
  BYTE checksum[4 + 1];

  BYTE serial[9];
	BYTE date[9 + 1];
	BYTE time[7 + 1];

	BYTE AuxIP[15 + 1];
	BYTE AuxPORT[5 + 1];
	BYTE AuxLinea[48 + 1 ];
	BYTE kbdbuf[16];
    BYTE kvc[2];

  //  BYTE aux[32], aux2[32];		//NOHORA 10Dic
	
  memset(checksum, 0x00, sizeof(checksum));
  //checksum[5] = 0x00;
  AppInfoGetMW(MY_APPL_ID, &applicationInfo);

  sprintf(checksum, "%04X", applicationInfo.w_checksum);  
  struct MW_NIF_INFO nif_info;

  	struct DATETIME dtg_a;
	memset(kvc,0x00,2);
	memset(kbdbuf,0x00,16);
	
   	kbdbuf[1]='1'; //MKEY_IDX
#if(CX_TCP == TRUE)
   	NetInfoMW(MW_NIF_ETHERNET, &nif_info);
#else
	NetInfoMW(MW_NIF_PPP, &nif_info);
#endif
	//KDLL_Key3Encrypt( CARDKEY_IDX, track2, 40 );    // PENDIENTE	 KDLL_KeyEncrypt
   	KDLL_GetTmkKvc(kbdbuf[1]-'0', &kvc);

   	//ret = KDLL_Key3Encrypt(MKEY_IDX, Buffer, 16);
   //ret = KDLL_KeyEncrypt(MKEY_IDX, Buffer, 16);
   //	printf("\f ret %d",ret);APM_WaitKey(3000,0);
   //	split(BufEnc, Buffer, 16);

	MsgBufSetup();
	//PackCtrl(PRN_BOLD, 1);

	pack_mem("           PARAMETROS         ", 24);

	pack_lf();
	pack_lf();
	    
	//os_rtc_get(actualDateTime);
	ReadRTC(&dtg_a);

	sprintf(date + 1, "%02X%02X/%02X/%02X", dtg_a.b_century, dtg_a.b_year, dtg_a.b_month, dtg_a.b_day);
	sprintf(time, "%02X:%02X:%02X", dtg_a.b_hour, dtg_a.b_minute, dtg_a.b_second);

	pack_lf();
   	pack_mem("Fecha: ", 7);
	pack_mem(&date[1], 10);
	pack_lf();
	pack_mem("Hora:  ", 7);
	pack_mem(time, 8);
	pack_lf();
	pack_lf();
	pack_str("MODELO:  SPECTRA T1000");
	pack_lf();
	pack_lf();
	pack_mem("SERIAL:", 7);
	GetSysCfgMW( MW_SYSCFG_SERIAL_NO, serial );
	pack_mem(serial, 8);
	pack_lf();
  pack_lf();
  pack_mem("VERSION:", 8);
  pack_mem(checksum, 4);
  pack_lf();
	pack_lf();
  	pack_mem("IP ACTUAL:", 10);
	pack_space(3);
	memset(AuxIP,0x00,15);
#if(CX_TCP == TRUE)
	NetInfoMW(MW_NIF_ETHERNET, &nif_info);
#else
	NetInfoMW(MW_NIF_PPP, &nif_info);
#endif
	DispTermIPX((BYTE *)&nif_info);
	pack_mem(g_ip,strlen(g_ip));
	pack_lf();
	pack_lf();
	pack_mem("IP PRIMARIA:", 12);
	pack_space(3);
	memset(AuxIP,0x00,15);
	sprintf(AuxIP,"%d.%d.%d.%d",gTcpIp.sb_ip[0],gTcpIp.sb_ip[1],gTcpIp.sb_ip[2],gTcpIp.sb_ip[3]);
	pack_mem(AuxIP,strlen(AuxIP));
	pack_lf();
	pack_mem("PUERTO PRIMARIO:", 16);
	pack_space(3);
	memset(AuxPORT,0x00,5);
	sprintf(AuxPORT,"%d",gTcpIp.w_port);
	pack_mem(AuxPORT,strlen(AuxPORT));
	pack_lf();
	pack_lf();
	pack_mem("IP SECUNDARIA:", 14);
	pack_space(3);
	memset(AuxIP,0x00,15);
	sprintf(AuxIP,"%d.%d.%d.%d",gTcpIp.sb_ip2[0],gTcpIp.sb_ip2[1],gTcpIp.sb_ip2[2],gTcpIp.sb_ip2[3]);
	pack_mem(AuxIP,strlen(AuxIP));
	pack_lf();
	pack_mem("PUERTO SECUNDARIO:", 18);
	pack_space(3);
	memset(AuxPORT,0x00,5);
	sprintf(AuxPORT,"%d",gTcpIp.w_port);
	pack_mem(AuxPORT, strlen(AuxPORT));
	pack_lf();
	pack_lf();
	pack_mem("Checksum MK:", 12);
	pack_space(3);
	memset(AuxLinea,0x00,48);
	sprintf(AuxLinea,"%02X%02X",kvc[0], kvc[1]);
	pack_mem(AuxLinea, 4);
	pack_lf();
	pack_lf();
	pack_mem("APN PRIMARIO:", 13);
	pack_lf();
	memset(AuxLinea,0x00,48);
	sprintf(AuxLinea,"%s",gTcpIp.sb_APN);
	pack_mem(AuxLinea, strlen(AuxLinea));
	pack_lf();
	pack_lf();
	pack_mem("APN SECUNDARIO:", 15);
	pack_lf();
	memset(AuxLinea,0x00,48);
	sprintf(AuxLinea,"%s",gTcpIp.sb_APN2);
	pack_mem(AuxLinea, strlen(AuxLinea));

	//NOHORA 10Dic Desde aca
	pack_lf();
 /*   memset(aux, 0x00, sizeof(aux));
	split(aux, gtoken_97.worKeyPin, 16);
    pack_mem("Key1 (NO MK):", 13);	pack_lf();
    pack_mem(aux, 32);
    memset(aux2, 0x00, sizeof(aux2));
    split(aux2,  gtoken_97.worTrack2 , 16);
    pack_mem("Key2 (NO MK):", 13);	pack_lf();
    pack_mem(aux2, 32);

	pack_lfs(3);*/
	PackSeperator('-');
  pack_lfs(6);
    //NOHORA 10Dic Hasta aca

	PackMsgBufLen();
	  //printf("\f");
	//PrintBuf((struct PRNBUF *) pBuf, TRUE, FALSE);
	PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);

	MsgBufSetup();
	PackMsgBufLen();
}



void ShowIpx(unsigned char *aIp)
{
  BYTE buf[128];
  g_ip1= 0;g_ip2= 0;g_ip3= 0;

  g_ip1 = aIp[0];g_ip2 = aIp[1];g_ip3 = aIp[2];
  sprintf(buf, "%3d.%3d.%3d.%3d", aIp[0], aIp[1], aIp[2], aIp[3]);
  memset(g_ip, 0x00, sizeof(g_ip));
  memcpy(g_ip, buf, sizeof(g_ip));
}

void DispTermIPX(BYTE *aIP)
{
 ShowIpx(aIP);
  
}
