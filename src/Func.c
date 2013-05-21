//-----------------------------------------------------------------------------
//  File          : func.c
//  Module        :
//  Description   : Include Merchant Functions.
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "sysutil.h"
#include "message.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "input.h"
#include "lptutil.h"
#include "print.h"
#include "tranutil.h"
#include "record.h"
#include "settle.h"
#include "menu.h"
#include "testrans.h"
#include "debugstis.h"
//#include "testcfg.h"
#include "adjust.h"
#include "func.h"
#include "keytrans.h"
#include "LCD_LOGO.h"
#include "SPGLib.h"


//-----------------------------------------------------------------------------
//  Constants
//-----------------------------------------------------------------------------
//static const BYTE KAppvCode[]  = {"APPR CODE:"};

// Default manager Function Menu
/*static const struct MENU_ITEM KFuncItem [] =
{
  {  1, "Display Batch"},
  {  5, "Display Total"},
  {  9, "Clear Reversal"},
  { 21, "Trans Detail"},
  { 22, "Adjust Tips"},
  { 50, "Set Demo EDC"},
  { 51, "Load TMK"},
  { 52, "Load Keys"},
  { 53, "Set Encr Mode"},
  { 54, "Encr Data"},
  { 55, "Get PIN"},
  { 56, "Reset Keys"},
  { 57, "Set Dukpt Host Idx"},
  { 58, "Load Dukpt Keys"},
  { 59, "Get Dukpt PIN"},
  { 60, "Reset Dukpt Keys"},
  { 61, "ICC TMK Inject"},
  { 64, "Reboot"},
  #if (LPT_SUPPORT|TMLPT_SUPPORT)    // Conditional Compile for Printer support
  { 72, "Reprint Last"},
  { 73, "Reprint Any"},
  { 74, "Print Total"},
  { 75, "Print Batch"},
  #endif                    // PRINTER_SUPPORT
  { 96, "Test Trans"},
  { 99, "Clear Batch"},
  {-1, NULL},
};

static const struct MENU_DAT KFuncMenu  = {
  "EDC Functions",
  KFuncItem,
};*/
//*****************************************************************************
//  Function        : DispTransData
//  Description     : Display transaction record.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*static void DispTransData(struct TXN_RECORD *aRec)
{
  static const BYTE KTraceNum[]   = {"TRACE NUM:"};
  static const BYTE KVoided[]     = {"VOIDED "};
  BYTE buffer[MW_MAX_LINESIZE+1], *ptr;
  DWORD var_j;

  DispLineMW(KTraceNum, MW_LINE1, MW_CLRDISP|MW_SMFONT);
  split(buffer,aRec->sb_trace_no, 3);
  buffer[6] = 0;
  DispLineMW(buffer, MW_LINE1, MW_RIGHT|MW_SMFONT);
  if (aRec->b_trans_status & ADJUSTED)
    var_j = ADJUST;
  else
    var_j = aRec->b_trans;

  if ((aRec->b_trans_status & VOIDED) != 0) {
    memcpy(buffer, KVoided, sizeof(KVoided));
    DispLineMW(buffer, MW_LINE2, MW_SMFONT);
  }
  ptr = buffer;
  memcpy(ptr, GetConstMsg(KTransHeader[var_j])+4, 12);
  ptr += 12;
  *ptr++ = 0;
  DispLineMW(buffer, MW_LINE2, MW_RIGHT|MW_SMFONT);

  DispAmount(aRec->dd_amount, MW_LINE3, MW_SMFONT);

  switch (aRec->b_entry_mode) {
    case ICC:
      buffer[0] = 'C';
      break;
    case MANUAL:
      buffer[0] = 'M';
      break;
    case SWIPE:
      buffer[0] = 'S';
      break;
    case FALLBACK:
      buffer[0] = 'F';
      break;
    default:
      buffer[0] = ' ';
      break;
  }
  split(&buffer[1],aRec->sb_pan, 10);
  var_j = (BYTE) fndb(&buffer[1], 'F', 19);
  if (MaskCardNo())
    memset(&buffer[7], 'X', 6);
  buffer[var_j+1] = 0;
  DispLineMW(buffer, MW_LINE4, MW_CLREOL|MW_SPFONT);
}*/
//*****************************************************************************
//  Function        : DispReversal
//  Description     : Display reversal batch record.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void DispReversal(void)
{
  static const BYTE KTraceNum[] = {"TRACE NUM:"};
  static const BYTE KVoided[]   = {"VOIDED "};
  BYTE i, var_j;
  BYTE tmp[MW_MAX_LINESIZE+1];
  BOOLEAN no_reversal;
  WORD max_acq = APM_GetAcqCount();

  no_reversal = TRUE;
  for (i=0;i<max_acq; i++) {
    if (APM_GetPending(i) != REV_PENDING)
      continue;
    APM_GetAcqTbl(i, &STIS_ACQ_TBL(0));
    if (!CorrectHost(GetHostType(0)))
      continue;

    no_reversal = FALSE;
    APM_GetRevRec(i, &RECORD_BUF,sizeof(RECORD_BUF));

    DispLineMW("REVERSAL", MW_LINE1, MW_REVERSE|MW_CENTER|MW_SMFONT);
    DispLineMW("HOST: ", MW_LINE2, MW_SMFONT);
    memcpy(tmp, STIS_ACQ_TBL(0).sb_name, 10);
    tmp[10] = 0;
    DispLineMW(tmp, MW_LINE2+6, MW_SMFONT);
    DispLineMW(KTraceNum, MW_LINE3, MW_SMFONT);
    split(tmp,RECORD_BUF.sb_roc_no, 3);
    tmp[6]= 0;
    DispLineMW(tmp, MW_LINE3+10, MW_SMFONT);
    if (RECORD_BUF.b_trans_status & ADJUSTED)
      var_j = ADJUST;
    else
      var_j = RECORD_BUF.b_trans;
    memcpy(tmp, GetConstMsg(KTransHeader[var_j]), 16);
    tmp[16]=0;
    DispLineMW(&tmp[4], MW_LINE4+4, MW_SMFONT);

    if ((RECORD_BUF.b_trans_status & VOIDED) != 0) {
      DispLineMW(KVoided, MW_LINE4, MW_SMFONT);
    }
    DispAmount(RECORD_BUF.dd_amount, MW_LINE5, MW_SMFONT);

    split(tmp,RECORD_BUF.sb_pan, 10);
    var_j = (BYTE) fndb(tmp, 'F', 19);
    tmp[var_j] = 0;
    DispLineMW(tmp, MW_LINE6, MW_RIGHT|MW_SPFONT);

    ConvDateTime(tmp, &RECORD_BUF.s_dtg, FALSE);
    tmp[16] = 0;
    DispLineMW(tmp, MW_LINE7, MW_SMFONT);
    memcpy(tmp, RECORD_BUF.sb_ecr_ref, 16);
    DispLineMW(tmp, MW_LINE8, MW_SMFONT);
    APM_WaitKey(KBD_TIMEOUT,0);
  }
  if (no_reversal) {
    DispClrBelowMW(MW_LINE3);
    Disp2x16Msg(GetConstMsg(EDC_FN_NO_REV), MW_LINE3, MW_BIGFONT);
    APM_WaitKey(KBD_TIMEOUT,0);
  }
}*/
//*****************************************************************************
//  Function        : ClearReversal
//  Description     : Clear acquirer reversal flag.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ClearReversal(void)
{
  int i;

  DispLineMW("CLEAR REVERSAL", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  if (!APM_GetAccessCode())
    return;

  while (1) {
    if ((i = APM_SelectAcquirer(FALSE)) == -1)
      return;
    DispLineMW("CLEAR REVERSAL", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
    if (APM_GetPending(i) != REV_PENDING) {
      Disp2x16Msg(GetConstMsg(EDC_FN_NO_REV), MW_LINE3, MW_CENTER|MW_BIGFONT);
      APM_WaitKey(KBD_TIMEOUT,0);
      break;
    }
    else {
      DispLineMW(GetConstMsg(EDC_FN_CLR_REV), MW_LINE5, MW_CENTER|MW_BIGFONT);
      switch (APM_YesNo()) {
        case 0 :
          return;
        case 2 :
          APM_SetPending(i, NO_PENDING);
          DispLineMW(GetConstMsg(EDC_FN_REV_CLEAR), MW_LINE5, MW_CENTER|MW_BIGFONT);
          ErrorDelay();
      }
    }
  }
}
//*****************************************************************************
//  Function        : DispStatusMsg
//  Description     : Display status message
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*static void DispStatusMsg(BYTE *aMsg)
{
  Disp2x16Msg(aMsg, MW_LINE5, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  Delay1Sec(1, 1);
}*/
//*****************************************************************************
//  Function        : DisplayBatch
//  Description     : Display batch record.
//  Input           : record index;     // display with specify record index.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void DisplayBatch(DWORD aRecIdx)
{
  static const BYTE KRRN[] = {"RRN:"};
  BYTE tmpbuf[MW_MAX_LINESIZE+1];
  int rec_cnt, rec_idx;
  DWORD keyin;

  rec_cnt = APM_GetRecCount();
  if (rec_cnt == 0) {
    RSP_DATA.w_rspcode = 'N'*256+'T';
    DispCtrlMW(MW_CLR_DISP);
    DispRspText(FALSE);
    ErrorDelay();
    return;
  }

  if (aRecIdx == -1) {
    DispLineMW(GetConstMsg(EDC_FN_DISP_BATCH), MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
    rec_idx = SearchRecord(FALSE);
    if (rec_idx == -1)
      return;
  }
  else
    rec_idx = rec_cnt - 1;

  while (1) {
    APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));	// Journal
    APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
    if (!CorrectHost(GetHostType(0))) {
      if (rec_idx == 0) {
        DispStatusMsg(GetConstMsg(EDC_FN_START_BATCH));
        break;
      }
      else
        rec_idx -= 1;
      continue;
    }
    APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));
    DispTransData(&RECORD_BUF);
    DispLineMW(KAppvCode, MW_LINE5, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_auth_code, sizeof(RECORD_BUF.sb_auth_code));
    DispLineMW(tmpbuf, MW_LINE5, MW_RIGHT|MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    ConvDateTime(tmpbuf, &RECORD_BUF.s_dtg, FALSE);
    tmpbuf[16] = 0;
    DispLineMW(tmpbuf, MW_LINE6, MW_SMFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_ecr_ref, sizeof(RECORD_BUF.sb_ecr_ref));
    //sprintf(tmpbuf, "%d", RECORD_BUF.dd_org_amount);
    DispLineMW(tmpbuf, MW_LINE7, MW_CENTER|MW_SMFONT);
    DispLineMW(KRRN, MW_LINE8, MW_SPFONT);
    memset(tmpbuf, 0, sizeof(tmpbuf));
    memcpy(tmpbuf, RECORD_BUF.sb_rrn, sizeof(RECORD_BUF.sb_rrn));
    DispLineMW(tmpbuf, MW_LINE8+4, MW_SPFONT);
    keyin=APM_WaitKey(KBD_TIMEOUT,0);
    if (keyin == MWKEY_CANCL)
      break;
    if ((keyin == MWKEY_ENTER)||(keyin == MWKEY_UP)) {
      if (rec_idx == 0) {
        DispStatusMsg(GetConstMsg(EDC_FN_START_BATCH));
      }
      else
        rec_idx -= 1;
    }
    if ((keyin ==MWKEY_CLR)||(keyin == MWKEY_DN)) {
      if (rec_idx == (rec_cnt - 1))
        DispStatusMsg(GetConstMsg(EDC_FN_END_BATCH));
      else
        rec_idx += 1;
    }
  }
}*/
//*****************************************************************************
//  Function        : ClearBatch
//  Description     : Clear EDC batch record.
//  Input           : aHeader;          // Header Line.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void ClearBatch(void)
{
  int i;
  WORD max_acq;

  DispCtrlMW(MW_CLR_DISP);
  if (!APM_GetAccessCode())
    return;
  DispClrBelowMW(MW_LINE3);
  DispLineMW(GetConstMsg(EDC_FN_CLR_BATCH), MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (APM_YesNo() == 2) {
    DispClrBelowMW(MW_LINE3);
    INPUT.w_host_idx = -1;
    SetDefault();
    max_acq = APM_GetAcqCount();
    for (i = 0; i < max_acq; i++) {
      if (!APM_GetAcqTbl(i, &STIS_ACQ_TBL(0)))
        break;
      if (CorrectHost(GetHostType(0))) {
        APM_SetPending(i, NO_PENDING);
        APM_BatchClear(i);
      }
    }
    fCommitAllMW();
    DispLineMW(GetConstMsg(EDC_FN_BATCH_ERASE), MW_LINE5, MW_CENTER|MW_BIGFONT);
    AcceptBeep();
    Delay1Sec(10, 0);
  }
}*/
//*****************************************************************************
//  Function        : DispTransTotal
//  Description     : Display transaction totals.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void DispTransTotal(void)
{
  static const BYTE KSale[]    = {"SALE:"};
  static const BYTE KRefund[]  = {"REFUND:"};
  BYTE buffer[5];
  int host_idx;
  BOOLEAN all_acq;

  do {
    all_acq=FALSE;
    host_idx = APM_SelectAcquirer(TRUE);
    if (host_idx == -1)
      break;
    if (host_idx == -2) {
      all_acq = TRUE;
      INPUT.w_host_idx = 0;
    }
    else {
      INPUT.w_host_idx = (WORD) host_idx;
      APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0));
    }

    CalTotals(all_acq);
    bin2dec(TERM_TOT.s_sale_tot.w_count, buffer, 3);
    buffer[3] = 0;
    DispLineMW(KSale, MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispLineMW(buffer, MW_LINE1, MW_RIGHT|MW_BIGFONT);
    DispAmount(TERM_TOT.s_sale_tot.dd_amount, MW_LINE3, MW_BIGFONT);

    bin2dec(TERM_TOT.s_refund_tot.w_count, buffer, 3);
    buffer[3] = 0;
    DispLineMW(KRefund, MW_LINE5, MW_BIGFONT);
    DispLineMW(buffer, MW_LINE5, MW_RIGHT|MW_BIGFONT);
    DispAmount(TERM_TOT.s_refund_tot.dd_amount, MW_LINE7, MW_BIGFONT);

    if (APM_WaitKey(KBD_TIMEOUT, 0) == MWKEY_CANCL)
      break;
  } while (1);
}*/
//*****************************************************************************
//  Function        : ReprintTxn
//  Description     : Reprint specified receipt.
//  Input           : aLastBatch;     // TRUE=>Last Batch Record
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void ReprintTxn(BOOLEAN aLastBatch)
{
  int rec_cnt, rec_idx;

  rec_cnt = APM_GetRecCount();
  if (rec_cnt == 0) {
    RSP_DATA.w_rspcode = 'N'*256+'T';
    DispCtrlMW(MW_CLR_DISP);
    DispRspText(FALSE);
    ErrorDelay();
    return;
  }

  DispLineMW(GetConstMsg(EDC_FN_REPRINT_BATCH), MW_LINE1, MW_REVERSE|MW_CENTER|MW_BIGFONT);
  if (aLastBatch) {
    rec_idx = rec_cnt - 1;
  }
  else {
    rec_idx = SearchRecord(TRUE);
    if (rec_idx == -1)
      return;
  }

  APM_GetBatchRec(rec_idx, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
  APM_GetAcqTbl(RECORD_BUF.w_host_idx, &STIS_ACQ_TBL(0));
  APM_GetIssuer(RECORD_BUF.w_issuer_idx, &STIS_ISS_TBL(0));
  PackRecordP(TRUE);
  PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
}*/
//*****************************************************************************
//  Function        : PrintTotals
//  Description     : Print Transaction total information.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void PrintTotals(void)
{
  DispLineMW(GetConstMsg(EDC_FN_CARD_TOTAL), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  MsgBufSetup();
  PackLogo();
  pack_lfs(2);
  pack_mem("Terminal ID: ", 13);
  pack_mem(STIS_TERM_DATA.sb_term_id, 8);
  pack_lf();
  PackCurrentDate();
  pack_lfs(2);
  PackIssuerSum(TRUE);
  PackFF();
  PackMsgBufLen();
  PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
}*/
//*****************************************************************************
//  Function        : PrintTxnRecord
//  Description     : Print transaction Log.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void PrintTxnRecord(void)
{
  static const BYTE c_nii[] = {"NII "};
  static const BYTE KBase[] = {"BASE  "};
  static const BYTE c_tip[] = {"TIP   "};
  DDWORD amount;
  WORD line_cnt, rec_cnt;
  BYTE tmp[21];
  DWORD keyin, i = 0;
  struct DATETIME dtg;
  struct DISP_AMT disp_amt;
  int  host_idx;

  host_idx = APM_SelectAcquirer(FALSE);
  if (host_idx == -1)
    return;

  APM_GetAcqTbl(host_idx, &STIS_ACQ_TBL(0));
  DispLineMW(GetConstMsg(EDC_FN_CARD_REC), MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_BIGFONT);
  MsgBufSetup();

  PackLogo();
  pack_lfs(2);
  pack_str("\x1bW1 DETAILS REPORT\x1bW0");
  pack_lfs(2);
  pack_mem("Terminal ID: ", 13);
  pack_mem(STIS_ACQ_TBL(0).sb_term_id, 8);
  pack_lf();
  PackCurrentDate();
  pack_lfs(2);

  pack_mem(STIS_ACQ_TBL(0).sb_name,10);
  pack_space(MAX_CHAR_LINE_NORMAL-18);
  pack_mem((BYTE *)c_nii, 4);
  split_data(STIS_ACQ_TBL(0).sb_nii,2);
  pack_lf();
  pack_mem("Batch# " , 7);
  pack_space(MAX_CHAR_LINE_NORMAL-13);  // Batch title= 7 + batch # = 6
  split_data(STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
  pack_lf();
  PackSeperator('=');

  pack_lf();
  pack_str("TRACE#  APP.CODE   TRANS  TIME");
  pack_lf();
  pack_str("ACCT #               EXP. DATE");
  pack_lf();
  pack_str("AMOUNT                    DATE");
  pack_lf();
  PackSeperator('-');

  line_cnt = 18;
  rec_cnt = APM_GetRecCount();
  for (i = 0; i < rec_cnt; i++) {
    APM_GetBatchRec(i, (BYTE *)&RECORD_BUF, sizeof(RECORD_BUF));
    if (RECORD_BUF.w_host_idx != host_idx)
      continue;

    if (line_cnt > 80) {
      PackMsgBufLen();
      PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, TRUE);
      MsgBufSetup();
      line_cnt = 0;
    }
    split_data(RECORD_BUF.sb_roc_no, 3);
    pack_space(1);
    pack_mem(RECORD_BUF.sb_auth_code,6);
    pack_space(1);

    keyin = RECORD_BUF.b_trans;
    if (RECORD_BUF.b_trans_status & ADJUSTED) {
      keyin = ADJUST;
    }
    memcpy(tmp, GetConstMsg(KTransHeader[keyin]), 16);
    memmove(tmp, &tmp[6], 10);
    if (RECORD_BUF.b_trans_status & VOIDED) {
      for (keyin=1;keyin<10;keyin++) {
        if (tmp[keyin]!=' ') {
          tmp[keyin-1]='V';
          break;
        }
      }
    }
    pack_mem(tmp, 10);
    //pack_space(1);

    ByteCopy((BYTE*) &dtg, (BYTE*)&RECORD_BUF.s_dtg, sizeof(struct DATETIME));
    ConvDateTime(tmp, &dtg, TRUE);
    pack_mem(&tmp[12],6);
    pack_lf();

    split(get_pptr(),RECORD_BUF.sb_pan, 10);
    inc_pptr(keyin = (BYTE)fndb(get_pptr(), 'F', 19));
    pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-keyin-4));
    split_data(&RECORD_BUF.sb_exp_date[1], 1);
    split_data(&RECORD_BUF.sb_exp_date[0],1);
    pack_lf();
    ConvAmount(RECORD_BUF.dd_amount, &disp_amt, STIS_TERM_CFG.b_decimal_pos, STIS_TERM_CFG.b_currency);
    pack_mem(disp_amt.content, disp_amt.len);
    pack_space((BYTE)(MAX_CHAR_LINE_NORMAL-disp_amt.len-6));
    pack_mem(tmp, 6);
    pack_lf();

    if (SaleType((BYTE)(RECORD_BUF.b_trans)) && TIPsReqd()) {
      amount = RECORD_BUF.dd_amount - RECORD_BUF.dd_tip;
      pack_mem((BYTE *)KBase, 6);
      pack_space(6);
      PackAmt(amount, 19);
      pack_lf();
      pack_mem((BYTE *)c_tip, 6);
      pack_space(6);
      PackAmt(RECORD_BUF.dd_tip, 19);
      pack_lf();
    }
    pack_str("ECR/INV NO:");
    if (skpb(RECORD_BUF.sb_ecr_ref,' ', 16)==16)
      pack_str(" NIL");
    else
      pack_mem(RECORD_BUF.sb_ecr_ref,16);
    pack_lfs(2);
    line_cnt += 7;
  }

  PackSeperator('=');
  pack_lfs(2);
  PackEndOfRep();
  PackFF();
  PackMsgBufLen();
  PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
}*/
//extern void ReferralCall(void);
//*****************************************************************************
//  Function        : MerchantFunc
//  Description     : Process function base on the input function id.
//  Input           : aFuncId;        // Function Id
//  Return          : TRUE;           // indicate func process complete
//                    FALSE;          // func not define.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*BOOLEAN MerchantFunc(DWORD aFuncId)
{
  int select;

  select = aFuncId;
  if (aFuncId == -1) {
    select = 0;
    select = MenuSelect(&KFuncMenu, select);
  }

  gGDS->b_disp_chgd = TRUE;
  if (select == -1)
    return TRUE;

  switch (select) {
    case 1:
      DisplayBatch(0);  // Start from last rec
      break;
    case 5:
      DispTransTotal();
      break;
    case 8:
      DispReversal();
      break;
    case 9 :
      ClearReversal();
      break;
    case 21:
      DisplayBatch(-1);  // Prompt User for TRACE
      break;
    case 22:
      AdjustTrans(-1);
      break;
      //case 23:
      //if (TIPsReqd())
      //  AdjustTrans(TRUE);   // adjust base amount
      //  break;
      // debug only ++

    case 50:
      DispLineMW("Reset DEMO Data", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
      DispLineMW("PROCESSING...",  MW_LINE5, MW_CENTER|MW_BIGFONT);

      // Set STIS data
      SaveSTISDbgData(MODE_ALL_PARAM);
      fCommitAllMW();
      LoadEMVKey();
      LoadEMVCfg();
      DispClrBelowMW(MW_LINE5);
      DispLineMW("Enter to Reboot!", MW_LINE5, MW_CENTER|MW_BIGFONT);
      APM_WaitKey(9000, 0);
      fCommitAllMW();
      APM_MerchFunc(64);
      break;
      // debug only ++
    case 51:
      LoadTmk();
      break;
    case 52:
      LoadKeys();
      break;
    case 53:
      SetMode();
      break;
    case 54:
      EncData();
      break;
    case 55:
      GetPIN();
      break;
    case 56:
      ResetKey(0xFF);
      break;
    case 57:
      SetDhostIdx();
      break;
    case 58:
      LoadDukptIkey();
      break;
    case 59:
      GetDukptPin();
      break;
    case 60:
      ResetDukptIkey();
      break;
    case 61:
      IccTmkInject();
      break;
    case 64:
      fCommitAllMW();
      APM_MerchFunc(select);
      break;
     #if (LPT_SUPPORT|TMLPT_SUPPORT)    // Conditional Compile for Printer support
    case 72:
      ReprintLast();
      break;
    case 73:
      ReprintTxn(FALSE);
      break;
    case 74:
      PrintTotals();
      break;
    case 75:
      PrintTxnRecord();
      break;
    #endif                    // PRINTER_SUPPORT
    case 96:
      return TestTrans();
    case 99:
      ClearBatch();
      break;
    default:
      RefreshDispAfter(0);
      return(FALSE);
  }
  return(TRUE);
}*/


BOOLEAN checkTeleCarga(void){
	BYTE fechaTeleCarga[8 + 1];
	BYTE horaTeleCarga[6 + 1];
	BYTE buffer[14 + 1];
	BYTE fechaPos[8 + 1];
	BYTE horaPos[6 + 1];
	int intHoraTeleCarga;
	int intHoraPos;
	int intFechaTeleCarga;
	int intFechaPos;

	if( gTeleCarga == FALSE )
		return FALSE;

	memset(fechaTeleCarga, 0x00,  sizeof(fechaTeleCarga));
	memset(horaTeleCarga,  0x00,  sizeof(horaTeleCarga));
	memset(buffer, 		   0x00,  sizeof(buffer));
	memset(fechaPos, 	   0x00,  sizeof(fechaPos));
	memset(horaPos, 	   0x00,  sizeof(horaPos));

	split(fechaTeleCarga, gToken_96.fechaTeleCarga, 4);
	split(horaTeleCarga, gToken_96.horaTeleCarga, 3);
	intHoraTeleCarga = atoi(horaTeleCarga);	//	paso a int la hora en que debe hacerse la telecarga
	intFechaTeleCarga = atoi(fechaTeleCarga);

	RtcGetMW(buffer);	// Extraigo la fecha y hora del POS
	memcpy(fechaPos, buffer, 8);
	memcpy(horaPos,  buffer + 8, 6);
	intHoraPos = atoi(horaPos);	// paso a int la hora actual del POS
	intFechaPos = atoi(fechaPos);
  /*	printf("\f buff %s", buffer);APM_WaitKey(6000,0);
	printf("\f fpos %s \n ftel %s \n h pos %s \n htel %s", fechaPos, fechaTeleCarga, horaPos,horaTeleCarga );APM_WaitKey(6000,0);
	printf("\f fpos %d \n ftel %d \n h pos %d \n htel %d", intFechaPos, intFechaTeleCarga, intHoraPos,intHoraTeleCarga );APM_WaitKey(6000,0);
   */
	if( intFechaPos >= intFechaTeleCarga ){     //if( memcmp(fechaTeleCarga, fechaPos, 8) == 0 ){
		
		if(intFechaPos > intFechaTeleCarga )
		   return TRUE;

		   if( intHoraPos >= intHoraTeleCarga )
				return TRUE;
	}
	return FALSE;
}

struct MENU_ITEM SyspackItem[20];

struct MENU_DAT SyspackMenu  = {
	  "VERSIONES",
	  SyspackItem,
	};


int verVersionSyspack(void)
{
	    T_APP_INFO app_list;

    DWORD i, contador = 0;
	int select = 0;
    //BYTE noofapp;
    BYTE linea[18 + 1];
    //int select;

    memset(linea, 0x00, sizeof(linea));
   // printf("\f\x1C      VERSIONES      \x1D");
    for( i = 0; i < 50; i++ )
    {
        if(os_app_search_id(i) == TRUE)
        {
			
            os_app_info(i , &app_list);
			 
            memset(linea,0x00,sizeof(linea));
            sprintf(linea, "%s -%04X ", app_list.s_app_name, app_list.w_checksum);
			SyspackItem[contador].iID = contador + 1;
            memcpy( SyspackItem[contador].scDescDat, linea, 18 );
            //printf("\n%s", linea);
            //APM_WaitKey(2000, 0);
			contador++;
        }

    }
  
  SyspackItem[contador].iID = -1;
  SyspackItem[contador].pcDesc = NULL;

  	
  select = 0;
  select = MenuSelect(&SyspackMenu, select);
  if (select == -1)
    return FALSE;
 

    APM_WaitKey(2000, 0);
    return 0;
}





void showLogo(void)
{
	GIFLoadMem( logoPichincha, sizeof(logoPichincha));
	GIFPlayOn( 0, 0 );
	GIFPlay();
	GIFStop(0);
	GIFUnLoad();

	/*struct CGRAPH *pLogo;

	pLogo = (struct CGRAPH *) MallocMW(sizeof(logoPichincha));
	if (pLogo == NULL)
		return;

	memcpy(pLogo, logoPichincha, sizeof(logoPichincha));

	pLogo->w_hstart = 0;
	pLogo->w_vstart = 0;

	DispPutGMW(pLogo);

	FreeMW(pLogo);*/

}




void showMessageError(void){
	BYTE linea1[80 + 1];
	BYTE i, y=0;

	if( strlen( gToken_7.msg_largo) <= 0 )
		return;

	memset(linea1, 0x00, sizeof(linea1));
	for(i=0; i<60; i++){
		if( i%20 == 0 ){
			linea1[y++] = 0x0A;
			linea1[y++] = 0x20;
			linea1[y++] = 0x20;
		}
		linea1[y++] = gToken_7.msg_largo[i];
	}
	DispLineMW( "ERROR", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	LongBeep();
	DispPutCMW(K_PushCursor);
	os_disp_textc(COLOR_RED);
	DispGotoMW( MW_LINE3 + 2, MW_SPFONT );
	printf("%s", linea1);
	DispPutCMW(K_PopCursor);
	WaitKey(1500, 0);

	procesarMensajePOS();
}



BOOLEAN confirmarRecarga(BYTE *empresa){
	DispLineMW( "CONFIRMAR DATOS", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "Empresa:", MW_LINE2, MW_LEFT|MW_SMFONT );
	DispLineMW( empresa, MW_LINE2 + 8, MW_LEFT|MW_SMFONT );
	DispLineMW( "Contrapartida:", MW_LINE4, MW_LEFT|MW_SMFONT );
	DispLineMW( INPUT.sb_contrapar, MW_LINE5, MW_LEFT|MW_SMFONT );
	DispLineMW( "Monto:", MW_LINE7, MW_LEFT|MW_SMFONT );
	DispAmnt(INPUT.dd_amount, MW_LINE7, MW_SMFONT);
	if( SiNo() != 2 ) {  // 2 = Press Enter
		return FALSE;
	}
	return TRUE;
}



BOOLEAN confirmarCompraStock(BYTE *aEmpresa, BYTE *aEstabl){

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Cod. Empresa:", MW_LINE2 + 2, MW_LEFT|MW_SPFONT );
	DispLineMW( aEmpresa, MW_LINE3 + 2, MW_SPFONT );

	DispLineMW( "Cod. Establecimiento:", MW_LINE5 + 2, MW_LEFT|MW_SPFONT );
	DispLineMW( aEstabl, MW_LINE6 + 2, MW_SPFONT );

	DispLineMW( "Valor:", MW_LINE8, MW_LEFT|MW_SMFONT );
	DispAmnt(INPUT.dd_amount, MW_LINE8, MW_SMFONT);

	if( SiNo() != 2 )   // 2 = Press Enter
		return FALSE;

	return TRUE;
}



BOOLEAN procesoConsultaSaldoImp(void){
	int costServ = 0;
	BYTE kbdbuff[10];
	BYTE tmpPan[20 + 1];
	//BYTE aux[10];

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(tmpPan,  0x00, sizeof(tmpPan));
	//memset(aux, 0x00, sizeof(aux));

	costServ = RSP_DATA.dd_amount;
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Costo Servicio:", MW_LINE3, MW_LEFT|MW_SPFONT );
	//split(aux, gToken_80.costoServicio, 2);
	//costServ = atoi(aux);
	DispAmnt( costServ, MW_LINE3, MW_SPFONT );
	if( SiNo() != 2 ) {  // 2 = Press Enter
		return FALSE;
	}
	DispLineMW( "DESLICE LA TARJETA", MW_LINE4, MW_SMFONT|MW_CENTER|MW_CLRDISP );

	if( GetCard( FALSE, FALSE ) == FALSE ){
		LongBeep();
		DispLineMW( "CONSULTA DE SALDO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
		DispLineMW( "TRANSACCION CANCELADA", MW_LINE3, MW_CENTER );
		APM_WaitKey(200, 0);
		return FALSE;
	}
	do
	{
		DispLineMW( "CONSULTA DE SALDO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
		DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
		DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
		DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

		memset(kbdbuff, 0x00, sizeof(kbdbuff));
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
			return FALSE;
		}
		// Compara los ultimo 4 digitos del Pan
		memset( tmpPan, 0x00, sizeof(tmpPan) );
		split( tmpPan, INPUT.sb_pan, 10 );
		RTrim( tmpPan, 'F');
	} while (memcmp(&kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4) != 0);

	DispLineMW( "CONSULTA DE SALDO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "Costo Servicio:", MW_LINE3, MW_LEFT|MW_SPFONT );
	DispAmnt(costServ, MW_LINE3, MW_SPFONT);

	if( !getPinblock() )
		return FALSE;

	INPUT.dd_amount = costServ;	// El costo del servicio debe viajar en el campo 4 en la proxima transaccion
	sprintf(gAmount, "%012d", INPUT.dd_amount);

	return TRUE;
}

BOOLEAN procesoConsultaSaldoCNB(void){
  
  BYTE kbdbuff[10];
  BYTE tmpPan[20 + 1];
  //BYTE aux[10];

  memset(kbdbuff, 0x00, sizeof(kbdbuff));
  memset(tmpPan,  0x00, sizeof(tmpPan));
  //memset(aux, 0x00, sizeof(aux));

  
  DispClrBelowMW(MW_LINE2);
  DispLineMW( "CONSULTA SALDO CNB", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
  DispLineMW( "DESLICE LA TARJETA", MW_LINE4, MW_SMFONT|MW_CENTER|MW_CLRDISP );

  if( GetCard( FALSE, FALSE ) == FALSE ){
    LongBeep();
    DispLineMW( "CONSULTA SALDO CNB", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
    DispLineMW( "TRANSACCION CANCELADA", MW_LINE3, MW_CENTER );
    APM_WaitKey(200, 0);
    return FALSE;
  }
  do
  {
    DispLineMW( "CONSULTA SALDO CNB", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
    DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
    DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
    DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

    memset(kbdbuff, 0x00, sizeof(kbdbuff));
    APM_SetCol(7); // Set Columna for TextBox
    if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
      return FALSE;
    }
    // Compara los ultimo 4 digitos del Pan
    memset( tmpPan, 0x00, sizeof(tmpPan) );
    split( tmpPan, INPUT.sb_pan, 10 );
    RTrim( tmpPan, 'F');
  } while (memcmp(&kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4) != 0);

  DispLineMW( "CONSULTA SALDO CNB", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
  

  if( !getPinblock() )
    return FALSE;

   return TRUE;
}


BOOLEAN checkMessageInit(void){
	BYTE buffer[14 + 1];
	BYTE horaPos[6 + 1];
	int intHoraPos;

	memset(buffer, 0x00, sizeof(buffer));
	memset(horaPos, 0x00, sizeof(horaPos));

	RtcGetMW(buffer);	// Extraigo la fecha y hora del POS
	memcpy(horaPos,  buffer + 8, 6);
	intHoraPos = atoi(horaPos);	// paso a int la hora actual del POS
	//printf("\fHora: %d", intHoraPos);
	if( intHoraPos > 235800 && intHoraPos < 235900){
		return TRUE;
	}
	return FALSE;
}



void mostrarSaldoCnb(void){
	BYTE nombre[26 + 1];
	BYTE fechaHora[26];
	BYTE rtc[14];

	memset(nombre,    0x00, sizeof(nombre));
	memset(fechaHora, 0x00, sizeof(fechaHora));
	memset(rtc,       0x00, sizeof(rtc));

	ReplaceLF( gToken_8.cupon );

	RtcGetMW(rtc);
	sprintf(fechaHora, "Fecha %.2s/%.2s/%.2s Hora %.2s:%.2s", &rtc[6], &rtc[4], &rtc[2], &rtc[8], &rtc[10]);

	memset(nombre, 0x00, sizeof(nombre));
	memcpy(nombre, gToken_47.nombre, 26);

	DispLineMW( "CONSULTA SALDO CNB", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispLineMW( fechaHora, MW_LINE3, MW_LEFT|MW_SPFONT );
	DispLineMW( "NOMBRE DEL CNB: ", MW_LINE4, MW_LEFT|MW_SMFONT );
	DispLineMW( nombre, MW_LINE5, MW_LEFT|MW_SPFONT );
	DispGotoMW( MW_LINE7 + 2, MW_SPFONT );
	printf("%s", gToken_8.cupon);

	APM_WaitKey(1000, 0);
}
