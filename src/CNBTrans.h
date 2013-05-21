/*
 * CNBTrans.h
 *
 *  Created on: Jul 6, 2012
 *      Author: danijacm
 */

#ifndef CNBTRANS_H_
#define CNBTRANS_H_

extern DWORD InitMsg( BOOLEAN aFromMenu );
extern DWORD RetiroTrans( WORD aTypeAccount );
extern DWORD RetiroMultiCuentaTrans( void );
extern DWORD ConfirmaRetiroTrans( void );
extern DWORD ConsultaDepositoTrans( BYTE aCedRuc );
extern DWORD DepositoTrans( void );
extern DWORD ConfirmaDepositoTrans( void );
extern DWORD ConsultaPagoServiciosTrans( void );
extern DWORD PagoServiciosTrans(void);
extern DWORD PagoServicioMultiCuentaTrans(void);
extern DWORD ConfirmaPagoServicioTrans(void);
extern DWORD ConfirmaPagoTarjetaTrans( void );
extern DWORD CambioClaveTrans( void );
extern DWORD pagoBonoConCedulaTrans(void);
extern DWORD efectivacionPagoBonoTrans(void);
extern DWORD ConfirmaPagoBonoTrans( void );
extern DWORD pagoBonoConTarjetaTrans(void);
extern DWORD CoberturaLoteTrans(void);
extern DWORD ConsultaComprobanteTrans(void);
extern DWORD RetiroTransTest( WORD aTypeAccount );
extern DWORD consultaXperta(BYTE opcion);
extern DWORD efectivacionCreaXperta(BYTE opcion);
extern DWORD ConfirmaCreaXperta( void );
extern DWORD ConfReponerXperta( void );
extern DWORD resumenVentaProd(void);
extern BOOLEAN extKeyBoardTest(void);
extern DWORD identificarBPTrans( void );
extern int recaudaDistribuidores(void);
//extern BOOLEAN recaudoDistriMulticuenta(void);
extern DWORD recaudoRecargasTrans(void);
extern DWORD ConfirmaRecargaTrans( void );
extern BOOLEAN RecargasMulticuentaTrans(void);
extern BYTE consultaVentaDeSegurosTrans(void);
extern BOOLEAN procesoRecaudoEfectivo(BYTE *titulo);
extern BOOLEAN procesoRecaudoDebitoCuenta(BYTE *titulo);
extern DWORD pagoDeSegurosTrans(BOOLEAN multicuenta);
extern DWORD ConfirmarTransaccion( BYTE aTrans );
extern BOOLEAN validarPagoDeSeguro(void);
extern BOOLEAN consultaRecaudoDistriTrans(void);
extern DWORD pagoRecaudoDistriTrans(BOOLEAN multicuenta);
extern BYTE compraDeStockTrans(BOOLEAN multicuenta);
extern BOOLEAN consultaComisionTrans(BYTE opcion);
extern BYTE consultaTrans(BYTE aOpcion, BOOLEAN aMulticuenta);
//extern DWORD reimpresionDoc(void);



#endif /* CNBTRANS_H_ */
