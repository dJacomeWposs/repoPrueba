/*!
*	\file rs232.h
*	\autor Technology And Solutions
*	\date 16 de Febrero de 2012
*	\brief Este archivo contiene todas las funciones para de la comunicacion serial
*/
#ifndef _RS232_H_
#define _RS232_H_

#include "common.h"



/*!
 * \fn int AuxOpen(int gPort,int gCommMode, int gSpeed)
 * \brief Abre (Habilita) el puerto serial del POS
 * \param gPort Numero del puerto
 * \param gCommMode Modo en que se trabajara la comunicacion
 * \param aLogoPtr Arreglo que contiene el Logo
 * \return ID del puerto habilitado
 */
int AuxOpen(int gPort, int gCommMode, int gSpeed);


/*!
 * \fn BOOLEAN AuxClose(int gAuxHandle)
 * \brief cierra (Deshabilita) el puerto serial del POS
 * \param gAuxHandle Identificador del puerto
 * \return TRUE: Puerto cerrado, FALSE: No se pudo cerrar
 */
BOOLEAN AuxClose(int gAuxHandle);




/*!
 * \fn int serialSendAndReceive(BYTE *dataTx, int len, BYTE *dataRx, int timeOut)
 * \brief Envia y recibe datos a traves del puerto serial
 * \param dataTx Buffer que contiene la informacion a transmitir
 * \param len Longitud de los datos a transmitir
 * \param dataRx Buffer donde se almacenara la informacion recibida
 * \param timeOut Tiempo maximo de espera para la recepcion
 * \return Total de bytes recibidos
 */
int serialSendAndReceive(BYTE *dataTx, int len, int timeOut);





BOOLEAN AuxWrite(BYTE *aBuf, DWORD aLen,int gAuxHandle );
BOOLEAN AuxTxRdy(int gAuxHandle);
BOOLEAN AuxRxRdy(int gAuxHandle );
DWORD AuxRead(BYTE *aBuf, DWORD aLen,int gAuxHandle);


void pruebaSendAndReceive(void);
BYTE serialSend2(BYTE *dataTx, int len);


#endif //_RS232_H_






