/*
 * tokensUnpack.c
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
#include "tranutil.h"
#include "hostmsg.h"
#include "keytrans.h"
#include "key2dll.h"
#include "tokensPack.h"
#include "rs232.h"


BOOLEAN getToken1(void)
{
	DWORD len1=0;
	BYTE id1=0;
	id1=get_byte();
	//printf("\fToken = %02X", id1);APM_WaitKey(3000, 0);
	len1=bcd2bin(get_word());
	if (id1 != 0x01)
	{
		MsgError( "ERROR DEL TOKEN 1" );
		return FALSE;
	}
    get_mem( gToken_1.version, 5 );
    return TRUE;

}


BOOLEAN getToken3(BYTE pagTarjeta)
{
	BYTE id3;
	DWORD len3=0;

	id3=get_byte();
	len3=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	if(len3 > 58){
		MsgError( "LONG. TOKEN 3 ERRADA" );
		return FALSE;
	}
	if( id3 != 0x03)
	{
		MsgError( "ERROR DEL TOKEN 3" );
		return FALSE;
	}
	if(pagTarjeta)
		memcpy( &gToken_3ptc.tipoTarjeta[0], get_pptr(), len3  );
	else
		memcpy( &gToken_3psp.codEmpresa[0], get_pptr(), len3  );
	inc_pptr(len3);
	return TRUE;
}


BOOLEAN getToken5(void)
{
	BYTE id5;
	DWORD len5=0;
	//BYTE linea[30];
	int i;
	int cantFacturas;
	int ind = 0;

	id5=get_byte();
	len5=bcd2bin(get_word());

	if( id5 != 0x05)
	{
		MsgError( "ERROR DEL TOKEN 5" );
		return FALSE;
	}

	/*memcpy( &gToken_5.nomEmpresa[0], get_pptr(), len5  );
	inc_pptr(len5);*/
	memcpy( &gToken_5.nomEmpresa[0], get_pptr(), 43 );
	inc_pptr(43);
	/*memcpy( &gToken_5.facturaEmp[0], get_pptr(), len5 - 43 );
	inc_pptr(len5 - 43);*/

	cantFacturas = bcd2bin(gToken_5.cantFacturas);
	if(cantFacturas > 20){
		MsgError( "CANT. DE FACTURAS EXCEDIDO" );
		return FALSE;
	}

	ptrFacturas = (BYTE*)MallocMW(840);
	memcpy( ptrFacturas, get_pptr(), len5 - 43 );
	inc_pptr(len5 - 43);

	for( i=0; i<cantFacturas; i++ )
	{
		memcpy( gToken_5.facturaEmp[i].fecha, &ptrFacturas[ind], 10 );
		ind+=10;
		memcpy( gToken_5.facturaEmp[i].factura, &ptrFacturas[ind], 20 );
		ind+=20;
		memcpy( gToken_5.facturaEmp[i].valor, &ptrFacturas[ind], 12 );
		ind+=12;
	}
	FreeMW(ptrFacturas);
	return TRUE;
}





BOOLEAN getToken6(void)
{
	BYTE id6;
	DWORD len6=0;

	id6=get_byte();
	len6=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	if(len6 > 86){
		MsgError( "LONG. TOKEN 6 ERRADA" );
		return FALSE;
	}
	if( id6 != 0x06)
	{
		MsgError( "ERROR DEL TOKEN 6" );
		return FALSE;
	}
	memcpy( &gToken_6.nomEmpresa[0], get_pptr(), len6  );
	inc_pptr(len6);
	return TRUE;
}




BOOLEAN getToken7(int rspCode)
{
	BYTE id7;
	DWORD len7=0;

	id7=get_byte();
	len7=bcd2bin(get_word());
	//printf("\fLen7 = %d", len7);WaitKey(3000, 0);

	 if (id7 != 0x07)
	 {
		 MsgError( "ERROR DEL TOKEN 7" );
		 return FALSE;
	 }
	 memset(&gToken_7.msg_largo, 0x00, 60);
	 memcpy( &gToken_7.msg_largo[0], get_pptr(), len7 );
	 inc_pptr(len7);

	 if( rspCode != 0 ){
		 if( rspCode != 97 && rspCode != 95 )
			 return FALSE;
	 }
	 return TRUE;
}


BOOLEAN getToken8(void)
{
	BYTE id8;
	DWORD len8=0;

	id8 = get_byte();
	len8=bcd2bin(get_word());
	//printf("\fId8: %02X", id8);
	//printf("Len8: %d", len8);WaitKey(3000, 0);
	if (id8 != 0x08)
	{
		MsgError( "ERROR DEL TOKEN 8" );
		return FALSE;
	}
	gToken_8.flag_more=get_byte();
	//if( !concatene )
	memcpy( &gToken_8.cupon[lenCupon], get_pptr(), len8-1 );
	/*else
		memcpy( &gToken_8.cupon[lenCupon], get_pptr(), len8-1 );*/
    inc_pptr(len8-1);
    lenCupon += (len8-1);
    //printf("\fLenCupon = %d", lenCupon);WaitKey(3000, 0);
    return TRUE;
}


BOOLEAN getToken11(void)
{
	BYTE id11;
	DWORD len11=0;

	id11=get_byte();
	len11=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);

	if( id11 != 0x11)
	{
		MsgError( "ERROR DEL TOKEN 11" );
		return FALSE;
	}
	memcpy( &gToken_11.banco_credito[0], get_pptr(), len11  );
	inc_pptr(len11 );
	return TRUE;
}



BOOLEAN getToken14(void)
{
	BYTE id14;
	DWORD len14=0;

	id14=get_byte();
	len14=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	if(len14 > 139){
		MsgError( "LONG. TOKEN 14 ERRADA" );
		return FALSE;
	}
	if( id14 != 0x14)
	{
		MsgError( "ERROR DEL TOKEN 14" );
		return FALSE;
	}
	memcpy( &gToken_14.nombre[0], get_pptr(), len14  );
	inc_pptr(len14);
	return TRUE;
}

BOOLEAN getToken15(void)
{
	BYTE id15;
	DWORD len15=0;

	id15=get_byte();
	len15=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	
	if( id15 != 0x15)
	{
		MsgError( "ERROR DEL TOKEN 15" );
		return FALSE;
	}
	memcpy( &gToken_15.nombre[0], get_pptr(), len15  );
	inc_pptr(len15);
	return TRUE;
}

BOOLEAN getToken39(void)
{
	BYTE id39;
	DWORD len39=0;

	id39=get_byte();
	len39=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	if(len39 > 3){
		MsgError( "LONG. TOKEN 39 ERRADA" );
		return FALSE;
	}
	if( id39 != 0x39)
	{
		MsgError( "ERROR DEL TOKEN 39" );
		return FALSE;
	}
	memcpy( &gToken_39, get_pptr(), len39  );
	inc_pptr(len39);
	return TRUE;
}



BOOLEAN getToken47(void)
{
	BYTE id47;
	DWORD len47=0;

	id47=get_byte();
	len47=bcd2bin(get_word());
	//printf("\fLen47 = %d", len47);WaitKey(3000, 0);

	if(id47 !=0x47)
	{
		MsgError( "ERROR DEL TOKEN 47" );
		return FALSE;
	}
	memcpy( &gToken_47.cedula_ruc[0], get_pptr(), len47  );
	inc_pptr(len47);
	return TRUE;
}

BOOLEAN getToken48(void)
{
	BYTE id48;
	DWORD len48=0;

	id48=get_byte();
	len48=bcd2bin(get_word());
	//printf("\fLen47 = %d", len47);WaitKey(3000, 0);

	if(id48 !=0x48)
	{
		MsgError( "ERROR DEL TOKEN 47" );
		return FALSE;
	}
	memcpy( &gToken_48.numero_kit[0], get_pptr(), 5  );
	inc_pptr(5);
	memcpy( &gToken_48.nombre[0], get_pptr(), 5  );
	inc_pptr(6);
	return TRUE;
}

BOOLEAN getToken80(void)
{
	DWORD len80=0;
	BYTE id80=0;

	id80=get_byte();
	len80=bcd2bin(get_word());
	//printf("\fLen80 = %d", len80);WaitKey(3000, 0);

	if (id80 != 0x80)
	{
		MsgError( "ERROR DEL TOKEN 80" );
		return FALSE;
	}
	memcpy( &gToken_80.timeMaxConnect[0], get_pptr(), len80 );
	inc_pptr(len80);
	return TRUE;
}



BOOLEAN getToken81(void)
{
	DWORD len81=0,tam=0;
	BYTE id81;

	id81=get_byte();
	len81=bcd2bin(get_word());

	//printf("\fID: %02X", id81);WaitKey(3000, 0);
	if( id81 != 0x81)
	{
		MsgError( "ERROR DEL TOKEN 81" );
		return FALSE;
	}
	get_mem(&gToken_81.fecha[0], len81);
	tam=ByteBcd2Int(gToken_81.lenMsg);
	memcpy(gMsgInit, gToken_81.msgInit, tam);

	return TRUE;
}



BOOLEAN getToken96(void){
	BYTE id96;
	DWORD len96=0;

	id96=get_byte();
	len96=bcd2bin(get_word());
	//printf("\fLen11 = %d", len11);WaitKey(3000, 0);
	if(len96 > 16){
		MsgError( "LONG. TOKEN 96 ERRADA" );
		return FALSE;
	}
	if( id96 != 0x96)
	{
		MsgError( "ERROR DEL TOKEN 96" );
		return FALSE;
	}
	memcpy( &gToken_96, get_pptr(), len96  );
	inc_pptr(len96);
	return TRUE;
}



BOOLEAN getToken97(void)
{
	DWORD len97=0;
	BYTE id97=0;

	id97=get_byte();
	len97=bcd2bin(get_word());

	if (id97 != 0x97)
	{
		MsgError( "ERROR DEL TOKEN 97" );
		return FALSE;
	}
	get_mem(&gtoken_97.worKeyPin[0],len97);
	//get_mem(gtoken_97.worKeyPin, 16);
	//get_mem(gtoken_97.worTrack2, 16);

	return TRUE;

}



BOOLEAN getToken98(void)
{
	BYTE numCuentas[3],entero=0, id98;
	DWORD i=0,len98;

	memset(numCuentas,0x00,sizeof(numCuentas));


	id98=get_byte();
	len98=bcd2bin(get_word());
	//get_mem(len98,2);

	if (id98 !=0x98)
	{
		MsgError( "ERROR DEL TOKEN 98" );
		return FALSE;
	}
	entero=get_byte();
	split(numCuentas,&entero,1);
	memset(gInfo_98[0].cantCuentas,0x00,sizeof(gInfo_98[0].cantCuentas));
	memcpy( gInfo_98[0].cantCuentas, numCuentas, 2 );

	//printf("NumCuentas = %d", atoi(numCuentas));WaitKey(3000, 0);

	if( atoi(numCuentas) > 5){
		MsgError( "MAX. CUENTAS SUPERADO" );
		return FALSE;
	}

	for( i = 0; i < atoi(numCuentas); i++ )
	{
		//printf("\fi = %d", i);WaitKey(3000, 0);
		memcpy( gInfo_98[i].cuentas, get_pptr(), 18 );  // Longitud de las cuentas
		inc_pptr(18);
	}
	return TRUE;
}

