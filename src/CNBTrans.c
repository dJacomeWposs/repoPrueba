/*
 * CNBTrans.c
 *
 *  Created on: Jul 6, 2012
 *      Author: danijacm
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "sysutil.h"
#include "Corevar.h"
#include "tranutil.h"
#include "CNBTrans.h"
#include "CNBmain.h"
#include "midware.h"
#include "hostmsg.h"
#include "constant.h"
#include "record.h"
//#include "IPCfg.h"
#include "input.h"
#include "keytrans.h"
#include "key2dll.h"
#include "tranutil.h"
#include "menu.h"
#include "reversal.h"
#include "rs232.h"
#include "FUNC.H"
#include "lptutil.h"
#include "usbkbd.h"

BOOLEAN validarPagoDeSeguro(void);

//============ MENU PAGO TARJETA ===============
static const struct MENU_ITEM KPagTarjetaItem [] =
{
  {  1, "Diners"},
  {  2, "VISA"},
  {  3, "MasterCard"},
  {  4, "Otras"},
  {-1, NULL},
};

static const struct MENU_DAT KPagTarjetaMenu  = {
  "PAGO DE TARJETA",
  KPagTarjetaItem,
};



BOOLEAN pagTarjetaMenu(BYTE *tipoTarjeta, BYTE *titulo)
{
  int select = 0;
  BOOLEAN salir = FALSE;

  while(salir == FALSE)
  {
    select = MenuSelect(&KPagTarjetaMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  if (!gRecaudoVar)
    	      strcpy(titulo, "DINERS");
    	  strcpy(tipoTarjeta, "01");
    	  salir = TRUE;
        break;

      case 2:
    	  if (!gRecaudoVar)
    		  strcpy(titulo, "VISA");
    	  strcpy(tipoTarjeta, "02");
    	  salir = TRUE;
        break;

      case 3:
    	  if (!gRecaudoVar)
    		  strcpy(titulo, "MASTERCARD");
    	  strcpy(tipoTarjeta, "03");
    	  salir = TRUE;
        break;

      case 4:
          /*if (!gRecaudoVar)
        	  strcpy(titulo, "OTRAS");
          salir = TRUE;*/
    	  return FALSE;
        break;
    }
  }
  return TRUE;
}




DWORD InitMsg( BOOLEAN aFromMenu )
{
	BYTE *pMsgInit;
	int lenMsg;
	int indice = 0;
	BYTE cont = 0;
#if(CX_TCP == FALSE)
	struct MW_NIF_INFO nif_info;
#endif
	//BYTE wk2[16];
	//memset(wk2, 0x00, sizeof(wk2));
	//memcpy( wk2, "\xD8\x65\x77\xE7\x45\x18\x2B\x3A\x50\xF2\xCF\xA3\x61\x9F\xFA\x33", 16);

	if(gTerminalInit == 1){	// Daniel 10/07/12
		gShowMsgInit = 0;
		return 1;
	}

	INPUT.b_trans = INIT;

/*********TOMA DE TRACE SERIAL***************/
	/*ReadRTC( &INPUT.s_dtg );
	IncTraceNoCNB();
	MoveInput2Tx();
	PackHostMsg();
	serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
	printf("\fFin de trace...");WaitKey(3000, 0);*/
/*******************************************/

	//APM_ResetComm();
	printf("\f");
	//APM_ResetComm();
	PackComm(TRUE, FALSE);
	APM_PreConnect();
#if(CX_TCP == FALSE)
	PackComm(TRUE, FALSE);
    APM_ConnectPPP(TRUE);


	 NetInfoMW(MW_NIF_PPP, &nif_info);/// Sacar Informacion de la IP actual de la sim
     DispTermIPX((BYTE *)&nif_info);
   //printf("IP %s\n",g_ip);APM_WaitKey(3000,0); 



   if(g_ip1 == 172 && g_ip2 == 18 && (g_ip3 >= 152 && g_ip3 <= 159)){/// Rango IP Movistar APN Secundario
	gTcpIp.cxPrimaria = FALSE;
	PackComm(TRUE, FALSE);
 	}
#endif
	ClearResponse();
	//UpdateHostStatus(REV_PENDING);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		//ReversalOK();
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		// RxBufSetup(TRUE);
		//PackAck();        // TEST - PENDIENTE QUITAR
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			//printf("\fYa envio el trace %d", RX_BUF.wLen);WaitKey(3000, 0);
			RSP_DATA.b_response = CheckHostRsp();   // Unpack Response
			if( RSP_DATA.b_response != TRANS_ACP ){
				/*APM_ResetComm();
				showMessage(1, 3, "ERROR");
				return -1;*/
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SMFONT|MW_CENTER );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	gTerminalInit = TRUE;
/***************TRACE LLAVES********************/
	/*serialSend2(gtoken_97.worKeyPin, 16);
	printf("\fTrace 1");WaitKey(3000, 0);
	serialSend2(gtoken_97.worTrack2, 16);*/
/**********************************************/
	KDLL_KeyReset(EKEY_IDX);		//NOHORA 10Dic
	KDLL_KeyReset(CARDKEY_IDX);		//NOHORA 10Dic

	//serialSend2(gtoken_97.worKeyPin, 16);
	LoadEWKPin( gtoken_97.worKeyPin ); 		 // Inject EWK for Pin
	//serialSend2(gtoken_97.worTrack2, 16);
	LoadEWKCard( gtoken_97.worTrack2 );     // // Inject EWK for Card (Track II)
	//memset(wk2, 0x00, sizeof(wk2));
	//memcpy(wk2, gtoken_97.worTrack2, 16);
	//serialSend2(wk2, 16);

	SaveKeysFile(&gtoken_97);	//NOHORA 10Dic

	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));        // Close Socket and more :)
	if (aFromMenu)
	{	printf("\f");
		DispPutCMW(K_PushCursor);
		os_disp_backc(COLOR_BLACK);
		os_disp_textc(COLOR_YELLOW);

		
		graphicWindow("BIENVENIDOS");
		lenMsg = strlen(gMsgInit);
		while(indice < lenMsg){
			pMsgInit = strtok( gMsgInit + indice, "@" );
			  indice += strlen( pMsgInit ) + 1;
			  switch(cont){
				case 0:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE3, MW_SMFONT| MW_CENTER);
					break;
				case 1:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE4, MW_SMFONT| MW_CENTER);
					break;
				case 2:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE5, MW_SMFONT| MW_CENTER);
					break;
				case 3:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE6, MW_SMFONT| MW_CENTER);
					break;
				case 4:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE7, MW_SMFONT| MW_CENTER);
					break;
				case 5:
					LTrim( pMsgInit, ' ' );
					DispLineMW( pMsgInit, MW_LINE8, MW_SMFONT| MW_CENTER);
					break;
			  }
			  cont++;
		}
		WaitKey(3000, 0);
		gShowMsgInit = 1;  // Daniel 10/07/12
		DispPutCMW(K_PopCursor);
	}
	return 1;
}



DWORD RetiroTrans( WORD aTypeAccount )
{
	BYTE kbdbuff[5];
	INPUT.b_trans = RETIRO;
	DDWORD costo_servicio = 0;
	BYTE tmpPan[21];
	BYTE msgPin[] = "DIGITE SU CLAVE";
	int retPin;
	BYTE panBCD[10];
	//BYTE costoServicio[8 + 1];
	BYTE aux[10];
	int minAmount;
	int maxAmount;


	PackComm(TRUE, FALSE);
	APM_PreConnect();

	memcpy( INPUT.sb_operador, gUsers_tbl.sb_user, strlen(gUsers_tbl.sb_user) );
	//memcpy( INPUT.sb_typeAccount, gCuenta, 2 );
	sprintf( INPUT.sb_typeAccount, "%02d", aTypeAccount );

	DispLineMW( "RETIRO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );

	DispLineMW( "Monto:", MW_LINE3, MW_LEFT|MW_SMFONT );
	DispLineMW( "Costo Servicio:", MW_LINE6, MW_LEFT|MW_SPFONT );
	memset(aux, 0x00, sizeof(aux));
	split(aux, gToken_80.costoServicio, 2);
	costo_servicio=atoi(aux);
	DispAmnt( costo_servicio, MW_LINE6, MW_SPFONT );
	DispLineMW( "TOTAL:", MW_LINE7, MW_LEFT|MW_SMFONT );

	if( !GetAmnt(MW_LINE4) ){
		APM_ResetComm();
		return FALSE;
	}

	memset(aux, 0x00, sizeof(aux));
	split(aux, gToken_80.valMinRetiro, 2);
	minAmount=atoi(aux);

	memset(aux, 0x00, sizeof(aux));
	split(aux, gToken_80.valMaxRetiro, 2);
	maxAmount=atoi(aux);

	/*printf("\fMin = %d\n", minAmount);
	printf("Max = %d\n",   maxAmount);
	printf("Monto = %d",   (INPUT.dd_amount/100));
	WaitKey(3000, 0);*/

	if( !validateAmount("RETIROS",  (INPUT.dd_amount/100), minAmount, maxAmount) ){
		APM_ResetComm();
		return FALSE;
	}

	memcpy( tmpAmount, gAmount, 12 );

	DispAmnt(INPUT.dd_amount + costo_servicio, MW_LINE7, MW_SMFONT);

	if( SiNo() != 2 ) {  // 2 = Press Enter
		APM_ResetComm();
		return -1;
	}

	sprintf( INPUT.sb_costo_serv, "08%d", costo_servicio );

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "DESLICE LA TARJETA", MW_LINE3, MW_CENTER|MW_SMFONT );
	DispLineMW( "DEL CLIENTE", MW_LINE4, MW_CENTER|MW_SMFONT );
	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}

	do
	{
		DispClrBelowMW(MW_LINE2);
		DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
		DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
		DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
			APM_ResetComm();
			return -1;
		}

		// Compara los ultimo 4 digitos del Pan

		memset( tmpPan, 0x00, sizeof(tmpPan) );
		split( tmpPan, INPUT.sb_pan, 10 );

		RTrim( tmpPan, 'F');

	}while( memcmp( &kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4 ) != 0 );

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Monto:", MW_LINE3 + 4, MW_SMFONT );
#if(NEW_MIDWARE == TRUE)
	CPutCharMW(MW_LPUSH_CURSOR);
#else
	DispPutCMW(K_PushCursor);
#endif
	os_disp_backc(COLOR_BLACK);
	os_disp_textc(COLOR_YELLOW);
	DispAmnt(INPUT.dd_amount + costo_servicio, MW_LINE3, MW_SMFONT);
#if(NEW_MIDWARE == TRUE)
	CPutCharMW(MW_LPOP_CURSOR);
#else
	DispPutCMW(K_PopCursor);
#endif
	DispGotoMW( MW_LINE5 + 3, MW_SMFONT );

	memset( panBCD, 0x00, sizeof(panBCD));
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;    // PIN fix len 4
	// PINBLOCK
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );

	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}
	/*
    printf("\fPINBLOCK:|%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X|", panBCD[0], panBCD[1], panBCD[2], panBCD[3],
        panBCD[4], panBCD[5], panBCD[6], panBCD[7], panBCD[8], panBCD[9] );
    GetChar();
	 */
	memcpy( pinBlock, panBCD, 8 );
	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			//printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			//  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

			/*if (RSP_DATA.b_response == TRANS_FAIL){
				APM_ResetComm();
				return -1;
			}*/
			//UpdateHostStatus(NO_PENDING);
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;   // Multiples cuentas - Arma el menu por fuera de la funcion
			}
			else if (RSP_DATA.b_response == TRANS_ACP)
			{
			   //	SaveRecord();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				/*APM_ResetComm();
				printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	/*if (ReversalOK2())
	{
		APM_ResetComm();
		return -1;
	}*/
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta = FALSE;
	return TRUE;
}






DWORD RetiroMultiCuentaTrans( void )
{
	INPUT.b_trans = RETIRO;

	gMultiCuenta = TRUE;
	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				//SaveRecord();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE3, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	/*if (ReversalOK2())
	{
		APM_ResetComm();
		return -1;
	}*/
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP)/*, TRUE*/ );
	gMultiCuenta = FALSE;
	return TRUE;
}



DWORD ConfirmaRetiroTrans( void )
{

	INPUT.b_trans = CONFIRMACION_RETIRO;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
		APM_SendMsg(&TX_BUF);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP)/*, FALSE*/ );
	procesarMensajePOS();
	return TRUE;
}




DWORD ConsultaDepositoTrans( BYTE aCedRuc )
{
	BYTE kbdbuff[11];
	BYTE msgPin[] = "DIGITE SU CLAVE";
	int retPin, retCedulaRuc;
	BYTE panBCD[10];
	BYTE nombreCuenta[32 + 1];
	BYTE aux[10];
	int minAmount;
	int maxAmount;

	INPUT.b_trans = CONSULTA_DEPOSITO;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	memcpy( INPUT.sb_operador, gUsers_tbl.sb_user, strlen(gUsers_tbl.sb_user) );
	memcpy( INPUT.sb_typeAccount, gCuenta, 2 );


	DispLineMW( "DEPOSITO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );

	if( aCedRuc == 1 )
	{
		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		DispLineMW( "Ingrese Cedula", MW_LINE3, MW_CENTER );
		APM_SetCol(4); // Set Columna for TextBox
		if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(1) + IMAX(10), kbdbuff) )
		{
			APM_ResetComm();
			return -1;
		}

	    if( kbdbuff[0] < 10 )
	    {
	      MsgError("CEDULA INVALIDA!!");
	      APM_ResetComm();
	      return -1;
	    }

		memset(INPUT.sb_cedula_ruc,0x00,sizeof(INPUT.sb_cedula_ruc));
		strcpy( INPUT.sb_cedula_ruc, &kbdbuff[1] );

		// Se valida la cedula
		retCedulaRuc = ValidaCedulaRuc( INPUT.sb_cedula_ruc );
		if( (retCedulaRuc == 0) || (retCedulaRuc == -1) )
		{
			MsgError( "CEDULA INVALIDA!" );
			APM_ResetComm();
			return -1;
		}
		//strcat( INPUT.sb_cedula_ruc, "     " );
		// RSetStr( INPUT.sb_cedula_ruc, 15, ' ' );
		// memset( INPUT.sb_cedula_ruc, 0x00, sizeof(INPUT.sb_cedula_ruc) );
	}
	else
	{
		DispLineMW( "Ingrese Ruc", MW_LINE3, MW_CENTER );
		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		APM_SetCol(3); // Set Columna for TextBox
		if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(10) + IMAX(13), kbdbuff) )
		{
			APM_ResetComm();
			return -1;
		}

	    if( kbdbuff[0] < 13 )
	    {
	      MsgError("R.U.C. INVALIDO !");
	      APM_ResetComm();
	      return -1;
	    }

		strcpy( INPUT.sb_cedula_ruc, &kbdbuff[1] );

		// Se valida la Ruc
		retCedulaRuc = ValidaCedulaRuc( INPUT.sb_cedula_ruc );
		if( (retCedulaRuc == 0) || (retCedulaRuc == -1) )
		{
			MsgError( "R.U.C. INVALIDO !" );
			APM_ResetComm();
			return -1;
		}
		//strcat( INPUT.sb_cedula_ruc, "  " );
		//RSetStr( INPUT.sb_cedula_ruc, 15, ' ' );
		//memset( INPUT.sb_cedula_ruc, 0x00, sizeof(INPUT.sb_cedula_ruc) );
	}
	DispClrBelowMW(MW_LINE3);
	DispLineMW( "Nro. de Cuenta", MW_LINE3, MW_CENTER|MW_SMFONT );
	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	APM_SetCol(3); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(10) + IMAX(14), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}
	memset(INPUT.sb_num_cuenta, 0x00, sizeof(INPUT.sb_num_cuenta));
	strcpy( INPUT.sb_num_cuenta, &kbdbuff[1] );
	//RSetStr( INPUT.sb_num_cuenta, 14, ' ' );	// Revisar

	DispLineMW( "Monto:", MW_LINE7 + 4, MW_SMFONT );
	if( !GetAmnt( MW_LINE8 ) ){
		APM_ResetComm();
		return FALSE;
	}

	memset(aux, 0x00, sizeof(aux));
	split(aux, gToken_80.valMinDeposito, 2);
	minAmount=atoi(aux);

	memset(aux, 0x00, sizeof(aux));
	split(aux, gToken_80.valMaxDeposito, 2);
	maxAmount=atoi(aux);

	if( !validateAmount("DEPOSITOS",  (INPUT.dd_amount/100), minAmount, maxAmount) ){
		APM_ResetComm();
		return FALSE;
	}

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "DESLICE LA TARJETA", MW_LINE3, MW_CENTER|MW_SMFONT );
	DispLineMW( "DEL CNB", MW_LINE4, MW_CENTER|MW_SMFONT );
	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Monto:", MW_LINE3 + 4, MW_SMFONT );
	DispPutCMW(K_PushCursor);
	os_disp_backc(COLOR_BLACK);
	os_disp_textc(COLOR_YELLOW);
	DispAmnt(INPUT.dd_amount, MW_LINE3, MW_SMFONT);
	DispPutCMW(K_PopCursor);
	DispGotoMW( MW_LINE5 + 3, MW_SMFONT );

	memset( panBCD, 0x00, 10 );
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;	// PIN fix len 4

	// PINBLOCK
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );

	
	/*
  printf("\fPINBLOCK:|%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X|", panBCD[0], panBCD[1], panBCD[2], panBCD[3],
      panBCD[4], panBCD[5], panBCD[6], panBCD[7], panBCD[8], panBCD[9] );
  GetChar();
	 */
	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}


	memset( pinBlock, 0x00, sizeof(pinBlock) );
	memcpy( pinBlock, panBCD, 8 );

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		/*serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		printf("\fEnvio trace");WaitKey(3000, 0);*/
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			//if (RSP_DATA.b_response <= TRANS_REJ)
			//UpdateHostStatus(NO_PENDING);
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				APM_ResetComm();
				memset(nombreCuenta, 0x00, sizeof(nombreCuenta));
				memcpy( nombreCuenta, gToken_11.nombre_credito, 32 );
				//printf("\f|%s|", nombreCuenta);WaitKey(3000, 0);
				RTrim( nombreCuenta, ' ' );
				DispLineMW( "CUENTA PERTENECIENTE", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_SMFONT );
				DispLineMW( nombreCuenta, MW_LINE5, MW_CENTER|MW_SPFONT );
				DispLineMW( "Correcto?", MW_LINE7, MW_CENTER|MW_SMFONT );
				//WaitKey(3000, 0);
				if( SiNo() != 2 )   // 2 = Press Enter
				{
					APM_ResetComm();
					return -1;
				}
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SMFONT|MW_CENTER );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	//TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	//APM_ResetComm();
	return TRUE;
}



DWORD DepositoTrans( void )
{
	INPUT.b_trans = DEPOSITO;
	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "DEPOSITO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP)
			{
			   //	SaveRecord();
			   SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				showMessageError();
				/*APM_ResetComm();
				printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				//printf("\fReturn -1");WaitKey(3000, 0);
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	/*if (ReversalOK2())
	{
		APM_ResetComm();
		return -1;
	}*/
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



DWORD ConfirmaDepositoTrans( void )
{

	INPUT.b_trans = CONFIRMACION_DEPOSITO;
	//gMsg60Show = FALSE;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}



DWORD ConsultaPagoServiciosTrans( void )
{
	BYTE kbdbuff[21];
	BYTE empresa[21];
	BYTE nmPersona[21]; //LFLF
	BYTE monto[13];
	DDWORD dd_monto;
	BYTE titulo[26];
	WORD cardLen = 0;
	if (gRecaudoVar == FALSE)
	{
		if( gPagoTarjeta == TRUE )       // SI ES PAGO DE TARJETA DE CREDITO
			INPUT.b_trans = CONSULTA_PAGO_TARJETA;
		else
			INPUT.b_trans = CONSULTA_PAGO_SERVICIO;
	}
	else
	{
		if( gPagoTarjeta == TRUE )       // SI ES PAGO DE TARJETA DE CREDITO
			INPUT.b_trans = CONFIRMA_PAGO_TARJETA;
		else
			INPUT.b_trans = CONFIRMA_PAGO_SER_N;
	}

	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	memset( empresa, 0x00, sizeof(empresa) );
	memset( nmPersona, 0x00, sizeof(nmPersona) );
	memset( monto, 0x00, sizeof(monto) );
	memset( titulo, 0x00, sizeof(titulo) );

    //memset(INPUT.sb_numtarjeta, 0x20, sizeof(INPUT.sb_numtarjeta));
	if(gPagoTarjeta == FALSE)
		strcpy(INPUT.sb_tipotarjeta, "00");

	PackComm(TRUE, FALSE);
	APM_PreConnect();


	if( gRecaudoVar == FALSE ){
		if( gPagoTarjeta ){
			memset( INPUT.sb_tipotarjeta, 0x00, sizeof(INPUT.sb_tipotarjeta) );
			memset(INPUT.sb_numtarjeta, 0x00, sizeof(INPUT.sb_numtarjeta));
			if( !pagTarjetaMenu(INPUT.sb_tipotarjeta, titulo) ){
				APM_ResetComm();
				return -1;
			}

			DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
			DispClrBelowMW(MW_LINE2);
			DispLineMW( "Ingrese Numero de", MW_LINE3, MW_CENTER|MW_SPFONT );
			DispLineMW( "Tarjeta:", MW_LINE4, MW_CENTER|MW_SPFONT );

			if( memcmp(INPUT.sb_tipotarjeta, "00", 2) == 0 )
				cardLen = 19;
			else if( memcmp(INPUT.sb_tipotarjeta, "01", 2) == 0 )
				cardLen = 14;
			else
				cardLen = 16;

			//APM_SetCol(3); // Set Columna for TextBox
			/*if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(4) + IMAX(cardLen), kbdbuff)){
				APM_ResetComm();
				return -1;
			}*/

			if( !APM_GetKbdSpectra(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(4) + IMAX(cardLen), kbdbuff) )
			{
				APM_ResetComm();
				return -1;
			}

			memcpy(INPUT.sb_numtarjeta, &kbdbuff[1], kbdbuff[0]);
			RSetStr( INPUT.sb_numtarjeta, 42, ' ');
			//printf("\f%s", INPUT.sb_numtarjeta);WaitKey(3000, 0);
			if( SiNo() != 2 ) {  // 2 = Press Enter
				APM_ResetComm();
				return -1;
			}
		}
		else{
			DispLineMW( "RECAUDACIONES", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
			DispClrBelowMW(MW_LINE2);
			DispLineMW( "COD DE EMPRESA/ SERVICIO", MW_LINE3, MW_CENTER );
			APM_SetCol(4); // Set Columna for TextBox
			if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(1) + IMAX(10), kbdbuff) )
			{
				APM_ResetComm();
				return -1;
			}
			memset( INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa) );
			memcpy( INPUT.sb_cod_empresa, &kbdbuff[1], kbdbuff[0] );
			LSetStr( INPUT.sb_cod_empresa, 10, '0' );

			memset( kbdbuff, 0x00, sizeof(kbdbuff) );
			DispLineMW( "Num. CONTRAPARTIDA", MW_LINE7, MW_CENTER );
			//ECHO
			if( !APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SMFONT + MW_LINE8, IMIN(1) + IMAX(20), kbdbuff) )
			{
				APM_ResetComm();
				return -1;
			}

			memset( INPUT.sb_contrapar, 0x00, sizeof(INPUT.sb_contrapar) );
			memcpy( INPUT.sb_contrapar, &kbdbuff[1], kbdbuff[0] );
			RSetStr( INPUT.sb_contrapar, 34, ' ' );

		}
	}

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		//UpdateHostStatus(NO_PENDING);// Asi esta en CREON
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			//if (RSP_DATA.b_response <= TRANS_REJ)
			//UpdateHostStatus(NO_PENDING);
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				AcceptBeep();
				if (gToken_39.flagVarFijo == 'N') // si es N es porque es variable
				{
					gToken_39.flagVarFijo = 'S';
					APM_ResetComm();
					return 2;
				}
				memcpy( empresa, gToken_6.nomEmpresa, 20 );
				memcpy( nmPersona, gToken_6.nomPersona, 20 ); //LFLF

				split(monto, gToken_6.valor_documento, 6);
				dd_monto = atoi(monto);
				memset(monto, 0x00, sizeof(monto));

				DispClrBelowMW(MW_LINE2);
				DispLineMW( "Empresa:", MW_LINE2 + 2, MW_LEFT|MW_SPFONT );
				DispLineMW( empresa, MW_LINE3, MW_SPFONT );
				//DispLineMW( gToken_6.nomPersona, MW_LINE4, MW_SPFONT|MW_CENTER );
				DispLineMW( nmPersona, MW_LINE4, MW_SPFONT|MW_CENTER );//LFLF

				DispLineMW( "Monto :", MW_LINE5, MW_LEFT|MW_SMFONT );
				//LTrim( monto, '0' );
				//dd_monto = decbin8b( monto, strlen(monto));
				DispAmnt(dd_monto, MW_LINE5, MW_SMFONT);

				DispLineMW( "Servicio:", MW_LINE6, MW_LEFT|MW_SPFONT );
				//memcpy( monto, gToken_6.costo_servicio, 12 );
				//LTrim( monto, '0' );
				split(monto, gToken_6.costo_servicio, 6);
				dd_monto = atoi(monto);
				memset(monto, 0x00, sizeof(monto));
				//dd_monto = decbin8b( monto, strlen(monto));
				DispAmnt(dd_monto, MW_LINE6, MW_SMFONT);

				DispLineMW( "TOTAL:", MW_LINE7, MW_LEFT|MW_SMFONT );
				//memcpy( monto, gToken_6.valor_adeudado, 12 );
				split(monto, gToken_6.valor_adeudado, 6);
				dd_monto = atoi(monto);
				//LTrim( monto, '0' );
				//dd_monto = decbin8b( monto, strlen(monto));
				DispAmnt(dd_monto, MW_LINE7, MW_SMFONT);
				DispLineMW( "Pagar?", MW_LINE8, MW_CENTER|MW_SMFONT );

				if( SiNo() != 2 ){   // 2 = Press Enter
					APM_ResetComm();
					return -1;
				}
			}
			else{
				gMultiCuenta = FALSE;
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SMFONT|MW_CENTER );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	APM_ResetComm();
	//memcpy( tmpAmount, gToken_6.valor_adeudado, 12 );
	memset(tmpAmount, 0x00, sizeof(tmpAmount));
	split(tmpAmount, gToken_6.valor_adeudado, 6);
	//APM_ResetComm();
	//TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP) );
	return TRUE;
}



DWORD PagoServiciosTrans(void)
{
	BYTE rtcbuf[15];
	int select = 0;
	int sel;
	BYTE msgPin[] = "DIGITE SU CLAVE";
	BYTE panBCD[10];
	BYTE monto[13];
	BYTE kbdbuff[5];
	BYTE tmpPan[21];
	BYTE titulo[26];
	int retPin;
	memset(titulo, 0x00, sizeof(titulo));
	memset(&INPUT.s_trk1buf, 0x00, sizeof(INPUT.s_trk1buf));
	memset(&INPUT.s_trk2buf, 0x00, sizeof(INPUT.s_trk2buf));

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	select = MenuRecaudaciones();
	if (select == -1)
	{
	  return -1;
	}
	if( gPagoTarjeta )
	{
	  INPUT.b_trans = EFECTIVACION_PAGO_TARJ;
	  strcpy(titulo, "PAGO DE TARJETA");
	}
	else
	{
	  INPUT.b_trans = PAGO_SERVICIO;
	  strcpy(titulo, "RECAUDACIONES");
	}
	//printf("\fTrans = %d", INPUT.b_trans);WaitKey(3000, 0);
	DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispClrBelowMW(MW_LINE2);

	switch (select)
	{
	case 1: // Efectivo
		DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_SMFONT|MW_CENTER );
		DispLineMW( "DEL CNB", MW_LINE4, MW_SMFONT|MW_CENTER );
		strcpy(INPUT.sb_typeAccount, "00");
		break;

	case 2: // Debito a cuenta
		sel = 0;
		sel = MenuRecaudaCuenta();
		switch (sel)
		{
		case 1:
			strcpy(INPUT.sb_typeAccount, "01");
			break;
		case 2:
			strcpy(INPUT.sb_typeAccount, "02");
			break;
		case 3:
			strcpy(INPUT.sb_typeAccount, "03");
			break;
		}
		//ClrScr();
		//DispClrBelowMW(MW_LINE2);
		printf("\f");
		DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_SMFONT|MW_CENTER );
		DispLineMW( "DEL CLIENTE", MW_LINE4, MW_SMFONT|MW_CENTER );
		break;
	}

	//PackComm(TRUE, FALSE);
	//APM_PreConnect();

	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}

	if (select == 2)
	{
		memset(titulo, 0x00, sizeof(titulo));
		do
		{
			//ClrScr();
			if( gPagoTarjeta )
			  strcpy(titulo, "PAGO DE TARJETA");
			else
			  strcpy(titulo, "RECAUDACION");

			DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
			DispClrBelowMW(MW_LINE2);
			DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
			DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
			DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

			memset(kbdbuff, 0x00, sizeof(kbdbuff));
			APM_SetCol(7); // Set Columna for TextBox
			if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
				APM_ResetComm();
				return -1;
			}

			// Compara los ultimo 4 digitos del Pan

			memset( tmpPan, 0x00, sizeof(tmpPan) );
			split( tmpPan, INPUT.sb_pan, 10 );

			RTrim( tmpPan, 'F');

		} while (memcmp(&kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4) != 0);
	}
	memset(gAmount, 0x00, sizeof(gAmount));
	memset(monto, 0x00, sizeof(monto));
	memset(tmpAmount, 0x00, sizeof(tmpAmount));
	//memcpy(gAmount, gToken_6.valor_adeudado, 12);
	split(gAmount, gToken_6.valor_adeudado, 6);
	split(monto,   gToken_6.valor_adeudado, 6);
	memcpy(tmpAmount, gAmount, 12);
	//printf("\fAmount = %s", tmpAmount);WaitKey(3000, 0);
	//memcpy(monto, gToken_6.valor_adeudado, 12);
	//memcpy(INPUT.sb_costo_serv, gToken_6.costo_servicio + 4, 8);
	//LTrim(monto, '0');
	//INPUT.dd_amount = decbin8b(monto, strlen(monto));
	INPUT.dd_amount = atoi(monto);

	//ClrScr();
	memset(titulo, 0x00, sizeof(titulo));
	if( gPagoTarjeta )
		strcpy(titulo, "PAGO DE TARJETA");
	else
		strcpy(titulo, "RECAUDACIONES" );

	DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Monto:", MW_LINE2 + 4, MW_LEFT|MW_SMFONT );
	DispAmnt(INPUT.dd_amount, MW_LINE3, MW_SMFONT);

	memset( panBCD, 0x00, sizeof(panBCD));
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;    // PIN fix len 4
	// PINBLOCK
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );

	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}


	memcpy( pinBlock, panBCD, 8 );

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		memset(rtcbuf, 0, sizeof(rtcbuf));
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			//printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			//  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;   // Multiples cuentas - Arma el menu por fuera de la funcion
			}
			else if (RSP_DATA.b_response == TRANS_ACP)
			{
				//SaveRecord();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				showMessageError();
				/*APM_ResetComm();
				printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	/*if (ReversalOK2())
	{
		APM_ResetComm();
		return -1;
	}*/
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta = FALSE;
	return TRUE;
}




DWORD PagoServicioMultiCuentaTrans(void)
{
  if( gPagoTarjeta )
  {
    INPUT.b_trans = EFECTIVACION_PAGO_TARJ;
    //graphicWindow("PAGO DE TARJETA");
  }
  else
  {
	  INPUT.b_trans = PAGO_SERVICIO;
    //graphicWindow("RECAUDACIONES");
  }
  PackComm(TRUE, FALSE);
  APM_PreConnect();

  ClearResponse();

  if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
  {
	  if( !reversalPichinchaOk() ){
		  APM_ResetComm();
		  return -1;
	  }
	  ReadRTC( &INPUT.s_dtg );
	  IncTraceNoCNB();
	  MoveInput2Tx();
	  PackHostMsg();
	  saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
	  //UpdateHostStatus(REV_PENDING);
	  //SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
	  //serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
	  if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
	  {
		  //printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
		  //serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
		  RSP_DATA.b_response = CheckHostRsp();
		  //  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

		  if (RSP_DATA.b_response == TRANS_ACP)
		  {
			 // SaveRecord();
			 SaveTransactionPichincha();
			  PackVoucher( FALSE );
		  }
		  else
		  {
			  //UpdateHostStatus(NO_PENDING);
			  clearReversoPichincha();
			  gMultiCuenta = FALSE;
			  APM_ResetComm();
			  /*printf("\f");
			  graphicWindow("ERROR");
			  LongBeep();
			  DispPutCMW(K_PushCursor);
			  os_disp_textc(COLOR_RED);
			  DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SMFONT|MW_CENTER );
			  DispPutCMW(K_PopCursor);
			  gMultiCuenta = FALSE;
			  WaitKey(3000, 0);*/
			  showMessageError();
			  return -1;
		  }
	  }
	  else{
		  APM_ResetComm();
		  showMessage(2, 3, "TIEMPO FUERA");
		  return -1;
	  }
  }
  else
  {
	  APM_ResetComm();
	  APM_DisconnectPPP();
	  showMessage(5, 3, "ERROR DE CONEXION");
	  gMultiCuenta = FALSE;
	  return -1;
  }
  /*if (ReversalOK2())
  {
	  APM_ResetComm();
	  return -1;
  }*/
  TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
  gMultiCuenta = FALSE;
  return TRUE;
}




DWORD ConfirmaPagoServicioTrans( void )
{
	INPUT.b_trans = CONFIRMA_PAGO_SERVICIO;
	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}



DWORD ConfirmaPagoTarjetaTrans( void )
{
	INPUT.b_trans = CONFIRMACION_PAGO_TARJ;
	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}



DWORD CambioClaveTrans( void )
{
	BYTE tmpPinBlock[17];
	BYTE msgPin[] = "DIGITE CLAVE ACTUAL";
	BYTE msgPin2[] = "DIGITE NUEVA CLAVE";
	BYTE msgPin3[] = "CONFIRME CLAVE";
	int retPin;
	BYTE panBCD[10];

	INPUT.b_trans = CAMBIO_CLAVE;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "CAMBIO DE CLAVE", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_CENTER|MW_BIGFONT );

	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}
	DispLineMW( "CAMBIO DE CLAVE", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);

	DispClrBelowMW(MW_LINE2);
	memset( panBCD, 0x00, sizeof(panBCD));
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;    // PIN fix len 4
	// PINBLOCK
	DispGotoMW( MW_LINE4, MW_SMFONT );
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );
	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}
	//memcpy(INPUT.sb_pin, panBCD, 8);
	memset( pinBlock, 0x00, sizeof(pinBlock) );
	memcpy(pinBlock, panBCD, 8);
	do
	{
		DispClrBelowMW(MW_LINE2);
		memset( panBCD, 0x00, sizeof(panBCD) );
		memcpy( panBCD, INPUT.sb_pan, 10 );
		panBCD[10] = 0x01;    // PIN fix len 4
		DispGotoMW( MW_LINE4, MW_SMFONT );
		retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin2, EKEY_IDX );
		if(retPin != 0){
			APM_ResetComm();
			return -1;
		}

		memset(pinBlock2, 0x00, sizeof(pinBlock2));
		memcpy( pinBlock2, panBCD, 8 );

		DispClrBelowMW(MW_LINE2);
		memset( panBCD, 0x00, sizeof(panBCD) );
		memcpy( panBCD, INPUT.sb_pan, 10 );
		panBCD[10] = 0x01;    // PIN fix len 4
		DispGotoMW( MW_LINE4, MW_SMFONT );
		retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin3, EKEY_IDX );
		if( retPin == 2 ){
			APM_ResetComm();
			return -1;
		}
		memset(tmpPinBlock, 0x00, sizeof(tmpPinBlock));
		memcpy( tmpPinBlock, panBCD, 8 );
	}while( memcmp( pinBlock2, tmpPinBlock, 8 ) != 0 );

	if( memcmp( pinBlock, pinBlock2, 8 ) == 0 ){
		showMessage(17, 5, "ERROR");
		APM_ResetComm();
		return -1;
	}

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			//if (RSP_DATA.b_response <= TRANS_REJ)
			//UpdateHostStatus(NO_PENDING);
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				PackVoucher( FALSE );
			}
			else{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_CENTER );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}

DWORD identificarBPTrans( void )
{
	BYTE auxCedula[10 +1], auxCargo[30 +1], auxNombre[50 +1];
	BYTE kbdbuff[11], tmpPan[21];
	BYTE msgPin[] = "DIGITE CLAVE";
	//BYTE msgPin2[] = "DIGITE NUEVA CLAVE";
	//BYTE msgPin3[] = "CONFIRME CLAVE";
	int retPin;
	BYTE panBCD[10];

	INPUT.b_trans = IDENTIF_BP;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "IDENTIFICACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_CENTER|MW_SMFONT );
	DispLineMW( "DEL FUNCIONARIO", MW_LINE4, MW_CENTER|MW_SMFONT );

	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}

	do
	{
		DispClrBelowMW(MW_LINE2);
		DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
		DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
		DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
			APM_ResetComm();
			return -1;
		}

		// Compara los ultimo 4 digitos del Pan

		memset( tmpPan, 0x00, sizeof(tmpPan) );
		split( tmpPan, INPUT.sb_pan, 10 );

		RTrim( tmpPan, 'F');

	}while( memcmp( &kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4 ) != 0 );




	DispLineMW( "IDENTIFICACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);

	DispClrBelowMW(MW_LINE2);
	memset( panBCD, 0x00, sizeof(panBCD));
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;    // PIN fix len 4
	// PINBLOCK
	DispGotoMW( MW_LINE4, MW_SMFONT );
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );
	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}
	//memcpy(INPUT.sb_pin, panBCD, 8);
	memset( pinBlock, 0x00, sizeof(pinBlock) );
	memcpy(pinBlock, panBCD, 8);
	
	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			//if (RSP_DATA.b_response <= TRANS_REJ)
			//UpdateHostStatus(NO_PENDING);
			if (RSP_DATA.b_response == TRANS_ACP){
				memset(auxCedula,0x00,sizeof(auxCedula));
				memset(auxCargo,0x00,sizeof(auxCargo));
				memset(auxNombre,0x00,sizeof(auxNombre));
				memcpy(auxNombre, gToken_15.nombre, 24);
				memcpy(auxCargo, gToken_15.cargo, 24);
				split(auxCedula,   gToken_15.cedula, 5);
				RTrim( auxCedula, 'F' );

				DispLineMW( "IDENTIFICACION BP", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
				DispClrBelowMW(MW_LINE2);
				DispLineMW( "Nombre:", MW_LINE2, MW_LEFT|MW_SPFONT );
				DispLineMW( auxNombre, MW_LINE3, MW_SPFONT );
				DispLineMW( "Cargo:", MW_LINE4, MW_LEFT|MW_SPFONT );
				DispLineMW( auxCargo, MW_LINE5, MW_SPFONT );
				DispLineMW( "Cedula:", MW_LINE6, MW_LEFT|MW_SPFONT );
				DispLineMW( auxCedula, MW_LINE7, MW_SPFONT );
				APM_WaitKey(3000,0);	

				PackVoucher( FALSE );
			}
			else{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_CENTER );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}




DWORD pagoBonoConCedulaTrans(void)
{
	BYTE kbdbuff[12];
	BYTE nombre[50 + 1];
	BYTE valor[12 + 1];
	BYTE tipoSubsidio[15 + 1];
	BYTE informacion[60 + 1];
	BYTE periodo[8 + 1];
	BYTE periodoFin[8 + 1];
	int retCedulaRuc;
	DDWORD dd_monto;

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(nombre, 0x00, sizeof(nombre));
	memset(valor, 0x00, sizeof(valor));
	memset(tipoSubsidio, 0x00, sizeof(tipoSubsidio));
	memset(informacion, 0x00, sizeof(informacion));
	memset(periodo, 0x00, sizeof(periodo));
	memset(periodoFin, 0x00, sizeof(periodoFin));

	INPUT.b_trans = CONSULTA_PAGO_BONO;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "PAGO BONO DESARROLLO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Ingrese No Cedula", MW_LINE3, MW_LEFT|MW_SPFONT );
	DispLineMW( "Del Beneficiario",  MW_LINE4, MW_LEFT|MW_SPFONT );
	memset(kbdbuff, 0x00, sizeof(kbdbuff));

	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(1) + IMAX(10), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}

    if( kbdbuff[0] < 10 )
    {
      MsgError("CEDULA INVALIDA!!");
      APM_ResetComm();
      return -1;
    }

	memset(INPUT.sb_cedula_ruc,0x00,sizeof(INPUT.sb_cedula_ruc));
	strcpy( INPUT.sb_cedula_ruc, &kbdbuff[1] );

	// Se valida la cedula
	retCedulaRuc = ValidaCedulaRuc( INPUT.sb_cedula_ruc );
	if( (retCedulaRuc == 0) || (retCedulaRuc == -1) )
	{
		MsgError( "CEDULA INVALIDA!" );
		APM_ResetComm();
		return -1;
	}
	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		/*serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		printf("\fEnvio trace");WaitKey(3000, 0);*/
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				memcpy(nombre, gToken_14.nombre, 50);
				//memcpy(valor, gToken_14.valor, 12);
				split(valor,   gToken_14.valor, 6);
				split(periodo,   gToken_14.periodo, 4);
				split(periodoFin,   gToken_14.periodoFin, 4);
				dd_monto = atoi(valor);

				INPUT.dd_amount = dd_monto;

				memcpy(tipoSubsidio, gToken_14.tipo_sub, 15);
				memcpy(informacion, gToken_14.informacion, 60);
				RTrim(nombre, ' ');
				RTrim(informacion, ' ');
				RTrim(tipoSubsidio, ' ');

				DispLineMW( "PAGO DE BONO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
				DispClrBelowMW(MW_LINE2);
				DispLineMW( "Nombre:", MW_LINE2, MW_LEFT|MW_SPFONT );
				DispLineMW( nombre, MW_LINE3, MW_SPFONT );
				DispLineMW( "Valor:", MW_LINE4, MW_LEFT|MW_SPFONT );
				DispAmnt(dd_monto, MW_LINE4, MW_SMFONT);
				DispLineMW( "Tipo Subsidio:", MW_LINE5, MW_LEFT|MW_SPFONT );
				DispLineMW( tipoSubsidio, MW_LINE6, MW_SPFONT );
				DispLineMW( "Periodo:", MW_LINE7, MW_LEFT|MW_SPFONT );
				DispLineMW( periodo, MW_LINE7 + 9, MW_LEFT|MW_SPFONT );
				DispLineMW( "Periodo Fin:", MW_LINE8, MW_LEFT|MW_SPFONT );
				DispLineMW( periodoFin, MW_LINE8 + 12, MW_LEFT|MW_SPFONT );

				memset(gAmount, 0x00, sizeof(gAmount));
				memcpy(gAmount, valor, 12);

				if( SiNo() != 2 )   // 2 = Press Enter
				{
					APM_ResetComm();
					return -1;
				}
				if( efectivacionPagoBonoTrans() == -1 )
					return -1;

				ConfirmaPagoBonoTrans();
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	//TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	APM_ResetComm();
	return TRUE;
}




DWORD pagoBonoConTarjetaTrans(void)
{
	BYTE kbdbuff[12];
	BYTE msgPin[] = "DIGITE SU CLAVE";
	BYTE panBCD[10];
	BYTE nombre[50 + 1];
	BYTE valor[12 + 1];
	BYTE tipoSubsidio[15 + 1];
	BYTE informacion[60 + 1];
	BYTE periodo[8 + 1];
	BYTE periodoFin[8 + 1];															
	BYTE tmpPan[21];
	int retPin;
	DDWORD dd_monto;

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(nombre, 0x00, sizeof(nombre));
	memset(valor, 0x00, sizeof(valor));
	memset(tipoSubsidio, 0x00, sizeof(tipoSubsidio));
	memset(informacion, 0x00, sizeof(informacion));
	memset(periodo, 0x00, sizeof(periodo));
	memset(periodoFin, 0x00, sizeof(periodoFin));
	memset(tmpPan, 0x00, sizeof(tmpPan));
	memset(INPUT.sb_cedula_ruc,0x00,sizeof(INPUT.sb_cedula_ruc));// Limpio el campo de cedula por que se hara con tarjeta

	INPUT.b_trans = CONSULTA_PAGO_BONO;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "PAGO BONO  DESARROLLO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "DESLICE LA TARJETA", MW_LINE3, MW_CENTER|MW_SMFONT );
	DispLineMW( "MIES", MW_LINE4, MW_CENTER|MW_SMFONT );
	if( GetCard( FALSE, FALSE ) == FALSE ){
		APM_ResetComm();
		return FALSE;
	}

	do
	{
		DispClrBelowMW(MW_LINE2);
		DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
		DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
		DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
			APM_ResetComm();
			return -1;
		}

		// Compara los ultimo 4 digitos del Pan

		memset( tmpPan, 0x00, sizeof(tmpPan) );
		split( tmpPan, INPUT.sb_pan, 10 );

		RTrim( tmpPan, 'F');

	}while( memcmp( &kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4 ) != 0 );

	DispClrBelowMW(MW_LINE2);
	memset( panBCD, 0x00, sizeof(panBCD));
	memcpy( panBCD, INPUT.sb_pan, 10 );
	panBCD[10] = 0x01;    // PIN fix len 4
	// PINBLOCK
	retPin = KDLL_GetX98PIN( (BYTE*)panBCD, (BYTE*)msgPin, EKEY_IDX );
	if(retPin != 0){
		APM_ResetComm();
		return -1;
	}

	memcpy( pinBlock, panBCD, 8 );


	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		/*serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		printf("\fEnvio trace");WaitKey(3000, 0);*/
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				memcpy(nombre, gToken_14.nombre, 50);
				//memcpy(valor, gToken_14.valor, 12);
				split(valor,   gToken_14.valor, 6);
				split(periodo,   gToken_14.periodo, 4);
				split(periodoFin,   gToken_14.periodoFin, 4);
				dd_monto = atoi(valor);

				INPUT.dd_amount = dd_monto;
				
				memcpy(tipoSubsidio, gToken_14.tipo_sub, 15);
				memcpy(informacion, gToken_14.informacion, 60);
				RTrim(nombre, ' ');
				RTrim(informacion, ' ');
				RTrim(tipoSubsidio, ' ');

				DispLineMW( "PAGO DE BONO", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
				DispClrBelowMW(MW_LINE2);
				DispLineMW( "Nombre:", MW_LINE2, MW_LEFT|MW_SPFONT );
				DispLineMW( nombre, MW_LINE3, MW_SPFONT );
				DispLineMW( "Valor:", MW_LINE4, MW_LEFT|MW_SPFONT );
				DispAmnt(dd_monto, MW_LINE4, MW_SMFONT);
				DispLineMW( "Tipo Subsidio:", MW_LINE5, MW_LEFT|MW_SPFONT );
				DispLineMW( tipoSubsidio, MW_LINE6, MW_SPFONT );
				DispLineMW( "Periodo:", MW_LINE7, MW_LEFT|MW_SPFONT );
				DispLineMW( periodo, MW_LINE7 + 9, MW_LEFT|MW_SPFONT );
				DispLineMW( "Periodo Fin:", MW_LINE8, MW_LEFT|MW_SPFONT );
				DispLineMW( periodoFin, MW_LINE8 + 12, MW_LEFT|MW_SPFONT );

				memset(gAmount, 0x00, sizeof(gAmount));
				memcpy(gAmount, valor, 12);

				if( SiNo() != 2 )   // 2 = Press Enter
				{
					APM_ResetComm();
					return -1;
				}
				if( efectivacionPagoBonoTrans() == -1 )
					return -1;

				ConfirmaPagoBonoTrans();
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	//TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	APM_ResetComm();
	return TRUE;
}





DWORD efectivacionPagoBonoTrans(void){
	INPUT.b_trans = EFECTIVACION_PAGO_BONO;
	ClearResponse();
				

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				//SaveRecord();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				APM_ResetComm();
				showMessageError();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	/*if (ReversalOK2())
	{
		APM_ResetComm();
		return -1;
	}*/
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}




DWORD ConfirmaPagoBonoTrans( void )
{
	INPUT.b_trans = CONFIRMACION_PAGO_BONO;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}




DWORD CoberturaLoteTrans(void){

	BYTE FechaIni[9 + 2];
	BYTE HoraIni[7 + 2];
	BYTE FechaFinal[9 + 2];
	BYTE HoraFinal[7 + 2];
	//BYTE campo48ini[13];
	//BYTE campo48fin[13];

	INPUT.b_trans = EMISION_CAPA_LOTES;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	memset(FechaIni, 0x00, sizeof(FechaIni));
	memset(HoraIni, 0x00, sizeof(HoraIni));
	memset(FechaFinal, 0x00, sizeof(FechaFinal));
	memset(HoraFinal, 0x00, sizeof(HoraFinal));
	//memset(campo48ini, 0x00, sizeof(campo48ini));
	//memset(campo48fin, 0x00, sizeof(campo48fin));
	memset(&gToken_91, 0x00, sizeof(struct TOKEN_91));

	DispLineMW( "COBERTURA LOTE", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "INICIO", MW_LINE2, MW_LEFT|MW_SPFONT|MW_CENTER );
	//DispLineMW( "<AAAAMMDD>", MW_LINE4 + 7, MW_SPFONT );
	DispLineMW( "Fecha<AAAAMMDD>", MW_LINE4 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(8) + IMAX(8), FechaIni)){
		APM_ResetComm();
		return -1;
	}
	DispLineMW( "Hora<HHMMSS>", MW_LINE7 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE8, IMIN(6) + IMAX(6), HoraIni)){
		APM_ResetComm();
		return -1;
	}

	sprintf(gToken_91.fechaHoraInicial, "%s%s", &FechaIni[1], &HoraIni[1]);

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "FIN", MW_LINE2, MW_LEFT|MW_SPFONT|MW_CENTER );
	//DispLineMW( "<AAAAMMDD>", MW_LINE4 + 7, MW_SPFONT );
	DispLineMW( "Fecha<AAAAMMDD>", MW_LINE4 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(8) + IMAX(8), FechaFinal)){
		APM_ResetComm();
		return -1;
	}
	DispLineMW( "Hora<HHMMSS>", MW_LINE7 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE8, IMIN(6) + IMAX(6), HoraFinal)){
		APM_ResetComm();
		return -1;
	}
	sprintf(gToken_91.fechaHoraFinal, "%s%s", &FechaFinal[1], &HoraFinal[1]);

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				PackVoucher( FALSE );
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



DWORD ConsultaComprobanteTrans(void){
	BYTE transNumbre[18];

	memset(transNumbre, 0x00, sizeof(transNumbre));
	INPUT.b_trans = CONSULTA_COMPROBANTE;


	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "CONSULTA COMPROBANTE", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);

	DispLineMW( "Ingrese el Documento", MW_LINE3 + 2, MW_SPFONT );
	DispLineMW( "De transaccion: ", MW_LINE4 + 2, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(6) + IMAX(12), transNumbre)){
		APM_ResetComm();
		return -1;
	}
	memset(gRrnTemporal, 0x00, sizeof(gRrnTemporal));
	memcpy(gRrnTemporal, &transNumbre[1], transNumbre[0]);

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				PackVoucher( FALSE );
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



DWORD resumenVentaProd(void){

	BYTE FechaIni[9 + 2];
	BYTE HoraIni[7 + 2];
	BYTE FechaFinal[9 + 2];
	BYTE HoraFinal[7 + 2];
	BYTE kbdbuff[6 + 2];
	//BYTE campo48ini[13];
	//BYTE campo48fin[13];

	INPUT.b_trans = RESUME_TX;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	memset(FechaIni, 0x00, sizeof(FechaIni));
	memset(HoraIni, 0x00, sizeof(HoraIni));
	memset(FechaFinal, 0x00, sizeof(FechaFinal));
	memset(HoraFinal, 0x00, sizeof(HoraFinal));
	//memset(campo48ini, 0x00, sizeof(campo48ini));
	//memset(campo48fin, 0x00, sizeof(campo48fin));
	memset(&gToken_91, 0x00, sizeof(struct TOKEN_91));

	DispLineMW( "VENTA PRODUCTOS", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Ingrese codigo", MW_LINE2 + 5, MW_SPFONT );
	DispLineMW( "de producto:", MW_LINE3 + 5, MW_SPFONT );
	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(6) + IMAX(6), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}
	memset(gToken_91.codigoProducto,0x00,6);
	//compress(gToken_91.codigoProducto, &kbdbuff[1], 3);
	memcpy(gToken_91.codigoProducto, &kbdbuff[1], 6);

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "INICIO", MW_LINE2, MW_LEFT|MW_SPFONT|MW_CENTER );
	//DispLineMW( "<AAAAMMDD>", MW_LINE4 + 7, MW_SPFONT );
	DispLineMW( "Fecha<AAAAMMDD>", MW_LINE3 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(8) + IMAX(8), FechaIni)){
		APM_ResetComm();
		return -1;
	}
	DispLineMW( "Hora<HHMMSS>", MW_LINE6 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE8, IMIN(6) + IMAX(6), HoraIni)){
		APM_ResetComm();
		return -1;
	}

	sprintf(gToken_91.fechaHoraInicial, "%s%s", &FechaIni[1], &HoraIni[1]);

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "FIN", MW_LINE2, MW_LEFT|MW_SPFONT|MW_CENTER );
	//DispLineMW( "<AAAAMMDD>", MW_LINE4 + 7, MW_SPFONT );
	DispLineMW( "Fecha<AAAAMMDD>", MW_LINE3 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(8) + IMAX(8), FechaFinal)){
		APM_ResetComm();
		return -1;
	}
	DispLineMW( "Hora<HHMMSS>", MW_LINE6 + 5, MW_SPFONT );
	APM_SetCol(5); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE8, IMIN(6) + IMAX(6), HoraFinal)){
		APM_ResetComm();
		return -1;
	}
	sprintf(gToken_91.fechaHoraFinal, "%s%s", &FechaFinal[1], &HoraFinal[1]);

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				PackVoucher( FALSE );
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}


DWORD reimpresionDoc(void){

	BYTE kbdbuff[6 + 2];
	//BYTE campo48ini[13];
	//BYTE campo48fin[13];

	INPUT.b_trans = REPRINT_DOC;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "REIMPRESION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Ingrese codigo", MW_LINE2 + 5, MW_SPFONT );
	DispLineMW( "de documento:", MW_LINE3 + 5, MW_SPFONT );
	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(6) + IMAX(6), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}
	memset(gAuxStan,'0',12);
	memcpy(gAuxStan,&kbdbuff[1],6);

	 
   	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				PackVoucher( FALSE );
			}
			else
			{
				APM_ResetComm();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}




/*******************************XPERTA ************************************************/

DWORD consultaXperta(BYTE opcion)
{
	BYTE kbdbuff[50];
	BYTE nombre[30 + 1];
	BYTE cedulaAux[16 + 1];
	BYTE cuentaAux[20];
	int retCedulaRuc;
	int idPort;
	BYTE auxCed_kbdbuff[20];		//NOHORA 23Nov
	BYTE auxNombre[30];

	memset(kbdbuff,   0x00, sizeof(kbdbuff));
	memset(nombre, 	  0x00, sizeof(nombre));
	memset(cuentaAux, 0x00, sizeof(cuentaAux));
	memset(cedulaAux, 0x00, sizeof(cedulaAux));
	memset(auxCed_kbdbuff, 0x00, sizeof(auxCed_kbdbuff));		//NOHORA 23Nov
	memset(auxNombre, 0x00, sizeof(auxNombre));

	if(opcion == 1)
		INPUT.b_trans = CONSULTA_ID_XPERTA;
	else
		INPUT.b_trans = CONS_REP_XPERTA;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	DispLineMW( "CUENTA XPERTA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Ingrese Identificacion", MW_LINE3, MW_LEFT|MW_SPFONT );
	DispLineMW( "Del Cliente",  MW_LINE4, MW_LEFT|MW_SPFONT );
	memset(kbdbuff, 0x00, sizeof(kbdbuff));

	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(1) + IMAX(10), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}

	if( kbdbuff[0] < 10 )
	{
		MsgError("CEDULA INVALIDA!!");
		APM_ResetComm();
		return -1;
	}

	memset(INPUT.sb_cedula_ruc,0x00,sizeof(INPUT.sb_cedula_ruc));
	strcpy( INPUT.sb_cedula_ruc, &kbdbuff[1] );

	strcpy(auxCed_kbdbuff, kbdbuff);		//NOHORA 23Nov

	// Se valida la cedula
	retCedulaRuc = ValidaCedulaRuc( INPUT.sb_cedula_ruc );
	if( (retCedulaRuc == 0) || (retCedulaRuc == -1) )
	{
		MsgError( "CEDULA INVALIDA!" );
		APM_ResetComm();
		return -1;
	}
	ClearResponse();
	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP || RSP_DATA.w_rspcode == '9'*256+'5')
			{
				if(gMultiCuenta == TRUE){/// si llega response code 95
					memset( kbdbuff, 0x00, sizeof(kbdbuff) );
					if( openExtKeyboard(&idPort) ){
						memset(&miCapturaExt, 0x00, sizeof(miCapturaExt));
						strcpy(miCapturaExt.titulo, "CUENTA XPERTA");
						strcpy(miCapturaExt.label, "Apellido:");
						miCapturaExt.colIni = 2;
						miCapturaExt.colFin = 24;
						miCapturaExt.minChar = 1;
						miCapturaExt.maxChar = 14;

						if( !getSExtKeyboard(idPort, kbdbuff, auxCed_kbdbuff) ){	//NOHORA 23Nov Modifique esta func. -> getSExtKeyboard
							closeExtKeyBoard(idPort);
							APM_ResetComm();
							return FALSE;
						}
						/*if(strlen(kbdbuff) <= 0){
							printf("\f");
							DispLineMW( "TRANSACCION CANCELDA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );	//NOHORA 20Nov
							DispLineMW( "No se ha digitado", MW_LINE4, MW_CENTER );
							DispLineMW( "nombre de cliente", MW_LINE5, MW_CENTER );
							closeExtKeyBoard(idPort);
							Delay10ms(200);
							APM_ResetComm();
							return FALSE;
						}*/
						memset(gToken_47.nombre,0x20,30);
						//memcpy(gToken_47.nombre, kbdbuff, strlen(kbdbuff));
						memcpy(auxNombre, kbdbuff, strlen(kbdbuff));
						strcat(auxNombre, " ");

						memset( kbdbuff, 0x00, sizeof(kbdbuff) );
						memset(&miCapturaExt, 0x00, sizeof(miCapturaExt));
						strcpy(miCapturaExt.titulo, "CUENTA XPERTA");
						strcpy(miCapturaExt.label, "Nombre:");
						miCapturaExt.colIni = 2;
						miCapturaExt.colFin = 24;
						miCapturaExt.minChar = 1;
						miCapturaExt.maxChar = 15;

						if( !getSExtKeyboard(idPort, kbdbuff, auxCed_kbdbuff) ){	//NOHORA 23Nov Modifique esta func. -> getSExtKeyboard
							closeExtKeyBoard(idPort);
							APM_ResetComm();
							return FALSE;
						}
						/*if(strlen(kbdbuff) <= 0){
							printf("\f");
							DispLineMW( "TRANSACCION CANCELDA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );	//NOHORA 20Nov
							DispLineMW( "No se ha digitado", MW_LINE4, MW_CENTER );
							DispLineMW( "nombre de cliente", MW_LINE5, MW_CENTER );
							closeExtKeyBoard(idPort);
							Delay10ms(200);
							APM_ResetComm();
							return FALSE;
						}*/

						strcat(auxNombre, kbdbuff);
						memcpy(gToken_47.nombre, auxNombre, strlen(auxNombre));
						memset(gToken_47.cedula_ruc,0xFF,8);		//NOHORA 23Nov desde aca

						closeExtKeyBoard(idPort);

						if( auxCed_kbdbuff[0] % 2 != 0 ){
							auxCed_kbdbuff[ auxCed_kbdbuff[0] + 1 ] = 'F';
							auxCed_kbdbuff[0] += 1;
						}
						compress(gToken_47.cedula_ruc, &auxCed_kbdbuff[1], (auxCed_kbdbuff[0]/2));	//NOHORA 23Nov hasta aca

					}
					else{
						DispLineMW( "CUENTA XPERTA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
						//DispLineMW( "Por favor ingrese", MW_LINE2, MW_CENTER );
						DispLineMW( "Apellido:", MW_LINE3, MW_LEFT );
						DispLineMW( "No de CC/CI:", MW_LINE7, MW_LEFT|MW_SPFONT );	//NOHORA 23Nov
						DispLineMW( &auxCed_kbdbuff[1], MW_LINE8, MW_SPFONT );	//NOHORA 23Nov
						if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+MW_LINE4+MW_BIGFONT+RIGHT_JUST, IMIN(1) + IMAX(14), kbdbuff)){
							APM_ResetComm();
							return -1;
						}
						/*if(kbdbuff[0] <= 0){
							printf("\f");
							DispLineMW( "TRANSACCION CANCELDA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );	//NOHORA 20Nov
							DispLineMW( "No se ha digitado", MW_LINE4, MW_CENTER );
							DispLineMW( "nombre de cliente", MW_LINE5, MW_CENTER );
							Delay10ms(200);
							APM_ResetComm();
							return FALSE;
						}*/
						memset(auxNombre, 0x00, sizeof(auxNombre));
						memset(gToken_47.nombre,0x20,30);
						memcpy(auxNombre, &kbdbuff[1], kbdbuff[0]);
						strcat(auxNombre, " ");
						//memcpy(gToken_47.nombre, &kbdbuff[1], kbdbuff[0]);

						memset(kbdbuff, 0x00, sizeof(kbdbuff));
						DispLineMW( "CUENTA XPERTA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
						//DispLineMW( "Por favor ingrese", MW_LINE2, MW_CENTER );
						DispLineMW( "Nombre:", MW_LINE3, MW_LEFT );
						DispLineMW( "No de CC/CI:", MW_LINE7, MW_LEFT|MW_SPFONT );	//NOHORA 23Nov
						DispLineMW( &auxCed_kbdbuff[1], MW_LINE8, MW_SPFONT );	//NOHORA 23Nov
						if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+MW_LINE4+MW_BIGFONT+RIGHT_JUST, IMIN(1) + IMAX(15), kbdbuff)){
							APM_ResetComm();
							return -1;
						}
						strcat(auxNombre, &kbdbuff[1]);
						memcpy(gToken_47.nombre, auxNombre, strlen(auxNombre));

						memset(gToken_47.cedula_ruc,0xFF,8);	//NOHORA 23Nov desde aca
						if( auxCed_kbdbuff[0] % 2 != 0 ){
							auxCed_kbdbuff[ auxCed_kbdbuff[0] + 1 ] = 'F';
							auxCed_kbdbuff[0] += 1;
						}
						compress(gToken_47.cedula_ruc, &auxCed_kbdbuff[1], (auxCed_kbdbuff[0]/2));	//NOHORA 23Nov hasta aca

					}
				}
				else{
					memcpy(nombre, gToken_47.nombre, 30);
					if(opcion == 1){
						//memcpy(valor, gToken_14.valor, 12);
						split(cedulaAux,   gToken_47.cedula_ruc, 8);
						RTrim( cedulaAux, 'F' );
						DispLineMW( "CUENTA XPERTA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
						DispClrBelowMW(MW_LINE2);
						DispLineMW( "Nombre:", MW_LINE2, MW_LEFT|MW_SPFONT );
						DispLineMW( nombre, MW_LINE3, MW_SPFONT );
						DispLineMW( "Cedula:", MW_LINE4, MW_LEFT|MW_SPFONT );
						DispLineMW( cedulaAux, MW_LINE5, MW_SPFONT );
						DispLineMW( "DATOS CORRECTOS?", MW_LINE8, MW_CENTER|MW_SPFONT );	//NOHORA 20Nov
					}
					else{
						split(cuentaAux,   gToken_47.cuenta, 7);
						RTrim( cuentaAux, 'F' );
						DispLineMW( "CUENTA XPERTA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
						DispLineMW( "Reponer Kit", MW_LINE2, MW_CENTER );
						DispLineMW( "Cliente:", MW_LINE3, MW_LEFT|MW_SPFONT );
						DispLineMW( nombre, MW_LINE4, MW_SPFONT );
						DispLineMW( "Cuenta Xperta No:", MW_LINE5, MW_LEFT|MW_SPFONT );
						DispLineMW( cuentaAux, MW_LINE6, MW_SPFONT );
						DispLineMW( "DATOS CORRECTOS?", MW_LINE8, MW_CENTER|MW_SPFONT );	//NOHORA 20Nov
					}
					if( SiNo() != 2 )   // 2 = Press Enter
					{
						DispLineMW( "TRANSACCION CANCELDA", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );	//NOHORA 20Nov
						DispLineMW( "Favor verificar", MW_LINE4, MW_CENTER );
						DispLineMW( "sus datos al:", MW_LINE5, MW_CENTER );
						DispLineMW( "2999999", MW_LINE6, MW_CENTER );
						Delay10ms(200);
						APM_ResetComm();
						return -1;
					}
				}
				DispClrBelowMW(MW_LINE2);
				memset( kbdbuff, 0x00, sizeof(kbdbuff) );
				DispLineMW( "Ingrese No. de Kit", MW_LINE2, MW_CENTER );
				DispLineMW( "Ingrese No. Celular", MW_LINE5, MW_CENTER );
				DispLineMW( "(10 Digitos)", MW_LINE7, MW_CENTER );		//NOHORA 20Nov
				APM_SetCol(4); // Set Columna for TextBox
				if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE3, IMIN(1) + IMAX(10), kbdbuff) )
				{
					APM_ResetComm();
					return -1;
				}
				memset(gToken_48.numero_kit,0xFF,5);

				if( kbdbuff[0] % 2 != 0 ){
					kbdbuff[ kbdbuff[0] + 1 ] = 'F';
					kbdbuff[0] += 1;
				}
				compress(gToken_48.numero_kit, &kbdbuff[1], (kbdbuff[0]/2));

				memset( kbdbuff, 0x00, sizeof(kbdbuff) );
				APM_SetCol(4); // Set Columna for TextBox
				if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE6, IMIN(10) + IMAX(10), kbdbuff) )
				{
					APM_ResetComm();
					return -1;
				}
				memset(gToken_48.numero_cel,0xFF,6);

				if( kbdbuff[0] % 2 != 0 ){
					kbdbuff[ kbdbuff[0] + 1 ] = 'F';
					kbdbuff[0] += 1;
				}
				compress(gToken_48.numero_cel, &kbdbuff[1], (kbdbuff[0]/2));

				if( efectivacionCreaXperta(opcion) == -1 )
					return -1;

				if(opcion == 1)
					ConfirmaCreaXperta();
				else
					ConfReponerXperta();
			}
			else
			{
				APM_ResetComm();
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	//TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	APM_ResetComm();
	return TRUE;
}


DWORD efectivacionCreaXperta(BYTE opcion){
	if(opcion == 1)
		INPUT.b_trans = EFECTIVACION_CREA_XPERTA;
	else
		INPUT.b_trans = EFECT_REP_XPERTA;

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//UpdateHostStatus(REV_PENDING);
		//SaveDataFile( &gAppDat );//Daniel Jacome 14/07/12 Salvo la fecha y hora para el reverso
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*0, FALSE*/))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				if(opcion == 2)
					APM_ResetComm();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				//clearReversoPichincha();
				APM_ResetComm();
				showMessageError();
				/*printf("\f");
				graphicWindow("ERROR");
				LongBeep();
				DispPutCMW(K_PushCursor);
				os_disp_textc(COLOR_RED);
				DispLineMW( gToken_7.msg_largo, MW_LINE4, MW_SPFONT|MW_LEFT );
				DispPutCMW(K_PopCursor);
				WaitKey(3000, 0);*/
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return -1;
	}
	if(opcion == 2){//Ojo dejar esto como esta por que de lo contrario SYSTEM ERROR DATA...
		PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
		MsgBufSetup();
		PackMsgBufLen();
		return TRUE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}




DWORD ConfirmaCreaXperta( void )
{
	INPUT.b_trans = CONFIRMACION_CREA_XPERTA;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}



DWORD ConfReponerXperta( void )
{
	INPUT.b_trans = CONF_REP_XPERTA;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}


/*int recadudaciones (WORD tipo){

int rdb = 0;
	switch(tipo){

		case 1:
		rdb = recaudaDistribuidores();
			if(rdb == 2){
				if( !MultiCuentaMenu() )
    				return FALSE;

    			if(recaudoMultiCuentaTrans())
	    		{
	        // Confirmacion de pago de servicios
	    	    	if( gPagoTarjeta == TRUE )
	    	        	//ConfirmaPagoTarjetaTrans();  LFLFLF
		    	    else
		    	        //ConfirmaPagoServicioTrans();
		        }

			}
			if (rdb == TRUE)
			{
				//confEfectivoDist();	
			}
		break;
		case 2:
		INPUT.b_trans = CON_RECARGA;
		break;
		case 3:
		INPUT.b_trans = CON_STOCK;
		break;
	}

	return TRUE;	


}*/


/*int consultaRecaudoDistriTrans(void){

	int select;
	BYTE kbdbuff[35];
	BYTE titulo[30];

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(titulo,  0x00, sizeof(titulo));

	INPUT.b_trans = CON_REC_DIST;

	DispLineMW( "REC. DISTRIBUIDOR", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "CODIDO DE EMPRESA", MW_LINE3, MW_CENTER|MW_SMFONT );
	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(1) + IMAX(10), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}
	memset( INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa) );
	memcpy( INPUT.sb_cod_empresa, &kbdbuff[1], kbdbuff[0] );
	LSetStr( INPUT.sb_cod_empresa, 10, '0' );

	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	DispLineMW( "COD COMERCIO", MW_LINE7, MW_CENTER );
	//ECHO
	if( !APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SMFONT + MW_LINE8, IMIN(1) + IMAX(20), kbdbuff) )
	{
		APM_ResetComm();
		return -1;
	}
	memset( INPUT.sb_contrapar, 0x00, sizeof(INPUT.sb_contrapar) );
	memcpy( INPUT.sb_contrapar, &kbdbuff[1], kbdbuff[0] );
	RSetStr( INPUT.sb_contrapar, 34, ' ' );

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				select = MultiFactura();// Dentro de esta funcion se llena el token 6
				if(select == FALSE){
					LongBeep();
					DispLineMW( "RECAUDACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
					//DispClrBelowMW(MW_LINE2);
					DispLineMW( "TRANSACCION CANCELADA", MW_LINE3, MW_CENTER );
					APM_WaitKey(200, 0);
					APM_ResetComm();
					return -1;
				}
				else if(select == 2){
					DispLineMW( "RECAUDACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
					DispLineMW( "NO TIENE FACTURAS", MW_LINE3, MW_CENTER|MW_SMFONT );
					DispLineMW( "PENDIENTES", MW_LINE4, MW_CENTER|MW_SMFONT );
					APM_WaitKey(200, 0);
					APM_ResetComm();
					return -1;
				}
				else
				{
					select = MenuRecaudaciones();
					if (select == -1)
						return -1;
					if( select == 2 )
					{
						  INPUT.b_trans = PAGO_DIST_DEBITO_CUENTA;
						if( !procesoRecaudoDebitoCuenta() ){
							APM_ResetComm();
							return -1;
						}
					}
					else
					{
						INPUT.b_trans = PAGO_DIST_EFECTIVO;
						if( !procesoRecaudoEfectivo() ){
							APM_ResetComm();
							return -1;
						}
					}

					ClearResponse();

					if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
					{
						if( !reversalPichinchaOk() ){
							APM_ResetComm();
							return -1;
						}
						ReadRTC( &INPUT.s_dtg );
						IncTraceNoCNB();
						MoveInput2Tx();
						PackHostMsg();
						saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
						//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
						if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
						{
							//printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
							//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
							RSP_DATA.b_response = CheckHostRsp();
							//  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

							if( (RSP_DATA.w_rspcode == '9'*256+'7') )
							{
								gMultiCuenta=TRUE;
								APM_ResetComm();
								clearReversoPichincha();
								return 2;   // Multiples cuentas - Arma el menu por fuera de la funcion
							}
							else if (RSP_DATA.b_response == TRANS_ACP)
							{
								//SaveRecord();
								SaveTransactionPichincha();
								PackVoucher( FALSE );

								TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
								gMultiCuenta = FALSE;
								return TRUE;

							}
							else
							{
								//UpdateHostStatus(NO_PENDING);
								clearReversoPichincha();
								gMultiCuenta = FALSE;
								showMessageError();
								return -1;
							}
						}
						else{
							APM_ResetComm();
							showMessage(2, 3, "TIEMPO FUERA");
							return -1;
						}
					}
					else
					{
						APM_ResetComm();
						APM_DisconnectPPP();
						showMessage(5, 3, "ERROR DE CONEXION");
						return -1;
					}
				}
			}
			else
			{
				APM_ResetComm();
				showMessageError();
				return -1;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return -1;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
	}
	return -1;
}*/


BOOLEAN consultaRecaudoDistriTrans(void){
	BYTE kbdbuff[35];
	BYTE titulo[30];

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(titulo,  0x00, sizeof(titulo));

	INPUT.b_trans = CON_REC_DIST;

	DispLineMW( "REC. DISTRIBUIDOR", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "CODIDO DE EMPRESA", MW_LINE3, MW_CENTER|MW_SMFONT );
	APM_SetCol(4); // Set Columna for TextBox
	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(1) + IMAX(10), kbdbuff) )
	{
		APM_ResetComm();
		return FALSE;
	}
	memset( INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa) );
	memcpy( INPUT.sb_cod_empresa, &kbdbuff[1], kbdbuff[0] );
	LSetStr( INPUT.sb_cod_empresa, 10, '0' );

	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	DispLineMW( "COD COMERCIO", MW_LINE7, MW_CENTER );
	//ECHO
	if( !APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SMFONT + MW_LINE8, IMIN(1) + IMAX(20), kbdbuff) )
	{
		APM_ResetComm();
		return FALSE;
	}
	memset( INPUT.sb_contrapar, 0x00, sizeof(INPUT.sb_contrapar) );
	memcpy( INPUT.sb_contrapar, &kbdbuff[1], kbdbuff[0] );
	RSetStr( INPUT.sb_contrapar, 34, ' ' );

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				APM_ResetComm();
			}
			else
			{
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	return TRUE;
}




DWORD pagoRecaudoDistriTrans(BOOLEAN multicuenta){
	INPUT.b_trans = PAGO_DIST_EFECTIVO;

	PackComm(TRUE, FALSE);

	if( multicuenta ){
		APM_PreConnect();
		if( !MultiCuentaMenu() ){
			APM_ResetComm();
			return FALSE;
		}
	}

	ClearResponse();
	DispClrBelowMW(MW_LINE1);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;
			}
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				AcceptBeep();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else{
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



/*BOOLEAN recaudoDistriMulticuenta(void){
	// Arma Menu Multiples Cuentas
	if( !MultiCuentaMenu() ){
		return FALSE;
	}
	INPUT.b_trans = PAGO_DIST_DEBITO_CUENTA;
	gMultiCuenta = TRUE;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			//  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta = FALSE;
	return TRUE;
}*/





DWORD recaudoRecargasTrans(void){
	int select;
	BYTE nomEmpresa[20 + 1];

	PackComm(TRUE, FALSE);

	memset(nomEmpresa, 0x00, sizeof(nomEmpresa));
	select = menuRecargas(nomEmpresa);
	//printf("\fSelect 2: %d", select);APM_WaitKey(3000, 0);
	if(select == FALSE)
		return -1;

	if(select != 5){
		if( !setRecarga(nomEmpresa) )
			return -1;
	}

	if( !confirmarRecarga(nomEmpresa) ){
		showMessage(14, 2, "RECARGA");
		return -1;
	}

	select = MenuRecaudaciones();

	if(select == -1)
		return -1;

	if(select == 1){
		APM_PreConnect();
		if( !procesoRecaudoEfectivo("RECARGA") ){
			APM_ResetComm();
			return -1;
		}
	}
	else if(select == 2){
		APM_PreConnect();
		if( !procesoRecaudoDebitoCuenta("RECARGA") ){
			APM_ResetComm();
			return -1;
		}
	}
	INPUT.b_trans = RECARGA;
	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if( memcmp(INPUT.sb_typeAccount, "00", 2) != 0 )
			saveReversoPichincha();	// No se debe generar reverso cuando la recarga se paga en efectivo
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				//printf("\fSalgo por 2");APM_WaitKey(3000, 0);
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;
			}
			else if (RSP_DATA.b_response == TRANS_ACP)
			{
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return FALSE;
		}
	}
	else{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



BOOLEAN RecargasMulticuentaTrans(void){
	// Arma Menu Multiples Cuentas

	if( !MultiCuentaMenu() ){
		return FALSE;
	}
	INPUT.b_trans = RECARGA;
	//gMultiCuenta = TRUE;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return -1;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF/*, FALSE*/))==COMM_OK)
		{
			//printf("\fLen = %d", RX_BUF.wLen);WaitKey(3000, 0);
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			//  serialSend2(TX_BUF.sbContent, TX_BUF.wLen);

			if (RSP_DATA.b_response == TRANS_ACP)
			{
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else
			{
				//UpdateHostStatus(NO_PENDING);
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		gMultiCuenta = FALSE;
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta = FALSE;
	return TRUE;
}



BYTE consultaVentaDeSegurosTrans(void){
	BYTE kbdbuff[50];

	if(gRecaudoVar == TRUE)
		INPUT.b_trans = CONSULTA_VENTA_SEGURO_V;
	else
		INPUT.b_trans = CONSULTA_VENTA_SEGURO;

	PackComm(TRUE, FALSE);
	//APM_PreConnect();

	if(gRecaudoVar == FALSE){
		DispLineMW( "VENTA DE SEGUROS", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
		DispLineMW( "COD DE EMPRESA/ SERVICIO", MW_LINE3, MW_CENTER );
		APM_SetCol(4); // Set Columna for TextBox
		memset(kbdbuff, 0x00, sizeof(kbdbuff));
		if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE5, IMIN(1) + IMAX(10), kbdbuff) )
		{
			APM_ResetComm();
			return FALSE;
		}
		memset( INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa) );
		memcpy( INPUT.sb_cod_empresa, &kbdbuff[1], kbdbuff[0] );
		LSetStr( INPUT.sb_cod_empresa, 10, '0' );

		memset( kbdbuff, 0x00, sizeof(kbdbuff) );
		DispLineMW( "Num. CONTRAPARTIDA", MW_LINE7, MW_CENTER );
		//ECHO
		if( !APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SMFONT + MW_LINE9, IMIN(1) + IMAX(20), kbdbuff) )
		{
			APM_ResetComm();
			return FALSE;
		}

		memset( INPUT.sb_contrapar, 0x00, sizeof(INPUT.sb_contrapar) );
		memcpy( INPUT.sb_contrapar, &kbdbuff[1], kbdbuff[0] );
		RSetStr( INPUT.sb_contrapar, 34, ' ' );
	}

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				APM_ResetComm();
				AcceptBeep();
				if (gToken_39.flagVarFijo == 'N') // si es N es porque es variable
				{
					gToken_39.flagVarFijo = 'S';
					return 2;
				}
			}
			else{
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	APM_ResetComm();
	return TRUE;
}



DWORD pagoDeSegurosTrans(BOOLEAN multicuenta){
	INPUT.b_trans = PAGO_VENTA_SEGURO;
	PackComm(TRUE, FALSE);

	if( multicuenta ){
		//APM_PreConnect();
		if( !MultiCuentaMenu() ){
			//APM_ResetComm();
			return FALSE;
		}
	}

	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;
			}
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				AcceptBeep();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else{
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	return TRUE;
}



BYTE compraDeStockTrans(BOOLEAN multicuenta){
	INPUT.b_trans = COMPRA_STOCK;

	if( multicuenta ){
		//APM_PreConnect();
		if( !MultiCuentaMenu() ){
			//APM_ResetComm();
			return FALSE;
		}
	}

	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		saveReversoPichincha();
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			RSP_DATA.b_response = CheckHostRsp();
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;
			}
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				AcceptBeep();
				SaveTransactionPichincha();
				PackVoucher( FALSE );
			}
			else{
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta=FALSE;
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta = FALSE;
	return TRUE;
}




BOOLEAN consultaComisionTrans(BYTE opcion){
	if(opcion == 1)
		INPUT.b_trans = CONSULTA_COMISION;
	else
		INPUT.b_trans = CONSULTA_COMISION_2;

	PackComm(TRUE, FALSE);

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				APM_ResetComm();
				AcceptBeep();
			}
			else{
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	return TRUE;
}



BYTE consultaTrans(BYTE aOpcion, BOOLEAN aMulticuenta){

	if(aOpcion == 1)
		INPUT.b_trans = CONSULTA_SALDO;
	else if(aOpcion == 2)
		INPUT.b_trans = CONSULTA_ULT_MOV;
	else if(aOpcion == 3){
	    INPUT.b_trans = CONSULTA_SALDOCNB;
	    INPUT.dd_amount = 0;
	}
	else if(aOpcion == 4){
		INPUT.b_trans = CONSULTA_ULT_MOV_CNB;
		INPUT.dd_amount = 0;
	}

	PackComm(TRUE, FALSE);

	if( aMulticuenta ){
		//APM_PreConnect();
		if( !MultiCuentaMenu() ){
			//APM_ResetComm();
			return FALSE;
		}
	}

	ClearResponse();

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		if( !reversalPichinchaOk() ){
			APM_ResetComm();
			return FALSE;
		}
		ReadRTC( &INPUT.s_dtg );
		IncTraceNoCNB();
		MoveInput2Tx();
		PackHostMsg();
		if(INPUT.dd_amount > 0)
			saveReversoPichincha();	// Almacena el reverso y pone el flag de reverso en TRUE internamente
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		if ((RSP_DATA.w_rspcode=APM_SendRcvd(&TX_BUF, &RX_BUF))==COMM_OK)
		{
			//serialSend2(RX_BUF.sbContent, RX_BUF.wLen);
			RSP_DATA.b_response = CheckHostRsp();
			if( (RSP_DATA.w_rspcode == '9'*256+'7') )
			{
				gMultiCuenta=TRUE;
				APM_ResetComm();
				clearReversoPichincha();
				return 2;
			}
			if (RSP_DATA.b_response == TRANS_ACP)
			{
				AcceptBeep();
				SaveTransactionPichincha();
				if(aOpcion == 1 || aOpcion == 2)
					PackVoucher( FALSE );
				else if(aOpcion == 3){
					mostrarSaldoCnb();
				}
			}
			else{
				clearReversoPichincha();
				gMultiCuenta = FALSE;
				APM_ResetComm();
				showMessageError();
				return FALSE;
			}
		}
		else{
			APM_ResetComm();
			showMessage(2, 3, "TIEMPO FUERA");
			gMultiCuenta = FALSE;
			return FALSE;
		}
	}
	else
	{
		APM_ResetComm();
		APM_DisconnectPPP();
		showMessage(5, 3, "ERROR DE CONEXION");
		return FALSE;
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	gMultiCuenta=FALSE;
	return TRUE;
}





DWORD ConfirmaRecargaTrans( void )
{
	INPUT.b_trans = CONFIRMA_RECARGAS;

	PackComm(TRUE, FALSE);
	APM_PreConnect();

	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		//serialSend2(TX_BUF.sbContent, TX_BUF.wLen);
		APM_SendMsg(&TX_BUF);
		//APM_SendRcvd(&TX_BUF, &RX_BUF, TRUE);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}



DWORD ConfirmarTransaccion( BYTE aTrans )
{
	INPUT.b_trans = aTrans;

	PackComm(TRUE, FALSE);
	//APM_PreConnect();

	ClearResponse();
	DispClrBelowMW(MW_LINE2);

	if ((RSP_DATA.w_rspcode = APM_ConnectOK(FALSE)) == COMM_OK)
	{
		ReadRTC( &INPUT.s_dtg );
		MoveInput2Tx();
		PackHostMsg();
		APM_SendMsg(&TX_BUF);
	}
	TransEnd((BOOLEAN)(RSP_DATA.b_response == TRANS_ACP));
	procesarMensajePOS();
	return TRUE;
}




BOOLEAN procesoRecaudoEfectivo(BYTE *titulo){
	//INPUT.b_trans = trans;

	DispLineMW( "RECAUDACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_SMFONT|MW_CENTER );
	DispLineMW( "DEL CNB", MW_LINE4, MW_SMFONT|MW_CENTER );
	strcpy(INPUT.sb_typeAccount, "00");

	if( GetCard( FALSE, FALSE ) == FALSE ){
		LongBeep();
		DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
		DispLineMW( "TRANSACCION CANCELADA", MW_LINE3, MW_CENTER );
		APM_WaitKey(200, 0);
		//APM_ResetComm();
		return FALSE;
	}
	INPUT.dd_amount = atoi(gAmount);

	DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "Monto:", MW_LINE2, MW_LEFT|MW_SMFONT );
	DispAmnt(INPUT.dd_amount, MW_LINE2, MW_SMFONT);

	if( !getPinblock() )
		return FALSE;

	return TRUE;
}


BOOLEAN procesoRecaudoDebitoCuenta(BYTE *titulo){
	int sel = 0;
	BYTE kbdbuff[10];
	BYTE tmpPan[20 + 1];

	memset(kbdbuff, 0x00, sizeof(kbdbuff));
	memset(tmpPan,  0x00, sizeof(tmpPan));

	sel = MenuRecaudaCuenta();
	switch (sel)
	{
		case 1:
			strcpy(INPUT.sb_typeAccount, "01");
			break;
		case 2:
			strcpy(INPUT.sb_typeAccount, "02");
			break;
		case 3:
			strcpy(INPUT.sb_typeAccount, "03");
			break;
	}

	DispLineMW( "DESLICE TARJETA", MW_LINE3, MW_SMFONT|MW_CENTER|MW_CLRDISP );
	DispLineMW( "DEL CLIENTE", MW_LINE4, MW_SMFONT|MW_CENTER );

	if( GetCard( FALSE, FALSE ) == FALSE ){
		LongBeep();
		DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
		DispClrBelowMW(MW_LINE2);
		DispLineMW( "TRANSACCION CANCELADA", MW_LINE3, MW_CENTER );
		APM_WaitKey(200, 0);
		//APM_ResetComm();
		return FALSE;
	}
	do
	{
		DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
		DispClrBelowMW(MW_LINE2);
		DispLineMW( "INGRESE LOS ULTIMOS", MW_LINE3, MW_CENTER|MW_SPFONT );
		DispLineMW( "CUATRO DIGITOS", MW_LINE4, MW_CENTER|MW_SPFONT );
		DispLineMW( "DE LA TARJETA", MW_LINE5, MW_CENTER|MW_SPFONT );

		memset(kbdbuff, 0x00, sizeof(kbdbuff));
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff)){
			//APM_ResetComm();
			return FALSE;
		}
		// Compara los ultimo 4 digitos del Pan
		memset( tmpPan, 0x00, sizeof(tmpPan) );
		split( tmpPan, INPUT.sb_pan, 10 );
		RTrim( tmpPan, 'F');
	} while (memcmp(&kbdbuff[1], &tmpPan[strlen(tmpPan) - 4], 4) != 0);

	INPUT.dd_amount = atoi(gAmount);
	DispLineMW( titulo, MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "Monto:", MW_LINE2, MW_LEFT|MW_SMFONT );
	DispAmnt(INPUT.dd_amount, MW_LINE2, MW_SMFONT);

	if( !getPinblock() )
		return FALSE;

	return TRUE;
}





BOOLEAN validarPagoDeSeguro(void){
	BYTE empresa[30];
	BYTE nmPersona[30];
	BYTE monto[13];
	DDWORD dd_monto;

	memset(empresa, 0x00, sizeof(empresa));
	memset(monto, 0x00, sizeof(monto));
	memset(nmPersona, 0x00, sizeof(nmPersona));

	memcpy( empresa, gToken_6.nomEmpresa, 20 );
	memcpy( nmPersona, gToken_6.nomPersona, 20 );
	split(monto, gToken_6.valor_documento, 6);
	dd_monto = atoi(monto);
	memset(monto, 0x00, sizeof(monto));

	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Empresa:", MW_LINE2, MW_LEFT|MW_SPFONT );
	DispLineMW( empresa, MW_LINE3, MW_SPFONT );
	DispLineMW( nmPersona, MW_LINE4, MW_SPFONT|MW_CENTER );//LFLF

	DispLineMW( "Monto :", MW_LINE5, MW_LEFT|MW_SMFONT );
	DispAmnt(dd_monto, MW_LINE5, MW_SMFONT);

	DispLineMW( "Servicio:", MW_LINE6, MW_LEFT|MW_SPFONT );
	split(monto, gToken_6.costo_servicio, 6);
	dd_monto = atoi(monto);
	memset(monto, 0x00, sizeof(monto));
	DispAmnt(dd_monto, MW_LINE6, MW_SMFONT);

	DispLineMW( "TOTAL:", MW_LINE7, MW_LEFT|MW_SMFONT );
	split(monto, gToken_6.valor_adeudado, 6);
	dd_monto = atoi(monto);
	DispAmnt(dd_monto, MW_LINE7, MW_SMFONT);
	DispLineMW( "Pagar?", MW_LINE8, MW_CENTER|MW_SMFONT );

	if( SiNo() != 2 ){   // 2 = Press Enter
		APM_ResetComm();
		return FALSE;
	}
	//INPUT.dd_amount = dd_monto;
	memset(gAmount, 0x00, sizeof(gAmount));
	sprintf(gAmount, "%012d", dd_monto);
	return TRUE;
}


///////////////////////////////////////////////////////////
//		FUNCION PARA PROBAR EL TECLADO EXTERNO
//////////////////////////////////////////////////////////
BOOLEAN extKeyBoardTest(void){
	int idPort = 0;
	BYTE buffer[50];

	memset(buffer, 0x00, sizeof(buffer));
	if( openExtKeyboard(&idPort) ){
		memset(&miCapturaExt, 0x00, sizeof(miCapturaExt));
		strcpy(miCapturaExt.titulo, "FUNCION DE PRUEBA");
		strcpy(miCapturaExt.label, "Nombre:");
		miCapturaExt.colIni = 2;
		miCapturaExt.colFin = 24;
		miCapturaExt.minChar = 1;
		miCapturaExt.maxChar = 30;
		if( !getSExtKeyboard(idPort, buffer, buffer) ){	//NOHORA 23Nov (puse el 3 param pa poder probar)
			closeExtKeyBoard(idPort);
			return FALSE;
		}
		closeExtKeyBoard(idPort);
		printf("\fDato: \n %s", buffer);APM_WaitKey(3000, 0);
		return TRUE;
	}
	return FALSE;
}
