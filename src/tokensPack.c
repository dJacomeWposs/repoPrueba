/*
 * tokensPack.c
 *
 *  Created on: Jul 6, 2012
 *      Author: danijacm
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "sysutil.h"
#include "constant.h"
#include "corevar.h"
#include "chkoptn.h"
#include "keytrans.h"
#include "tokensPack.h"
#include "tranutil.h"
#include "key2dll.h"
#include "rs232.h"
#include "coremain.h"

int setInfo1 (void)
{
	struct MW_APPL_INFO applicationInfo;
	BYTE checksum[4 + 1];

	memset(checksum, 0x00, sizeof(checksum));
	//checksum[5] = 0x00;
	AppInfoGetMW(MY_APPL_ID, &applicationInfo);

	sprintf(checksum, "%04X", applicationInfo.w_checksum);	// Extraigo el checksum de la aplicacion
	pack_byte(0x01); //id_campo
   	pack_word(0x0005);//tamaño del campo
  // pack_word(0x0004);//tamaño del campo
	//pack_str("01.06");
	pack_str(checksum);
	pack_space(1);
	return 8;
}

int setInfo2 (void)
{
	pack_byte(0x02); //id_campo
	pack_word(0x0013);//tamaño del campo
	pack_str( "PRT" );
	pack_str( "00.01" );
	pack_str( "02.00" );
	return 16;
}


int setInfo3(BYTE pagTarjeta){
	pack_byte(0x03); //id token
	pack_word(0x0045);//tamaño del token

	if(pagTarjeta == TRUE){
		pack_mem(INPUT.sb_tipotarjeta, 2);// tipo de tarjeta
		//printf("\f%s", INPUT.sb_tipotarjeta);WaitKey(3000, 0);
		pack_mem(INPUT.sb_numtarjeta, 42);// numero de tarjeta
	}
	else{
		pack_mem(INPUT.sb_cod_empresa, 10);// Codigo de la empresa
		pack_mem(INPUT.sb_contrapar, 34);  // Contrapartida
	}
	if( memcmp(INPUT.sb_typeAccount, "00", 2 ) == 0)	// Pago en efectivo
		pack_byte(0x01);	// Modalidad
	else
		pack_byte(0x04);	// Modalidad
	/*pack_byte(0x00);	// Tipo captura
	pack_zero(6);		// Valor deducciones
	pack_zero(6);		// Valor incrementado*/
	return 48;
}

int setInfo5 (void)
{
	pack_byte(0x05);//id_campo
	pack_word(0x0085);//tamaño del campo
	//pack_mem(&gToken_5_pack.nomEmpresa[0], 85);
	pack_mem(gToken_5_pack.nomEmpresa, 20);
	pack_mem(gToken_5_pack.nomPersona, 20);
	pack_mem(gToken_5_pack.costServ, 2);
	pack_byte(gToken_5_pack.cantFacturas);
	pack_mem(gToken_5_pack.facturaEmp[0].fecha, 10);
	pack_mem(gToken_5_pack.facturaEmp[0].factura, 20);
	pack_mem(gToken_5_pack.facturaEmp[0].valor, 12);
	return 88;
}


int setInfo6(void){
	pack_byte(0x06);// id_campo
	pack_word(0x0086);// tamaño del campo
	pack_mem(&gToken_6.nomEmpresa[0], 86);// Empaqueto todo el token 6
	return 89;

}


int setInfo9 (void)
{
	BYTE tipCuenta = 0;
//	memset(tipCuenta,0x00,sizeof(tipCuenta));
	pack_byte(0x09);//id_campo
	pack_word(0x0001);//tamaño del campo
	//memcpy(&tipCuenta,& TX_DATA.sb_typeAccount[0], 1);
	//compress(tipCuenta, TX_DATA.sb_typeAccount,1);//pasar  ACSI a BCD
	if(INPUT.b_trans == CONSULTA_DEPOSITO || INPUT.b_trans == RETIRO){
		tipCuenta=atoi(TX_DATA.sb_typeAccount);
		gTypeAcount = tipCuenta;
	}
	else if( INPUT.b_trans == DEPOSITO )
		tipCuenta = gTypeAcount;
	else{
		tipCuenta=atoi(TX_DATA.sb_typeAccount);
	}
	//tipCuenta = 0x01;
	pack_byte(tipCuenta);//tipo de cuenta
	return 4;
}


int setInfo11 (void)
{
	BYTE cuentaAsc[14 + 1];
	BYTE cuentaBcd[8];
	BYTE lenCuenta;

	memset(cuentaAsc, 'F', sizeof(cuentaAsc));
	pack_byte(0x11); //id_campo
	pack_word(0x0049); //tamaño campo
	pack_word(0x0010); //banco
	pack_zero(3); //Agencia
	lenCuenta = strlen(INPUT.sb_num_cuenta);
	if(lenCuenta % 2 > 0)
	{
		TX_DATA.sb_num_cuenta[ lenCuenta ] = 'F';
		lenCuenta++;
	}
	memcpy(cuentaAsc, TX_DATA.sb_num_cuenta, lenCuenta);
	compress(cuentaBcd, cuentaAsc, 7); //pasar  ACSI a BCD
	pack_mem(cuentaBcd,7);
	if(TX_DATA.b_trans == DEPOSITO){
		pack_space(2);
		pack_mem(gToken_11.nombre_credito, 32);
	}
	else
		pack_space(34);   // Dig Credito 02 + Nombre Credito 32
	return 49;
}



int setInfo12 (void)
{
	BYTE cedula1[16 + 1];
	BYTE cedula2[8];
   
	int lenCed=0;
	memset(cedula1, 'F', sizeof(cedula1));
	memset(cedula2, 0x00, sizeof(cedula2));

	pack_byte(0x12); 	    //id_campo
	pack_word(0x0008);		//tamaño del campo
	lenCed = strlen(INPUT.sb_cedula_ruc);
	if(lenCed % 2 > 0)
	{
		TX_DATA.sb_cedula_ruc[ lenCed ] = 'F';
		lenCed++;
	}
	memcpy(cedula1, TX_DATA.sb_cedula_ruc, lenCed);
	compress(cedula2, cedula1, 8); //pasar  ACSI a BCD
   
	pack_mem(cedula2,8);
	return 11;
}



int setInfo13 (void)
{
	pack_byte(0x13); 	    //id_campo
	pack_word(0x0001);		//tamaño del campo
	pack_byte(0x00);
	return 4;
}


int setInfo14 (void)
{
	pack_byte(0x14); 	    //id_campo
	pack_word(0x0139);		//tamaño del campo
	pack_mem(&gToken_14.nombre[0], 139);// Empaqueto todo el token 14
	return 142;
}



int setInfo43 (void)
{
	BYTE track2[41];
	BYTE trackEnc[81];
	memset(track2,0x00,sizeof(track2));
	memset(trackEnc,0x00,sizeof(trackEnc));

	pack_byte(0x43); //id_campo
	pack_word(0x0080); //tamaño campo

	strcpy( track2, INPUT.s_trk2buf.sb_content );
	RSetStr( track2, 40, ' ' );
	KDLL_Key3Encrypt( /*EKEY_IDX*/CARDKEY_IDX, track2, 40 );
	split( trackEnc, track2, 40 );
	//serialSend2(trackEnc, 80);
	pack_mem( trackEnc, 80 );
	return 83;
}



int setInfo47 (void)
{
	BYTE cedula1[16 + 1];
	BYTE cedula2[8];
	int lenCed=0;
	memset(cedula1, 'F', sizeof(cedula1));
	memset(cedula2, 0x00, sizeof(cedula2));
	//serialSend2(cedula, 8);

	//WaitKey(3000,0);
	pack_byte(0x47); 			//id_campo
	pack_word(0x0045);		//tamaño del campo
	if(TX_DATA.b_trans == CONSULTA_DEPOSITO){
		lenCed = strlen(INPUT.sb_cedula_ruc);
		if(lenCed % 2 > 0)
		{
			TX_DATA.sb_cedula_ruc[ lenCed ] = 'F';
			lenCed++;
		}
		memcpy(cedula1, TX_DATA.sb_cedula_ruc, lenCed);
		compress(cedula2, cedula1, 8); //pasar  ACSI a BCD
		pack_mem(cedula2,8);
		pack_space( 30 ); // Nombre depositante
		memset(gToken_47.cuenta, 0xFF, 7);
		pack_mem(gToken_47.cuenta, 7);
	}
	else if(TX_DATA.b_trans == DEPOSITO){
		pack_mem(gToken_47.cedula_ruc, 8);
		pack_space( 30 ); // Nombre depositante
		memset(gToken_47.cuenta, 0xFF, 7);
		pack_mem(gToken_47.cuenta, 7);
	}
	else if(TX_DATA.b_trans == EFECTIVACION_CREA_XPERTA){
		pack_mem(gToken_47.cedula_ruc, 8);
		pack_mem(gToken_47.nombre, 30);
		memset(gToken_47.cuenta, 0xFF, 7);
		pack_mem(gToken_47.cuenta, 7);
	}
	else if(TX_DATA.b_trans == EFECT_REP_XPERTA){
		pack_mem(gToken_47.cedula_ruc, 8);
		pack_mem(gToken_47.nombre, 30);
		pack_mem(gToken_47.cuenta, 7);
	}
	return 48;
}


int setInfo48 (void)
{
	
	//serialSend2(cedula, 8);

	//WaitKey(3000,0);
	pack_byte(0x48); 			//id_campo
	pack_word(0x0011);		//tamaño del campo
	pack_mem(gToken_48.numero_kit,5);
	pack_mem(gToken_48.numero_cel,6);
	
	return 14;
}


int setInfo90(void){	// Datos del Reverso
	if ( !getReverseFile() ){
		showMessage(8, 3, "ERROR");
		return 0;
	}
	pack_byte(0x90); 		//id_campo
	pack_word(0x0010);		//tamaño del campo
	pack_word(gRevTable.MTI);
	pack_mem(gRevTable.STAN, 3);
	pack_mem(gRevTable.dateTrans, 2);
	pack_mem(gRevTable.timeTrans, 3);
	return 13;
}


int setInfo91(void){
	pack_byte(0x91); 		//id_campo
	pack_word(0x0034);		//tamaño del campo
	pack_mem(gToken_91.fechaHoraInicial, 14);
	pack_mem(gToken_91.fechaHoraFinal, 14);
	if(INPUT.b_trans == EMISION_CAPA_LOTES)
	pack_null(6);
	if(INPUT.b_trans == RESUME_TX)
		pack_mem(gToken_91.codigoProducto, 6);
	return 37;
}



int setInfo98 (void)
{
	BYTE numCuentas=0;
	pack_byte(0x98); 			//id_campo
	pack_word(0x0019);		//tamaño del campo
	compress(&numCuentas, gInfo_98_pack.cantCuentas, 1);
	pack_byte(numCuentas);
	pack_mem(gInfo_98_pack.cuentas, 18);
	return 22;
}


int setComprobanteTrans(void){
	BYTE comprobante[12];

	memset(comprobante, 0x20, sizeof(comprobante));
	memcpy(comprobante, gRrnTemporal, strlen(gRrnTemporal));
	pack_mem(comprobante, 12);
	return 12;
}
