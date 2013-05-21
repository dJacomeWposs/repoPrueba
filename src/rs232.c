//******************************************************************************
//*                                                                            *
//*                 T & S Ltda.                                                *
//*                 (c) Copyright 2006                                         *
//*                                                                            *
//*  PRODUCT   :  BASE project                                                 *
//*                                                                            *
//*  FILENAME  :  rs232.c                                                      *
//*                                                                            *
//*      DATE        Modified            COMMENT                               *
//*  -----------  ----------------       ---------                             *
//*  26/OCT/2011  Jhony Mauricio Toro E. release                               *
//*                                                                            *
//******************************************************************************
//==============================================================================
// Included files
//==============================================================================
#include "rs232.h"
#include "midware.h"
#include "sysutil.h"
//#include "gui.h"
#include "corevar.h"

/*******************************************************************************
 *  Function        : AuxOpen
 *  Description     : Abre el puerto auxiliar
 *  Input           : gPort		=	numero del puerto
 gCommMode	=	modo de transmision
 gSpeed	=	velocidad
 *  Return          : id del puerto que se abrio o -1 si fallo
 ******************************************************************************/
int AuxOpen(int gPort, int gCommMode, int gSpeed)
{
	struct MW_AUX_CFG aux_cfg;
	char dev_name[24];
	int gAuxHandle;

	if ((gPort < 0) || (gPort >= 3))
		return FALSE;

	aux_cfg.b_len = 0x06;
	aux_cfg.b_mode = gCommMode; //5
	aux_cfg.b_speed = (BYTE) gSpeed; //7
	aux_cfg.b_rx_gap = 0x32;
	aux_cfg.b_rsp_gap = 0x32;
	aux_cfg.b_tx_gap = 0x32;
	aux_cfg.b_retry = 0x03;
	SprintfMW(dev_name, "/dev/aux%d", gPort);
	// printf("\f gmode: %d",gPort);Delay10ms(600);
	if (((gAuxHandle = OpenMW(dev_name, MW_RDWR)) < 0) || (IOCtlMW(gAuxHandle, IO_AUX_CFG, &aux_cfg) < 0))
	{
		return -1;
	}
	return gAuxHandle;
}

/*******************************************************************************
 ******************************************************************************/
BOOLEAN AuxClose(int gAuxHandle)
{

	if (CloseMW(gAuxHandle) < 0)
		return FALSE;
	return TRUE;
}

/*******************************************************************************
 *  Function        : AuxRead
 *  Description     : Lee datos del puerto auxiliar
 *  Input           : 	aBuf		=	apuntador donde se almacena el dato recibido
 *  					aLen		=	Longitud del dato a recibir
 *  					gAuxHandle	=	id del puerto auxiliar
 *  Return          : TRUE/FALSE
 ******************************************************************************/
DWORD AuxRead(BYTE *aBuf, DWORD aLen, int gAuxHandle)
{
	int len;

	len = ReadMW(gAuxHandle, aBuf, aLen);
	if (len < 0)
		len = 0;
	return len;
}

/*******************************************************************************
 *  Function        : AuxWrite
 *  Description     : Envia datos por el puerto auxiliar
 *  Input           : 	aBuf		=	apuntador donde se almacena el dato recibido
 *  					aLen		=	Longitud del dato a recibir
 *  					gAuxHandle	=	id del puerto auxiliar
 *  Return          : TRUE/FALSE
 ******************************************************************************/
BOOLEAN AuxWrite(BYTE *aBuf, DWORD aLen, int gAuxHandle)
{

	if (WriteMW(gAuxHandle, aBuf, aLen) != aLen)
		return FALSE;
	return TRUE;
}

/*******************************************************************************
 *  Function        : AuxRxRdy
 *  Description     : Verifica si el puerto esta listo para recibir
 *  Input           :
 *  					gAuxHandle	=	id del puerto auxiliar
 *  Return          : TRUE/FALSE
 ******************************************************************************/
BOOLEAN AuxRxRdy(int gAuxHandle)
{

	if (IOCtlMW(gAuxHandle, IO_AUX_RX_RDY, NULL) <= 0)
		return FALSE;
	return TRUE;
}

/*******************************************************************************
 *  Function        : AuxTxRdy
 *  Description     : Verifica si el puerto esta listo para enviar
 *  Input           :
 *  					gAuxHandle	=	id del puerto auxiliar
 *  Return          : TRUE/FALSE
 ******************************************************************************/
BOOLEAN AuxTxRdy(int gAuxHandle)
{

	if (IOCtlMW(gAuxHandle, IO_AUX_TX_RDY, NULL) <= 0)
		return FALSE;
	return TRUE;
}

/******************************************************************************
 ******************************************************************************/
int serialSendAndReceive(BYTE *dataTx, int len, int timeOut)
{

	int id_port;
	int lenRx = 0;
	int idTimer = -1;
	BYTE *buffTx;
	BYTE *buffRx;

	buffTx = MallocMW(1024);
	buffRx = MallocMW(1024);
	if ((buffTx == NULL) || (buffRx == NULL))
	{
		DispLineMW("ERROR", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		DispLineMW("No fue posible reservar", MW_LINE4, MW_CENTER|MW_SMFONT);
		DispLineMW("Memoria", MW_LINE5, MW_CENTER|MW_SMFONT);
		WaitKey(9000, 0);

		return 0;
	}
	memset(buffTx, 0x00, 1024);
	memset(buffRx, 0x00, 1024);
	memcpy(buffTx, dataTx, len);
	id_port = AuxOpen(1, 5, 7);
	if (!id_port)
	{
		DispLineMW("ERROR RS232", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		DispLineMW("No fue posible abrir", MW_LINE4, MW_CENTER|MW_SMFONT);
		DispLineMW("El puerto serial...", MW_LINE5, MW_CENTER|MW_SMFONT);
		WaitKey(9000, 0);
		FreeMW(buffTx);
		FreeMW(buffRx);
		return 0;
	}
	if (AuxTxRdy(id_port))
	{
		//clrscr();
		DispLineMW("Enviando...", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		//centerS(4, 1, COLFIN, "Enviando...");
		if (AuxWrite(buffTx, len, id_port))
		{
			idTimer = os_timer_open();
			os_timer_set(idTimer, timeOut);
			//clrscr();
			//gotoxy(4,7);
			printf("\f Recibiendo...");
			while (os_timer_get(idTimer))
			{
				if (AuxRxRdy(id_port))
				{
					lenRx = AuxRead(buffRx, 1023, id_port);
					if (lenRx > 0)
					{
						//memcpy(TBuffers.gRxBuf, buffRx, lenRx);
						break;
					}
				}
			}
			os_timer_close(idTimer);
			if (!lenRx)
			{
				//screen_message(33, "TIME OUT", 300);
				//gotoxy(4,7);
				printf( "\f TIME OUT");
			}
		}
		else
		{
			//gotoxy(4,7);
			printf( "\f No fue posible");
			//gotoxy(5,7);
			printf("\f Enviar los datos...");
			WaitKey(9000, 0);
		}
	}
	else
	{
		//graphicWindow("ERROR RS232");
		//gotoxy(4,7);
		printf( "\f No esta listo");
		//gotoxy(5,7);
		printf("\f Para enviar...");
		WaitKey(9000, 0);
	}
	if (!AuxClose(id_port))
	{
		//graphicWindow("ERROR RS232");
		//gotoxy(4,7);
		printf("\f No fue posible cerrar");
		//gotoxy(5,7);
		printf("\f El puerto serial...");
		WaitKey(9000, 0);
	}
	FreeMW(buffTx);
	FreeMW(buffRx);
	return lenRx;
}

////////////////////////FUNCIONES PARA PRUEBAS DE COMUNICACION SERIAL//////////////////////////////////////
/******************************************************************************
 * FUNCTION NAME: pruebaSendAndReceive
 *
 * DESCRIPTION:   Prueba sencilla de comunicacion RS232
 *
 * IN:            N/A
 * return:		  N/A
 ******************************************************************************/
/*void pruebaSendAndReceive(void)
{
	int len = 0;

	//memset(TBuffers.gTxBuf, 0x00, COMM_BUFF_SIZE);
	//memset(TBuffers.gRxBuf, 0x00, COMM_BUFF_SIZE);

	memcpy(TBuffers.gTxBuf, "Hola mundo", 10);
	TBuffers.Tlen = strlen(TBuffers.gTxBuf);
	len = serialSendAndReceive(TBuffers.gTxBuf, TBuffers.Tlen, 1000);
	if (len)
	{
		//graphicWindow("RESPUESTA");
		DispLineMW("RESPUESTA", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		gotoxy(3, 2);
		printf("%s", TBuffers.gRxBuf);
		gotoxy(8, 2);
		printf("Len: %d", len);
		WaitKey(9000, 0);
	}
}*/

/******************************************************************************
 * FUNCTION NAME: serialSend2
 *
 * DESCRIPTION:   Envia datos a través del puerto serial
 *
 * IN:            bufferTX   -> Buffer que contiene los datos a enviar.
 *                lenTx      -> Longitud de los datos a enviar.
 ******************************************************************************/
BYTE serialSend2(BYTE *dataTx, int len)
{

	int id_port;
	BYTE *buffTx;

	buffTx = MallocMW(1024);
	if ((buffTx == NULL))
	{

		DispLineMW("ERROR", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		DispLineMW("No fue posible reservar", MW_LINE4, MW_CENTER|MW_SMFONT);
		DispLineMW("Memoria", MW_LINE5, MW_CENTER|MW_SMFONT);
		WaitKey(9000, 0);
		return 0;
	}
	memset(buffTx, 0x00, 1024);
	memcpy(buffTx, dataTx, len);
	id_port = AuxOpen(1, 5, 7);
	if (!id_port)
	{

		DispLineMW("ERROR", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
		DispLineMW("No fue posible abrir", MW_LINE4, MW_CENTER|MW_SMFONT);
		DispLineMW("El puerto serial...", MW_LINE5, MW_CENTER|MW_SMFONT);
		WaitKey(9000, 0);
		FreeMW(buffTx);
		return 0;
	}
	if (AuxTxRdy(id_port))
	{
		if (AuxWrite(buffTx, len, id_port))
		{
			FreeMW(buffTx);
			return TRUE;
		}
		else
		{
			//gotoxy(4,7);
			printf("\f No fue posible");
			//gotoxy(5,7);
			printf("\f Enviar los datos...");
			WaitKey(9000, 0);
			//getch(200);
		}
	}
	else
	{
		//graphicWindow("ERROR RS232");
		//gotoxy(4,7);
		printf("\f No esta listo");
		//gotoxy(5,7);
		printf("\fPara enviar...");
		WaitKey(9000, 0);
		//getch(200);
	}
	if (!AuxClose(id_port))
	{
		//graphicWindow("ERROR RS232");
		//gotoxy(4,7);
		printf( "\f No fue posible cerrar");
		//gotoxy(5,7);
		printf("\fEl puerto serial...");
		WaitKey(9000, 0);
		//getch(200);
	}
	FreeMW(buffTx);
	return 0;

}

