//-----------------------------------------------------------------------------
//  File          : reversal.c
//  Module        :
//  Description   : Include routines send reversal transaction to host.
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
#include "util.h"
#include "sysutil.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "tranutil.h"
#include "hostmsg.h"
#include "emvtrans.h"
#include "record.h"
#include "reversal.h"

//*****************************************************************************
//  Function        : ReversalOK
//  Description     : Send reversal transaction to host if available.
//  Input           : N/A
//  Return          : TRUE;     // sended;
//                    FALSE;    // send error.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReversalOK(void)
{
  DWORD host_idx;

  host_idx = INPUT.w_host_idx;
  if (APM_GetPending(host_idx) != REV_PENDING)
    return /*FALSE;*/TRUE;
  APM_GetRevRec(host_idx, &RECORD_BUF, sizeof(RECORD_BUF));

  if (TX_DATA.b_trans == VOID) {
    PackProcCode(RECORD_BUF.b_trans, RECORD_BUF.b_acc_ind);
    TX_DATA.sb_proc_code[0] |= 0x02;
    TX_DATA.dd_amount = 0;
  }
  else
    PackProcCode(TX_DATA.b_trans, RECORD_BUF.b_acc_ind);

  TX_DATA.b_org_trans = TX_DATA.b_trans;
  TX_DATA.b_trans = EDC_REV;

  PackHostMsg();
  ClearResponse();
  if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
    RSP_DATA.b_response = CheckHostRsp();
    if (RSP_DATA.b_response <= TRANS_REJ)
      UpdateHostStatus(NO_PENDING);
  }
  if (RSP_DATA.b_response <= TRANS_REJ) {
    //gAppDat.reversal_count++;	// Daniel 23/07/12
    return TRUE;
  }
  return FALSE;
}




//*****************************************************************************
//  Function        : ReversalOK2
//  Description     : Send reversal transaction to host if available.
//  Input           : N/A
//  Return          : TRUE;     // sended;
//                    FALSE;    // send error.
//  Note            : open socket before send
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ReversalOK2(void)
{
  DWORD host_idx;
  gMsg60Show = FALSE;

  host_idx = INPUT.w_host_idx;
  if (APM_GetPending(host_idx) != REV_PENDING)
    return FALSE;//TRUE;
  APM_GetRevRec(host_idx, &RECORD_BUF, sizeof(RECORD_BUF));

  PackComm(TRUE, FALSE);
  APM_PreConnect();

  if (TX_DATA.b_trans == VOID) {
    PackProcCode(RECORD_BUF.b_trans, RECORD_BUF.b_acc_ind);
    TX_DATA.sb_proc_code[0] |= 0x02;
    TX_DATA.dd_amount = 0;
  }
  else
    PackProcCode(TX_DATA.b_trans, RECORD_BUF.b_acc_ind);

  TX_DATA.b_org_trans = TX_DATA.b_trans;
  TX_DATA.b_trans = EDC_REV;

  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK){
	  PackHostMsg();
	  ClearResponse();
	  if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
		  RSP_DATA.b_response = CheckHostRsp();
		  if (RSP_DATA.b_response <= TRANS_REJ){
			  UpdateHostStatus(NO_PENDING);
			  printf("\f");
			  showMessage(6, 3, "AVISO");
		  }
	  }
	  if (RSP_DATA.b_response <= TRANS_REJ) {
		  gAppDat.reversal_count++;
		  return TRUE;
	  }
  }
  //return FALSE;
  return TRUE;
}



//********************FUNCIONES PARA EL MANEJO DE REVERSO EN PICHINCHA*******************************//


void saveReversoPichincha (void){
	struct REVTABLE revTable;

	revTable.MTI = TX_DATA.Msgtype;
	memcpy(revTable.procCode, TX_DATA.procCode, 3);
	memcpy(revTable.STAN, TX_DATA.sb_trace_no, 3);
	memcpy(revTable.dateTrans, &TX_DATA.s_dtg.b_month, 2);
	memcpy(revTable.timeTrans, &TX_DATA.s_dtg.b_hour, 3);
	memset(revTable.r_User, 0x00, 15);
	memcpy(revTable.r_User, gUsers_tbl.sb_user, 15);
	revTable.enableReverse = REV_PENDING;

	SaveReverseFile(&revTable);
}



BOOLEAN reversalPichinchaOk(void){
	if( getReverseFile() == -1){
		printf("\ferror al leer reverso");WaitKey(3000, 0);
		return FALSE;
	}
	if( gRevTable.enableReverse == NO_PENDING){
		//printf("\fNo hay reverso");WaitKey(3000, 0);
		return TRUE;
	}

	TX_DATA.b_trans = EDC_REV;

	ReadRTC( &TX_DATA.s_dtg );
	IncTraceNoCNBReverse();
	PackHostMsg();
	ClearResponse();
	if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK) {
		RSP_DATA.b_response = CheckHostRsp();
		if (RSP_DATA.b_response <= TRANS_REJ){
			//UpdateHostStatus(NO_PENDING);
			clearReversoPichincha();
		}
	}
	if (RSP_DATA.b_response <= TRANS_REJ) {
		//gAppDat.reversal_count++;	// Daniel 23/07/12
		return TRUE;
	}
	return FALSE;
}



void clearReversoPichincha (void){
	struct REVTABLE revTable;

	memset(&revTable, 0x00, sizeof(struct REVTABLE));
	SaveReverseFile(&revTable);
}
