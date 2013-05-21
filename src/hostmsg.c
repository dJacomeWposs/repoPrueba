//-----------------------------------------------------------------------------
//  File          : hostmsg.c
//  Module        :
//  Description   : Include routines to Pack/Unpack Host Message.
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
#include <stdlib.h>
#include "corevar.h"
#include "chkoptn.h"
#include "constant.h"
#include "record.h"
#include "message.h"
#include "tranutil.h"
#include "util.h"
#include "sysutil.h"
#include "hostmsg.h"
#include "tokensPack.h"
#include "tokensUnpack.h"
#include "keytrans.h"
#include "rs232.h"

//*****************************************************************************
//  Function        : TrainiingRsp
//  Description     : Generate training response.
//  Input           : N/A
//  Return          : Transaction status
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BYTE TrainingRsp(void)
{
  struct DATETIME dtg;

  ReadRTC(&dtg);
  memcpy(&RSP_DATA.s_dtg.b_year, &dtg.b_year, 6);
  memset(RSP_DATA.sb_auth_code, '0', sizeof(RSP_DATA.sb_auth_code));
  split(RSP_DATA.sb_rrn, &dtg.b_year, 6);
  RSP_DATA.b_response = TRANS_ACP;
  RSP_DATA.w_rspcode = '0'*256+'0';
  RSP_DATA.dd_amount = 0;
  memset(&RSP_DATA.text[21], ' ', 20);
  return TRANS_ACP;
}
//*****************************************************************************
//  Function        : Match
//  Description     : Compare current pointer to buffer.
//  Input           : aPtr;    // pointer to buffer needs to Match.
//                    aLen;    // no. of byte to Match.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static BOOLEAN Match(void *aPtr, BYTE aLen)
{
  BOOLEAN result;

  result = (memcmp(aPtr, get_pptr(), aLen) == 0);
  inc_pptr(aLen);
  return(result);
}
//*****************************************************************************
//  Function        : BcdBin8b
//  Description     : Convert max 6 bytes of bcd number to DDWORD.
//  Input           : aSrc;        // pointer src bcd number
//                    aLen;        // no. of byte of the bcd number to be conv.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DDWORD BcdBin8b(void *aSrc,BYTE aLen)
{
  BYTE buffer[12];

  if (aLen < 7) {
    split(buffer,aSrc,aLen);
    return decbin4b(buffer,(BYTE)(aLen*2));
  }
  return 0;
}
//*****************************************************************************
//  Function        : SyncHostDtg
//  Description     : Sync RTC with HOST date & time.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void SyncHostDtg(void)
{
  BYTE tmp;
  struct DATETIME dtg;

  ReadRTC(&dtg);
  RSP_DATA.s_dtg.b_century = dtg.b_century;
  RSP_DATA.s_dtg.b_year = dtg.b_year;
  tmp = 1;
  if ((RSP_DATA.s_dtg.b_month == 0x12) && (dtg.b_month == 0x01))
    bcdsub(&RSP_DATA.s_dtg.b_year,&tmp,1);
  else
    if ((RSP_DATA.s_dtg.b_month == 0x01) && (dtg.b_month == 0x12))
      bcdadd(&RSP_DATA.s_dtg.b_year,&tmp,1);
  ByteCopy((BYTE *)&dtg, (BYTE *)&RSP_DATA.s_dtg, sizeof(struct DATETIME));
  SetRTC(&dtg);
}
//*****************************************************************************
//  Function        : PackProcCode
//  Description     : Pack the processing code for input transaction type,
//  Input           : aTrans;      // transaction type
//                    aProcCode;   // 2nd byte of processing code
//  Return          : N/A
//  Note            : when aProcCode != 0xFF => use INPUT.b_acc_ind
//  Globals Changed : TX_DATA.sb_proc_code[];
//*****************************************************************************
void PackProcCode(BYTE aTrans, BYTE aProcCode)
{
  TX_DATA.sb_proc_code[0] = KTransBitmap[aTrans].b_proc_code;

  if (aTrans == ADJUST)
    TX_DATA.sb_proc_code[0] = 2;
  else
    if ((aTrans <= AUTH_MANUAL) || (aTrans == AUTH_ICC))  // !2005-09-21
      TX_DATA.sb_proc_code[0] = 0x30;

  if (aProcCode == 0xFF)
    aProcCode = INPUT.b_acc_ind;

  TX_DATA.sb_proc_code[1] = aProcCode;
  TX_DATA.sb_proc_code[2] = 0;
}
/*********************************************************************/
/*  This procedure fills stxbuf with information required according  */
/*  to the BOOLEAN map of the trans.                                 */
/*  02. primary account number   03. processing code                 */
/*  04. amount, transaction      11. system trace number             */
/*  12. local transaction time   13. local transaction date          */
/*  14. card expiration date     22. entry mode                      */
/*  24. netword int'l id.        25. condition code                  */
/*  35. track 2 data             37. retrieval reference number      */
/*  38. authorization id.        39. response code                   */
/*  41. terminal id              42. card acceptor id                */
/*  44. additional resp data     45. track 1 data                    */
/*  52. PIN                                                          */
/*  54. additional amount        60. private - org. amount           */
/*  61. product code             62. private - invoice no            */
/*  63. settlement totals        64. MAC                             */
/*********************************************************************/
void PackHostMsg(void)
{
  BYTE * bitmap_ptr;
  BYTE *lenFieldPtr = NULL;
  BYTE tx_bitmap[8];
  BYTE var_i, var_j;
  BYTE tmp[10];  // request by dbin2bcd
  BYTE tmpSN[12];
  int lenToken=0;
  struct DESC_TBL prod;

  TxBufSetup(TRUE/*ExtraMsgLen()*/);

  /* TPDU header */
  pack_byte(0x60);
  //pack_mem((BYTE *)&STIS_ACQ_TBL(0).sb_nii, 2);
  pack_null(2);
  pack_null(2);

  /* msg type id, bitmap */
  pack_word(KTransBitmap[TX_DATA.b_trans].w_txmsg_id);
  TX_DATA.Msgtype = KTransBitmap[TX_DATA.b_trans].w_txmsg_id;

  /*bit map*/
  memcpy(tx_bitmap, KTransBitmap[TX_DATA.b_trans].sb_txbitmap, 8);
  bitmap_ptr = get_pptr();
  inc_pptr(8);

  // Send PAN and expire date if TRK2 not available of ICC trans.
  if ((TX_DATA.b_trans >= SALE_ICC) && (TX_DATA.b_trans <= AUTH_ICC)) {
    if (INPUT.s_trk2buf.b_len == 0) {
      tx_bitmap[0] |= 0x40;
      tx_bitmap[1] |= 0x04;
      tx_bitmap[4] &= ~0x20;
    }
  }

  if( TX_DATA.b_trans == EFECTIVACION_PAGO_BONO ){
	  if( strlen(INPUT.sb_cedula_ruc) > 0){
		  tx_bitmap[6] &= 0xEF;	// Deshabilito el campo 52
	  }
  }

  if( (TX_DATA.b_trans == CONSULTA_PAGO_BONO) && (strlen(INPUT.sb_cedula_ruc) == 0) ){
	  tx_bitmap[6] |= 0x10;	// Habilito el campo 52
  }

  /* 02. primary account number */
  if (tx_bitmap[0] & 0x40) {
    var_i = (BYTE)fndb(TX_DATA.sb_pan, 0xff, 10);
    if (var_i == 0xff)
      var_i = 10;
    var_j = var_i * 2;
    if ((TX_DATA.sb_pan[var_i-1] & 0x0f) == 0x0f) {
      var_j -= 1;
      if ((STIS_ACQ_TBL(0).sb_nii[0]*256+STIS_ACQ_TBL(0).sb_nii[1]) == 0x016)
        TX_DATA.sb_pan[var_i - 1] &= 0xf0;
    }
    pack_byte((BYTE)bin2bcd(var_j));
    pack_mem(TX_DATA.sb_pan, var_i);
  }

  /* 03. processing code */
  if (tx_bitmap[0] & 0x20) {
    //pack_mem(TX_DATA.sb_proc_code, 3);
	  if(TX_DATA.b_trans == EDC_REV){
		  getReverseFile();	// Lee el archivo que almacena el reverso
		  pack_mem(gRevTable.procCode, 3);
		  memcpy(TX_DATA.procCode, gRevTable.procCode,  3);
	  }
	else{
		pack_mem(KTransBitmap[TX_DATA.b_trans].b_proc_code2, 3);
		memcpy(TX_DATA.procCode, KTransBitmap[TX_DATA.b_trans].b_proc_code2,  3);
	}
  }

  /* 04. amount, transaction */
  if (tx_bitmap[0] & 0x10) {
	 if(TX_DATA.b_trans == RETIRO ){
		 if( gMultiCuenta == FALSE )
			 packField4();
		 else
		 {
			 memset( gAmount, 0x00, sizeof(gAmount) );
			 memcpy( gAmount, tmpAmount, 12 );
			 packField4();
		 }
	 }
	 else if(TX_DATA.b_trans == CONFIRMACION_RETIRO || TX_DATA.b_trans == DEPOSITO || TX_DATA.b_trans == CONFIRMACION_DEPOSITO ||
			  TX_DATA.b_trans == PAGO_SERVICIO || TX_DATA.b_trans == CONFIRMA_PAGO_SERVICIO || TX_DATA.b_trans == CONFIRMA_PAGO_SER_N
			  || TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ || TX_DATA.b_trans == CONFIRMA_PAGO_TARJETA || TX_DATA.b_trans == CONFIRMACION_PAGO_TARJ
			  || TX_DATA.b_trans == EFECTIVACION_PAGO_BONO || TX_DATA.b_trans == CONFIRMACION_PAGO_BONO || TX_DATA.b_trans == CONFIRMA_RECAUDO_DIST
			  || TX_DATA.b_trans == RECARGA || TX_DATA.b_trans == CONFIRMA_RECARGAS || TX_DATA.b_trans == PAGO_VENTA_SEGURO
			  || TX_DATA.b_trans == CONSULTA_VENTA_SEGURO_V  || TX_DATA.b_trans == COMPRA_STOCK || TX_DATA.b_trans == CONSULTA_SALDO
			  || TX_DATA.b_trans == CONSULTA_ULT_MOV || TX_DATA.b_trans == PAGO_DIST_EFECTIVO){
		 packField4();
	 }
  }

  /* 7. Transmission date & time */
  if (tx_bitmap[0] & 0x02)
  {
	  pack_mem(&TX_DATA.s_dtg.b_month, 5);
  }

  /* 11. systems trace audit number */
  if (tx_bitmap[1] & 0x20) {
    pack_mem(TX_DATA.sb_trace_no, 3);
  }

  /* 12. local transaction time */
  if (tx_bitmap[1] & 0x10) {
    pack_mem(&TX_DATA.s_dtg.b_hour, 3);
  }

  /* 13. local transaction date */
  if (tx_bitmap[1] & 0x08) {
    pack_mem(&TX_DATA.s_dtg.b_month, 2);
  }

  /* 14. expiry date */
  if (tx_bitmap[1] & 0x04) {
    pack_mem(TX_DATA.sb_exp_date, 2);
  }

  /* 22. Entry mode */
  if (tx_bitmap[2] & 0x04) {
    switch (TX_DATA.b_entry_mode) {
      case MANUAL:
        pack_word(0x0021);
        break;
      case SWIPE:
        pack_word(0x0021);
        break;
      case MANUAL_4DBC:
        pack_word(0x0021);
        break;
      case SWIPE_4DBC:
        pack_word(0x0021);
        break;
      case FALLBACK_4DBC:
        pack_word(0x0021);
        break;
      case FALLBACK:
        pack_word(0x0021);
        break;
      default :
        pack_word(0x0021);
        break;
    }
  }

  /* 24. network international identifier */
  if (tx_bitmap[2] & 0x01) {
    pack_mem((BYTE *)&STIS_ACQ_TBL(0).sb_nii, 2);
  }

  /* 25. pos condition code */
  if (tx_bitmap[3] & 0x80) {
    if ((TX_DATA.b_trans <= AUTH_MANUAL)||
        (TX_DATA.b_trans == SALE_COMPLETE) ||
        (TX_DATA.b_trans == ESALE_COMPLETE) ||
        (TX_DATA.b_trans == AUTH_ICC) ||
        (TX_DATA.b_trans == ESALE_UNDER_LMT) ||
        (TX_DATA.b_trans == SALE_UNDER_LMT))
      pack_byte(0x06);
    else
      if (((TX_DATA.b_trans == TRANS_UPLOAD)||
           (TX_DATA.b_trans == EDC_REV)
          ) &&
          ((TX_DATA.b_org_trans<=AUTH_MANUAL)   ||
           (TX_DATA.b_org_trans==ESALE_COMPLETE) ||
           (TX_DATA.b_org_trans==ESALE_UNDER_LMT) ||
           (TX_DATA.b_org_trans==SALE_COMPLETE) ||
           (TX_DATA.b_org_trans==SALE_UNDER_LMT)
          )
         )
        pack_byte(0x06);
      else
        pack_byte(0x00);
  }

  /* 32. Acquiring institution identification code */   // Jorge Numa
  if (tx_bitmap[3] & 0x01)
  {
	  pack_byte(0x04);
	  pack_word(0x0010);
	  //  if(TX_DATA.b_org_trans==CONSULTA_DEPOSITO||TX_DATA.b_org_trans==DEPOSITO||
	  // CONSULTA_PAGO_SERVICIO ||TX_DATA.b_org_trans==PAGO_SERVICIO)
	  // pack_byte(0x04);

  }

  /* 35. track 2 data */
  if (tx_bitmap[4] & 0x20) {
    if (INPUT.s_trk2buf.b_len) {
      pack_byte((BYTE)bin2bcd(INPUT.s_trk2buf.b_len));
      compress(get_pptr(),INPUT.s_trk2buf.sb_content, (BYTE)((INPUT.s_trk2buf.b_len+1)/2));
      inc_pptr((BYTE)(INPUT.s_trk2buf.b_len/2));
      if ((INPUT.s_trk2buf.b_len % 2) != 0) {
        *(BYTE *)get_pptr() |= 0x0f;
        inc_pptr(1);
      }
    } else
      tx_bitmap[4] &= ~0x20;
  }

  /* 37. authorization code */
  if (tx_bitmap[4] & 0x08) {
    if ((TX_DATA.b_trans == EDC_REV) && (TX_DATA.b_org_trans != VOID))
      tx_bitmap[4] &= ~0x08; /* not applicable, clear BIT */
    else{
      pack_str(gRrnTemporal);
      //printf("\fMi rrn = |%s|", gRrnTemporal);WaitKey(3000, 0);
    }
  }

  /* 38. authorization code */
  if (tx_bitmap[4] & 0x04) {
    pack_mem(TX_DATA.sb_auth_code, 6);
  }

  /* 39. Response code */
  if (tx_bitmap[4] & 0x02) {
    pack_str("00");
  }


  /* 41. terminal identification */
  if (tx_bitmap[5] & 0x80) {
    //pack_mem(STIS_ACQ_TBL(0).sb_term_id, 8);
	  memset(tmpSN, 0x00, sizeof(tmpSN));
	  GetSysCfgMW( MW_SYSCFG_SERIAL_NO, tmpSN );
	  pack_mem(tmpSN, 8);
	  memcpy(TX_DATA.sb_serialTerm,tmpSN,8);
  }

  /* 42. card acceptor identification */
  if (tx_bitmap[5] & 0x40) {
    //pack_mem(STIS_ACQ_TBL(0).sb_acceptor_id, 15);
	  //packField42();
	  //pack_space(15);
	  packField42();
  }

  /* 44. additional resp. data */
  if (tx_bitmap[5] & 0x10) { /* indicates under floor limit approval */
    if (TX_DATA.b_trans == TRANS_UPLOAD) {
      if ((TX_DATA.b_org_trans == REFUND) ||
          (TX_DATA.b_org_trans == SALE_OFFLINE) ||
          (TX_DATA.b_org_trans == SALE_COMPLETE) ||
          (TX_DATA.b_org_trans == SALE_UNDER_LMT) ||
          (TX_DATA.b_org_trans == ESALE_COMPLETE) ||
          (TX_DATA.b_org_trans == ESALE_UNDER_LMT)
         )
        tx_bitmap[5] &= ~0x10; /* not applicable, clear bit */
    }
//      if (((TX_DATA.b_trans == TRANS_UPLOAD) &&
//          (!((TX_DATA.b_org_trans >= REFUND) && (TX_DATA.b_org_trans <= SALE_UNDER_LMT))))
//         )
//        tx_bitmap[5] &= ~0x10; /* not applicable, clear bit */
    else /* indicates under floor limit approval */
      if ((TX_DATA.b_org_trans == SALE_UNDER_LMT) ||(TX_DATA.b_org_trans == ESALE_UNDER_LMT))
        pack_str("\x02\x30\x33");
      else
        pack_str("\x02\x30\x30");
  }

  /* 45. track 1 data */
  if (tx_bitmap[5] & 0x08) {
    // Send Track1 when available
    if ((INPUT.s_trk1buf.b_len==0)||!Track1Enable())
      tx_bitmap[5] &= ~0x08;
    else {
      pack_byte((BYTE)bin2bcd(INPUT.s_trk1buf.b_len));
      pack_mem(INPUT.s_trk1buf.sb_content, INPUT.s_trk1buf.b_len);
    }
  }

  /* 48 private field*/
  if( tx_bitmap[5] & 0x01 ){

	  if(TX_DATA.b_trans != REPRINT_DOC){
		  lenToken=0;
		  lenFieldPtr=get_pptr();
		  inc_pptr(2);
	  }
	  if(TX_DATA.b_trans == REPRINT_DOC){

	  pack_mem(gAuxStan, 12);
	  }

	  if( TX_DATA.b_trans == RETIRO )
	  {
		  // TOKEN 05
		  //lenToken+=setInfo5();

		  // TOKEN 09
		  lenToken+=setInfo9();

		  // TOKEN 43
		  lenToken+=setInfo43();
		  //auxToken=lenToken;

		  if(gMultiCuenta)
		  {
			  // TOKEN 98
			  lenToken+=setInfo98();
		  }
		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_DEPOSITO){

		  lenToken+=setInfo9();

		  lenToken+=setInfo11();

		  lenToken+=setInfo43();

		  lenToken+=setInfo47();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == DEPOSITO){

		  lenToken+=setInfo9();

		  lenToken+=setInfo11();

		  //if( TX_DATA.b_trans == CONSULTA_DEPOSITO )
		  //lenToken+=setInfo43();

		  lenToken+=setInfo47();

		  worToBcd(lenToken, lenFieldPtr);
	  }
    else if(TX_DATA.b_trans == CON_REC_DIST){
      lenToken+=setInfo3(FALSE);

      worToBcd(lenToken, lenFieldPtr);
    }
	  else if(TX_DATA.b_trans == CONSULTA_PAGO_SERVICIO){
		  lenToken+=setInfo3(FALSE);

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_PAGO_TARJETA){
		  lenToken+=setInfo3(TRUE);

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_VENTA_SEGURO){
		  lenToken+=setInfo3(FALSE);

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONFIRMA_PAGO_SER_N || TX_DATA.b_trans == CONSULTA_VENTA_SEGURO_V){
		  lenToken+=setInfo3(FALSE);

		  lenToken+=setInfo6();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONFIRMA_PAGO_TARJETA){
		  lenToken+=setInfo3(TRUE);

		  lenToken+=setInfo6();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == PAGO_SERVICIO || TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ || TX_DATA.b_trans == PAGO_VENTA_SEGURO){
		  lenToken+=setInfo3(gPagoTarjeta);

		  lenToken+=setInfo6();

		  if( memcmp(INPUT.sb_typeAccount, "00", 2) != 0)	// Si el pago es con debito cuenta
			  lenToken+=setInfo9();

		  lenToken+=setInfo43();

		  if(gMultiCuenta == TRUE){
			  lenToken+=setInfo98();
		  }

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == COMPRA_STOCK){
		  lenToken+=setInfo3(gPagoTarjeta);

		  if( memcmp(INPUT.sb_typeAccount, "00", 2) != 0)	// Si el pago es con debito cuenta
			  lenToken+=setInfo9();

		  lenToken+=setInfo43();

		  if(gMultiCuenta == TRUE){
			  lenToken+=setInfo98();
		  }

		  worToBcd(lenToken, lenFieldPtr);
	  }

	  else if(TX_DATA.b_trans == CAMBIO_CLAVE){
		  lenToken+=setInfo43();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_PAGO_BONO){
		  if( strlen(INPUT.sb_cedula_ruc) > 0)
			  lenToken+=setInfo12();

		  lenToken+=setInfo13();

		  if( strlen(INPUT.sb_cedula_ruc) == 0)
			  lenToken+=setInfo43();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == EFECTIVACION_PAGO_BONO){
		  if( strlen(INPUT.sb_cedula_ruc) > 0)
			  lenToken+=setInfo12();

		  lenToken+=setInfo13();

		  lenToken+=setInfo14();

		  if( strlen(INPUT.sb_cedula_ruc) == 0)
			  lenToken+=setInfo43();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == EDC_REV){
		  lenToken+=setInfo90();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == EMISION_CAPA_LOTES || TX_DATA.b_trans == RESUME_TX ){
		  lenToken+=setInfo91();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_COMPROBANTE){
		  lenToken += setComprobanteTrans();

		  worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == CONSULTA_ID_XPERTA || TX_DATA.b_trans == CONS_REP_XPERTA){
		  if( strlen(INPUT.sb_cedula_ruc) > 0)
			  lenToken+=setInfo12();
		 worToBcd(lenToken, lenFieldPtr);
	  }
	  else if(TX_DATA.b_trans == EFECTIVACION_CREA_XPERTA || TX_DATA.b_trans == EFECT_REP_XPERTA){

		   lenToken+=setInfo47();
		   lenToken+=setInfo48();

		  worToBcd(lenToken, lenFieldPtr);
	  }	
     else if(TX_DATA.b_trans == PAGO_DIST_EFECTIVO ){
       lenToken+=setInfo3(FALSE);
       lenToken+=setInfo5();

	   if( memcmp(INPUT.sb_typeAccount, "00", 2) != 0)	// Si el pago es con debito cuenta
		   lenToken+=setInfo9();

	   lenToken+=setInfo43();

	   if(gMultiCuenta == TRUE)
	   		lenToken+=setInfo98();

      worToBcd(lenToken, lenFieldPtr);
    }
    else if(TX_DATA.b_trans == RECARGA){
        lenToken+=setInfo3(FALSE);

 	   if( memcmp(INPUT.sb_typeAccount, "00", 2) != 0)	// Si el pago es con debito cuenta
 		   lenToken+=setInfo9();

 	   lenToken+=setInfo43();

 	   if(gMultiCuenta == TRUE)
 	   		lenToken+=setInfo98();

       worToBcd(lenToken, lenFieldPtr);
    }
    else if( TX_DATA.b_trans == CONSULTA_SALDO || TX_DATA.b_trans == CONSULTA_ULT_MOV )
    {
    	// TOKEN 09
    	lenToken+=setInfo9();

    	// TOKEN 43
    	lenToken+=setInfo43();

    	if(gMultiCuenta == TRUE)
    	{
    		// TOKEN 98
    		lenToken+=setInfo98();
    	}
    	worToBcd(lenToken, lenFieldPtr);
    }
    else if( TX_DATA.b_trans == CONSULTA_SALDOCNB ){
    	// TOKEN 43
    	lenToken+=setInfo43();
    }
	else if(TX_DATA.b_trans == IDENTIF_BP){
    	lenToken+=setInfo43();
    	worToBcd(lenToken, lenFieldPtr);
    }
  }

  /* 49. Currency code*/        // Jorge Numa
  if( tx_bitmap[6] & 0x80 )
  {
	  pack_mem( "840", 3 );
  }


  /* 52. pin block */
  if (tx_bitmap[6] & 0x10) {
    //pack_mem (TX_DATA.sb_pin, 8);
	  pack_mem (pinBlock, 8);

  }
  /* 53. pin block para nueva clave*/
  if (tx_bitmap[6] & 0x08) {
	  pack_mem (pinBlock2, 8);
  }
  /* 54. additional amount */
  if (tx_bitmap[6] & 0x04) {
    if (TX_DATA.dd_tip !=0) {
      pack_word(0x0012);
      dbin2bcd(tmp,TX_DATA.dd_tip);
      split_data(&tmp[4],6);
    }
    else
      tx_bitmap[6] &= ~0x04; /* no tip, clear BIT */
  }

  /* 55. EMV data */
  if (tx_bitmap[6] & 0x02) {
    // EMVEDC data higher priority
    if (TX_DATA.s_icc_data.w_misc_len) {
      pack_word(bin2bcd(TX_DATA.s_icc_data.w_misc_len));
      pack_mem(TX_DATA.s_icc_data.sb_misc_content, TX_DATA.s_icc_data.w_misc_len);
    }
    else
      if (Prompt4DBC() &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0220) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0400) &&
          (KTransBitmap[TX_DATA.b_trans].w_txmsg_id!=0x0320)) {
        pack_word(0x0004);
        pack_mem(INPUT.sb_amex_4DBC, 4);
      }
      else
        tx_bitmap[6] &= ~0x02; // no 4DBC
  }

  /* 60. reserved private */
  if (tx_bitmap[7] & 0x10) {
    if (TX_DATA.b_trans == SETTLEMENT) {
      pack_word(0x0006); /* length */
      split_data(STIS_ACQ_TBL(0).sb_curr_batch_no, 3);
    }
    else
      if (TX_DATA.b_trans == TRANS_UPLOAD) {
        pack_word(0x0022);
        tmp [0] = (BYTE)(KTransBitmap[TX_DATA.b_org_trans].w_txmsg_id >> 8);
        tmp [1] = (BYTE)(KTransBitmap[TX_DATA.b_org_trans].w_txmsg_id &0xF);
        split_data(tmp, 2);
        split_data(TX_DATA.sb_org_trace_no, 3);
        pack_space(12);
      }
      else { /* adjust original amount */
        pack_word(0x0012);
        dbin2bcd(tmp,TX_DATA.dd_org_amount);
        split_data(&tmp[4], 6);
      }
  }

  /* 61. private - product codes */
  if (tx_bitmap[7] & 0x08) {
    if (TX_DATA.sb_product[0] == 0xFF)
      tx_bitmap[7] &= ~0x08; /* clear BIT */
    else {
      pack_word(0x0008); /* length */
      for (var_i = 0; var_i != 4; var_i++) {
        if (TX_DATA.sb_product[var_i] != 0xFF) {
          APM_GetDescTbl(TX_DATA.sb_product[var_i], &prod);
          pack_mem((BYTE *)&prod.sb_host_tx_code, 2);
        }
        else
          pack_space(2);
      }
    }
  }

  /* 62. private - ROC number of financial/SOC number of settlement */
  if (tx_bitmap[7] & 0x04) {
    pack_word(0x0006); /* length */
    split_data(TX_DATA.sb_roc_no, 3);
  }

  /* 63. transaction counts and totals for settlement */
  if (tx_bitmap[7] & 0x02) {
	lenToken=0;

	lenFieldPtr=get_pptr();
	inc_pptr(2);
	lenToken+=setInfo1();
	worToBcd(lenToken, lenFieldPtr);
  }

  memcpy(bitmap_ptr, tx_bitmap, 8); // set bit map

  PackTxBufLen(TRUE/*ExtraMsgLen()*/);	// Set Length Iso
}
/******************************************************************/
/*  This procedure extracts or checks items in response message.  */
/*  Enter with type of message to be retrieved:                   */
/*  02. P.A.N.                                                    */
/*  03. Processing code          04. amount                       */
/*  11. system trace number      12. trans time                   */
/*  13. trans date               14. expiry date                  */
/*  15. settlement date                                           */
/*  22. pos entry mode           24. network id                   */
/*  25. pos condition code                                        */
/*  32.                          37. retrieval reference number   */
/*  38. auth code                39. response code                */
/*  41. terminal id              42. card acceptor id             */
/*  48. CITI instalment info.                                     */
/*  54. additional amount        60. init. param / original trans */
/*  61. product descriptor code  62. record of charge number      */
/******************************************************************/
BYTE CheckHostRsp(void)
{
  int intRspCode = -1;
  DWORD var_i;
  WORD msg_id;
  BYTE bitmap[8];
  BYTE rtcbuf[5];
  BYTE respCode[3];
  //BYTE tmp;
  BOOLEAN more_msg, sync_datetime;
  BYTE *ptr;
  BYTE auxiliar[15 + 1];
  //BYTE serialAux[8 + 1];

  gMsg60Show = FALSE;	//Deshabilito el flag de procesar los mensajes enviados desde el switch
  lenCupon = 0;	// pongo la longitud de los datos del tiket en cero
  memset(gToken_8.cupon, 0x00, sizeof(gToken_8.cupon));
  memset(&gToken_7.msg_largo, 0x00, 60);

  Disp2x16Msg(GetConstMsg(EDC_CO_PROCESSING), MW_LINE5, MW_CENTER|MW_BIGFONT);
  if (TrainingModeON())
    return TrainingRsp();

  RxBufSetup(TRUE/*ExtraMsgLen()*/);
  more_msg = sync_datetime = 0;
  inc_pptr(5); // 5 BYTE TO MSGID

  //printf("\fLlego 1");WaitKey(3000, 0);
  if ((msg_id=get_word()) == 0x0820)
    return 0xff; /* please wait message */
  if ((KTransBitmap[TX_DATA.b_trans].w_txmsg_id + 0x10) != msg_id) {
	MsgError( "MTI INVALIDO" );
    RSP_DATA.w_rspcode = 'I'*256+'R';
    return TRANS_FAIL;
  }
  //printf("\fLlego 2");WaitKey(3000, 0);
  get_mem(bitmap, 8);
  //serialSend2(bitmap, 8);

  /* 02. pan */
  if (bitmap[0] & 0x40) {
    var_i = bcd2bin(get_byte());
    memset(RSP_DATA.sb_pan, 0xFF, 10);
    get_mem(RSP_DATA.sb_pan, ((var_i+1)/2));
  }

  /* 03. processing code */
  if (bitmap[0] & 0x20) {
    /*if (memcmp(get_pptr(), TX_DATA.sb_proc_code, 2)!=0) {
      if ((TX_DATA.b_trans != SETTLEMENT)|| (memcmp(get_pptr(),KSetlPCode2,2)!=0)) {
        RSP_DATA.w_rspcode = 'I'*256+'P';
        return TRANS_FAIL;
      }
    }
    inc_pptr(2);
    tmp = get_byte();
    if ((TX_DATA.b_trans == TRANS_UPLOAD) && (tmp == 0x01))
      more_msg = 1;
    else if ((tmp != 0x01) && tmp) {
      RSP_DATA.w_rspcode = 'I'*256+'P';
      return TRANS_FAIL;
    }*/
		//BYTE procCode[3];

	  if (memcmp(get_pptr(), TX_DATA.procCode, 3)!=0)
	  {
		  MsgError( "PROCCODE INVALIDO" );
		  RSP_DATA.w_rspcode = 'I'*256+'P';
		  return TRANS_FAIL;
	  }
	  inc_pptr(3);

  }

  /* 04. amount */
  if (bitmap[0] & 0x10) {
	  if(TX_DATA.b_trans == CONSULTA_COMISION || TX_DATA.b_trans == CONSULTA_COMISION_2){
		  RSP_DATA.dd_amount = BcdBin8b(get_pptr(),6);
		  inc_pptr(6);
	  }
	  else{
		  if( TX_DATA.b_trans != EDC_REV ){
			  if (memcmp(get_pptr(),glocAmount, 6)!=0)
			  {
				  MsgError( "MONTO INVALIDO" );
				  RSP_DATA.w_rspcode = 'I'*256+'P';
				  return TRANS_FAIL;
			  }
		  }
		  inc_pptr(6);
		  LTrim( gAmount, '0' );
		  RSP_DATA.dd_amount = decbin8b( gAmount, strlen(gAmount));
	  }
  }

  /* 7. Transmission date & time */
  if (bitmap[0] & 0x02)
  {
	  get_mem( rtcbuf, 5);
  }

  /* 11. check system trace no */
  if (bitmap[1] & 0x20) {
    if (TX_DATA.b_trans != TRANS_UPLOAD) {
      if (!Match(TX_DATA.sb_trace_no, 3)) {
    	MsgError( "STAN INVALIDO" );
        RSP_DATA.w_rspcode = 'I'*256+'S';
        return TRANS_FAIL;
      }
    }
    else
      inc_pptr(3);
  }

  /* 12. trans time */
  if (bitmap[1] & 0x10) {
    sync_datetime |= 0x10;
    get_mem(&RSP_DATA.s_dtg.b_hour, 3);
  }

  /* 13. trans date */
  if (bitmap[1] & 0x08) {
    sync_datetime |= 0x08;
    get_mem(&RSP_DATA.s_dtg.b_month, 2);
  }

  /* 14. expiry date */
  if (bitmap[1] & 0x04) {
    get_mem(RSP_DATA.sb_exp_date, 2);
  }

  /* 24. bypass netword id */
  if (bitmap[2] & 0x01) {
    inc_pptr(2);
  }

  /* 25. pos condition code */
  if (bitmap[3] & 0x80) {
    inc_pptr(1);
  }

  /* 37. retrieval reference number */
  if (bitmap[4] & 0x08) {
    get_mem(RSP_DATA.sb_rrn, 12);
    //printf("\frrn = |%s|", RSP_DATA.sb_rrn);WaitKey(3000, 0);
    memset(gRrnTemporal, 0x00, sizeof(gRrnTemporal));
    memcpy(gRrnTemporal, RSP_DATA.sb_rrn, 12);// Lo guardo para devolverlo en las transacciones de confirmacion.
  }

  /* 38. auth code */
  memset(RSP_DATA.sb_auth_code, ' ', 6);
  if (bitmap[4] & 0x04) {
    get_mem(RSP_DATA.sb_auth_code, 6);
  }

  /* 39. response code  */
  RSP_DATA.w_rspcode = '0'*256+'0'; // upload response does not have response code
  if (bitmap[4] & 0x02) {
    //RSP_DATA.w_rspcode=get_word();
//     printf("\x1b\xc0%02x: %c%c", RSP_DATA.w_rspcode, RSP_DATA.w_rspcode>>8, RSP_DATA.w_rspcode&0xFF); //!TT
//     Delay1Sec(3, 0);
	  memset( respCode, 0x00, sizeof(respCode) );
	  get_mem( respCode, 2 );

	  intRspCode = atoi(respCode);	// Convierto el codigo de respuesta en entero.

	  //printf("RspCode: %s", respCode);WaitKey(3000, 0);

	  if( StrCmp( respCode, "00" ) == 0 )
	  {
		  RSP_DATA.w_rspcode = '0'*256+'0';
		    gMultiCuenta = FALSE;
	  }
	  else if( (TX_DATA.b_trans == INIT) && (StrCmp( respCode, "98") == 0 ) ){
		  RSP_DATA.w_rspcode = '0'*256+'0';
		  intRspCode = 0;
		  gTeleCarga = TRUE;
	  }
	  else if( (TX_DATA.b_trans == RETIRO) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  //printf("\fMulticuantas");WaitKey(3000, 0);
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == PAGO_SERVICIO) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == PAGO_VENTA_SEGURO) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == COMPRA_STOCK) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ) && (StrCmp( respCode, "97") == 0 ) ){
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == PAGO_DIST_EFECTIVO) && (StrCmp( respCode, "97") == 0 ) ){
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == RECARGA) && (StrCmp( respCode, "97") == 0 ) ){
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == CONSULTA_ID_XPERTA) && (StrCmp( respCode, "95") == 0 ) ){
		  RSP_DATA.w_rspcode = '9'*256+'5';
		  gMultiCuenta = TRUE;
	  }	
	  else if( (TX_DATA.b_trans == CONSULTA_SALDO) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
	  else if( (TX_DATA.b_trans == CONSULTA_ULT_MOV) && (StrCmp( respCode, "97") == 0 ) )
	  {
		  RSP_DATA.w_rspcode = '9'*256+'7';
		  gMultiCuenta = TRUE;
	  }
    else if( (TX_DATA.b_trans == CONSULTA_SALDOCNB) && (StrCmp( respCode, "97") == 0 ) )
    {
      RSP_DATA.w_rspcode = '9'*256+'7';
      gMultiCuenta = TRUE;
    }

    

	  else if((TX_DATA.b_trans == EDC_REV) && (StrCmp( respCode, "98") == 0 )){
		  showMessage(9, 3, "ERROR");
		  return TRANS_FAIL;
	  }
	  /*else
	  {
		  //MsgError( "ERROR NO CODIFICADO" );
		  return TRANS_FAIL;
	  }*/
  }

  /* 41. term id */
  if (bitmap[5] & 0x80) {
	  //memset(serialAux, 0x00, sizeof(serialAux));
	  //get_mem( serialAux, 8 );
	  //printf("\fSerial 1: %s\n", serialAux);WaitKey(3000, 0);
	  //printf("Serial 2: %s\n", TX_DATA.sb_serialTerm);WaitKey(3000, 0);
	  if (!Match(TX_DATA.sb_serialTerm, 8)) {
		  MsgError( "SERIAL INVALIDO" );
		  RSP_DATA.w_rspcode = 'I'*256+'T';
		  return TRANS_FAIL;
	  }
	  //inc_pptr(8);
	  //printf("\fCampo 41");WaitKey(3000, 0);
  }

  /* 42. operator code */
  if (bitmap[5] & 0x40){	// Pendiente
	  if(TX_DATA.b_trans == INIT){
		  //printf("\fCampo 42");WaitKey(3000, 0);
		  inc_pptr(15);
	  }
	  else{
		  memset(auxiliar, 0x00, sizeof(auxiliar));
		  get_mem(auxiliar, 15);
		  RTrim(auxiliar, ' ' );
		 // printf("\f42: |%s|\n|%s|", auxiliar,gUsers_tbl.sb_user);WaitKey(3000,0);
		  if( memcmp(auxiliar, gUsers_tbl.sb_user, strlen(auxiliar)) ){
			  MsgError( "OPERADOR INVALIDO" );
			  RSP_DATA.w_rspcode = '0'*256+'3';
			  return TRANS_FAIL;
		  }
	  }
  }

  /* 48. private */
  if( bitmap[5] & 0x01 ){
	  inc_pptr(2);

	  if( TX_DATA.b_trans == INIT )
	  {
		  if ( !getToken7(intRspCode) )
    		  return TRANS_FAIL;

		  if ( !getToken80() )
			  return TRANS_FAIL;

		  if (!getToken81())
			  return TRANS_FAIL;

		  if( !getToken96() )	// Token que contiene la informacion para la telecarga
			  return FALSE;

		  if (!getToken97())
			  return TRANS_FAIL;

	  }
	  else if(TX_DATA.b_trans == RETIRO || TX_DATA.b_trans == PAGO_SERVICIO || TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ  || TX_DATA.b_trans == PAGO_DIST_EFECTIVO
			  || TX_DATA.b_trans == RECARGA || TX_DATA.b_trans == PAGO_VENTA_SEGURO  || TX_DATA.b_trans == COMPRA_STOCK){
		  if( intRspCode != 97 ){
			  if ( !getToken7(intRspCode) ){
				  return TRANS_FAIL;
			  }
		  }
		  //printf("\fgMultiCuenta = %d", gMultiCuenta);WaitKey(3000, 0);
		  if( gMultiCuenta == TRUE ){
			  //printf("\fToken 98");WaitKey(3000, 0);
			  if (!getToken98())
				  return TRANS_FAIL;
		  }
	  }
	  else if( TX_DATA.b_trans == CONSULTA_DEPOSITO ){
		  //printf("\fEstoy aca...");WaitKey(3000, 0);
		  if ( !getToken7(intRspCode) )
		  	  return TRANS_FAIL;

		  if( !getToken11() )
			  return TRANS_FAIL;

		  if( !getToken47() )
			  return TRANS_FAIL;
	  }
	  else if(TX_DATA.b_trans == DEPOSITO || TX_DATA.b_trans == CAMBIO_CLAVE || TX_DATA.b_trans == EFECTIVACION_PAGO_BONO || TX_DATA.b_trans == EMISION_CAPA_LOTES
			  || TX_DATA.b_trans == CONSULTA_COMPROBANTE || TX_DATA.b_trans == RESUME_TX || TX_DATA.b_trans == CONSULTA_COMISION
			  || TX_DATA.b_trans == CONSULTA_COMISION_2){
		  if ( !getToken7(intRspCode) ){
			  return TRANS_FAIL;
		  }
	  }
	  else if(TX_DATA.b_trans == CONSULTA_PAGO_SERVICIO  || TX_DATA.b_trans == CONSULTA_PAGO_TARJETA || TX_DATA.b_trans == CONSULTA_VENTA_SEGURO){
		  if ( !getToken6() )
			  return TRANS_FAIL;

		  if ( !getToken7(intRspCode) )
			  return TRANS_FAIL;

		  if ( !getToken39() )
			  return TRANS_FAIL;
	  }
	  else if(TX_DATA.b_trans == CONFIRMA_PAGO_SER_N || TX_DATA.b_trans == CONFIRMA_PAGO_TARJETA || TX_DATA.b_trans == CONSULTA_VENTA_SEGURO_V){
		  if ( !getToken6() )
			  return TRANS_FAIL;

		  if ( !getToken7(intRspCode) )
			  return TRANS_FAIL;
	  }
	  else if(TX_DATA.b_trans == CONSULTA_PAGO_BONO){
		  if ( !getToken7(intRspCode) )
			  return TRANS_FAIL;

		  if ( !getToken14() )
			  return TRANS_FAIL;
	  }
	  else if( TX_DATA.b_trans == CONSULTA_ID_XPERTA || TX_DATA.b_trans == EFECTIVACION_CREA_XPERTA || TX_DATA.b_trans == CONS_REP_XPERTA
			  || TX_DATA.b_trans == EFECT_REP_XPERTA){
		  if( intRspCode != 95 ){
			  if ( !getToken7(intRspCode) ){
				  return TRANS_FAIL;
			  }
		  }
		  if( !getToken47() )
			  return TRANS_FAIL;
	  }
	  else if(TX_DATA.b_trans == IDENTIF_BP){

		  if ( !getToken7(intRspCode) )
			  return TRANS_FAIL;

		  if( !getToken15() )
			  return TRANS_FAIL;
	  }
    else if(TX_DATA.b_trans == CON_REC_DIST){
    	if( !getToken5() )
    		return TRANS_FAIL;

    	if ( !getToken7(intRspCode) )
			  return TRANS_FAIL;
    }
    else if(TX_DATA.b_trans == CONSULTA_SALDO || TX_DATA.b_trans == CONSULTA_ULT_MOV || TX_DATA.b_trans == CONSULTA_SALDOCNB){
    	if( intRspCode != 97 ){
    		if ( !getToken7(intRspCode) ){
    			return TRANS_FAIL;
    		}
        	if( !getToken47() )
    			  return TRANS_FAIL;
    	}
    	if( gMultiCuenta == TRUE ){
    		if (!getToken98())
    			return TRANS_FAIL;
    	}
    }
  }
  /* 54. additional amount */
  RSP_DATA.dd_tip=0;
  if (bitmap[6] & 0x04) {
    RSP_DATA.dd_tip = BcdBin8b(get_pptr(),(BYTE)(bcd2bin(get_word())/2));
  }

  /* 55. EMV relative data */
  RSP_DATA.s_icc_data.w_misc_len = 0;
  if (bitmap[6] & 0x02) {
    RSP_DATA.s_icc_data.w_misc_len  = (BYTE) bcd2bin(get_word());
    if (RSP_DATA.s_icc_data.w_misc_len > sizeof(RSP_DATA.s_icc_data.sb_misc_content)) {
      RSP_DATA.w_rspcode = '9'*256 + '6';
      return(TRANS_FAIL);
    }
    if (RSP_DATA.s_icc_data.w_misc_len > 6) {
      inc_pptr(6);
      ptr = RSP_DATA.s_icc_data.sb_misc_content;
      *ptr++ = 0x91;  // issuer auth. data
      var_i = *ptr++ = get_byte();
      get_mem(ptr, var_i);
      ptr += var_i;
      if (RSP_DATA.s_icc_data.w_misc_len > 7 + var_i) {
        // script
        var_i = get_byte();
        get_mem(ptr, var_i);
        ptr += var_i;
      }
      RSP_DATA.s_icc_data.w_misc_len = ptr - RSP_DATA.s_icc_data.sb_misc_content;
    }
    else
      RSP_DATA.s_icc_data.w_misc_len = 0;
  }

   /* 60. private field */
  if ((bitmap[7] & 0x10) != 0) {
    gMsg60Show = TRUE;
    var_i = bcd2bin(get_word());
    if(var_i > 200){
    	MsgError( "ERROR EN MSG PARA POS" );
    	gMsg60Show = FALSE;
    	return TRANS_FAIL;
    }
    memset(gMsgPOS, 0x00, sizeof(gMsgPOS));
    get_mem( gMsgPOS, var_i );
    ReplaceLF( gMsgPOS );
    SaveMsg60( var_i );
  }


  /* 61. private field */
  if (bitmap[7] & 0x08){
	  memset(&gToken_8, 0x00, sizeof(struct TOKEN_8));
	  inc_pptr(2);
	  if( TX_DATA.b_trans == RETIRO || TX_DATA.b_trans == PAGO_SERVICIO || TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ || TX_DATA.b_trans == CAMBIO_CLAVE
			  || TX_DATA.b_trans == EFECTIVACION_PAGO_BONO || TX_DATA.b_trans == EMISION_CAPA_LOTES || TX_DATA.b_trans == CONSULTA_COMPROBANTE ||
			  TX_DATA.b_trans == EFECTIVACION_CREA_XPERTA || TX_DATA.b_trans == EFECT_REP_XPERTA || TX_DATA.b_trans == RESUME_TX || TX_DATA.b_trans == IDENTIF_BP
			  || TX_DATA.b_trans == PAGO_DIST_EFECTIVO  || TX_DATA.b_trans == RECARGA || TX_DATA.b_trans == PAGO_VENTA_SEGURO
			  || TX_DATA.b_trans == COMPRA_STOCK || TX_DATA.b_trans == CONSULTA_SALDO || TX_DATA.b_trans == CONSULTA_ULT_MOV || TX_DATA.b_trans == CONSULTA_SALDOCNB)
	  {
		  if ( !getToken8() )
			  return TRANS_FAIL;
	  }
  }
  /* 62. private field */
  if (bitmap[7] & 0x04) {
	  /*inc_pptr(2);
	  memset(RSP_DATA.sb_roc_no, 0xFF, 3);
	  var_i = bcd2bin(get_word());
	  if ((var_i == 6) && (peek_byte() != ' '))
		  compress(RSP_DATA.sb_roc_no, get_pptr(), 3);
	  inc_pptr(var_i);*/
	  inc_pptr(2);
	  if( TX_DATA.b_trans == RETIRO || TX_DATA.b_trans == DEPOSITO || TX_DATA.b_trans == PAGO_SERVICIO || TX_DATA.b_trans == EFECTIVACION_PAGO_TARJ
			  || TX_DATA.b_trans == CAMBIO_CLAVE || TX_DATA.b_trans == EFECTIVACION_PAGO_BONO || TX_DATA.b_trans == EMISION_CAPA_LOTES
			  || TX_DATA.b_trans == CONSULTA_COMPROBANTE || TX_DATA.b_trans == EFECTIVACION_CREA_XPERTA || TX_DATA.b_trans == EFECT_REP_XPERTA
			  || TX_DATA.b_trans == RESUME_TX || TX_DATA.b_trans == IDENTIF_BP || TX_DATA.b_trans == PAGO_DIST_EFECTIVO
			  || TX_DATA.b_trans == RECARGA || TX_DATA.b_trans == RECARGA || TX_DATA.b_trans == PAGO_VENTA_SEGURO
			  || TX_DATA.b_trans == COMPRA_STOCK || TX_DATA.b_trans == CONSULTA_SALDO || TX_DATA.b_trans == CONSULTA_ULT_MOV ){

		  if ( !getToken8() )
			  return TRANS_FAIL;
	  }
  }

  /* 63. private field */
  if ((bitmap[7] & 0x02) != 0) {
    //var_i = bcd2bin(get_word());
    //printf("\fLen63 = %d", var_i);APM_WaitKey(3000, 0);
	  inc_pptr(2);
	/*if( TX_DATA.b_trans == INIT || TX_DATA.b_trans == RETIRO || TX_DATA.b_trans == CONSULTA_DEPOSITO
			|| TX_DATA.b_trans == DEPOSITO){
		if ( !getToken1() )
			return TRANS_FAIL;
		//printf("\fToken 1");WaitKey(3000, 0);

	}*/

	if ( !getToken1() )
		return TRANS_FAIL;

	//if(TX_DATA.b_trans == RETIRO || TX_DATA.b_trans == CONSULTA_DEPOSITO || TX_DATA.b_trans == DEPOSITO){
	if(TX_DATA.b_trans != INIT){
		if (!getToken80())
			return TRANS_FAIL;
	}
  }

  if (sync_datetime == 0x18) // Date/time receive from Host.
    SyncHostDtg();
  if ((RSP_DATA.w_rspcode == '0'*256+'0') ||
      (RSP_DATA.w_rspcode == '0'*256+'8') ||
      (RSP_DATA.w_rspcode == '8'*256+'8') )
  {
    STIS_ACQ_TBL(0).b_status = UP;
    /*if (more_msg)
      return MORE_RSP;*/
    //printf("\fFIestas del carmen");WaitKey(3000, 0);
    return TRANS_ACP;
  }
  else if (RSP_DATA.w_rspcode == 'Z'*256+'1') {
    RSP_DATA.w_rspcode = '0'*256+'0';
    STIS_ACQ_TBL(0).b_status = DOWN;
    return TRANS_ACP;
  }
  else
    return TRANS_REJ;
}




