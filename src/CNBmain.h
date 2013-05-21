/*
 * CNBmain.h
 *
 *  Created on: Jul 6, 2012
 *      Author: danijacm
 */

#ifndef CNBMAIN_H_
#define CNBMAIN_H_

extern BOOLEAN MenuPrincipal(void);
extern BOOLEAN PowerOffRebootMenu(void);
extern BOOLEAN Retiro(void);
extern BOOLEAN MultiCuentaMenu(void);
extern BOOLEAN DepositoMenu(void);
extern BOOLEAN Deposito(void);
extern BOOLEAN RecaudaEmpresa(void);
//extern int pagTarjetaMenu(BYTE *tipoTarjeta, BYTE *titulo);
extern int MenuRecaudaciones(void);
extern int MenuRecaudaCuenta(void);
extern DWORD RecaudacionVariable(void);
extern int PagoTarjeta(void);
extern BOOLEAN MenuPagos(void);
extern BOOLEAN MenuPagoDeBono(void);
extern BOOLEAN menuXperta(void);
extern BOOLEAN MultiFactura(void);
extern int recaudacion2(void);
extern BYTE menuRecargas(BYTE *nombreEmpresa);
extern void ventaDeSeguros(void);
extern void recaudoDistribuidores(void);
extern void compraDeStock(void);
extern void consultaImpresa(void);
extern void consultaDeSaldoCNB(BYTE aOpcion);
extern void MenuConsultasDelCnb(void);
#endif /* CNBMAIN_H_ */
