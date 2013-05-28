/*
 * CNBmain.c
 *
 *  Created on: Jul 6, 2012
 *      Author: danijacm
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "sysutil.h"
#include "menu.h"
#include "CNBmain.h"
#include "CNBAdmin.h"
#include "CNBTrans.h"
#include "input.h"
#include "Corevar.h"
#include "tranutil.h"
#include "Print.h"
#include "message.h"
#include "Constant.h"
#include "FUNC.H"
#include "reversal.h"
#include "keytrans.h"
#include "download.h"

/*struct MENU_ITEM KMainItem [] =	// Este es al antiguo menu principal
{
  {  1, "Retiro"},
  {  2, "Deposito"},
  {  3, "Recaudaciones"},
  {  4, "Cuenta Xperta"},
  {  5, "Cambio de Clave"},
  {  6, "Pagos"},
  {  7, "Pago Tarj.credito"},
  {  8, "Imprimir Ultimo"},
  {  9, "Administrativo"},
  {  10,"Visita Funcionario BP"},
  {  11,"Recaudacion Seguro"},
  {  12,"Consulta Impresa"},
  {	 13, "Borrar Reverso" 	},// Opcion agregada solo para desarrollo
  {  14, "Apagar / Reiniciar"},
  {-1, NULL},
};

static const struct MENU_DAT KMainMenu  = {
  "MENU PRINCIPAL",
  KMainItem,
};*/

/******ESTE ES EL NUEVO MENU PRINCIPAL ENTREGADO POR GRUPO MAS 25/02/13******/
struct MENU_ITEM KMenuPpalItem [] =
{
  {  1, "TRANSACCIONES CUENTAS"},
  {  2, "PRODUCTOS"},
  {  3, "RECAUDACIONES"},
  {  4, "PAGOS"},
  {  5, "ADMINISTRATIVO"},
  {  6, "APAGAR / REINICIAR"},
  {  7, "CONFLICTO"},
  {-1, NULL},
};

static const struct MENU_DAT KMenuPrincipal  = {
  "MENU PRINCIPAL",
  KMenuPpalItem,
};
/*****************************************************************************/



struct MENU_ITEM KSumMenu01Item [] =
{
  {  1, "RETIRO"},
  {  2, "RETIRO MONTO BAJO"},	//Compra debito Experta
  {  3, "DEPOSITO"},
  {  4, "CONSULTA IMPRESA"},
  {  5, "COMPRA DEBITO XPERTA"},
  {  6, "CAMBIO CLAVE EXPERTA"},
  {-1, NULL},
};

static const struct MENU_DAT KSubMenu01  = {
  "TRANSACCIONES CUENTAS",
  KSumMenu01Item,
};



struct MENU_ITEM KSumMenu02Item [] =
{
  {  1, "CLIENTE REFERIDO"},
  {  2, "CUENTA EXPERTA"},
  {  3, "RECAUDACION SEGURO"},
  {  4, "RECAUDACION LOTERIA"},
  {  5, "RECARGAS PREPAGO"},
  {  6, "RECAUDACION OTROS PROD"},
  {-1, NULL},
};

static const struct MENU_DAT KSubMenu02  = {
  "PRODUCTOS",
  KSumMenu02Item,
};


struct MENU_ITEM KSumMenu03Item [] =
{
  {  1, "SERV BASICOS Y EMPRESAS"},
  {  2, "PAGO TARJETA CREDITO"},
  {  3, "RECAUDO DISTRIBUIDOR"},
  {  4, "STOCK RECARGA PREPAGO"},
  {  5, "TRANSF. OTROS BANCOS"},
  {  6, "GIROS"},
  {-1, NULL},
};

static const struct MENU_DAT KSubMenu03  = {
  "RECAUDACIONES",
  KSumMenu03Item,
};


struct MENU_ITEM KSumMenu04Item [] =
{
  {  1, "BONO DESARROLLO HUMANO"},
  {  2, "ORDEN DE PAGO CASH"},
  {  3, "TRANSF. PAGO DIRECTO"},
  {  4, "AVANCE TARJETA"},
  {  5, "REMESAS"},
  {-1, NULL},
};

static const struct MENU_DAT KSubMenu04  = {
  "PAGOS",
  KSumMenu04Item,
};


struct MENU_ITEM KSumMenu05Item [] =
{
  {  1, "VISITA DE FUNCIONARIO"},
  {  2, "COMPROBANTE"},
  {  3, "INFORMES"},
  {  4, "OPERADOR"},
  {  5, "OTROS"},
  {  6, "CONSULTA SALDO CNB"},
  {  7, "MENSAJES CAMPO 60"},
  {-1, NULL},
};

static const struct MENU_DAT KSubMenu05  = {
  "ADMINISTRATIVO",
  KSumMenu05Item,
};




static const struct MENU_ITEM KSubMenuOtrosItem [] =
{
  {  1, "CONFIGURACION"},
  {  2, "MODO DE CONEXION"},
  {  3, "CARGAR MK"},
  {  4, "EJECUTOR DE PAGOS"},
  {  5, "VERSIONES"},
  {  6, "PARAMETROS"},
  {  7, "DESCARGA FTP"},	//OJO NOHORA 12 - MAR 2013//
  {-1, NULL},
};

static const struct MENU_DAT KSubMenuOtros  = {
  "OTROS",
  KSubMenuOtrosItem,
};


static const struct MENU_ITEM KPagosItem [] =
{
  {  1, "Bono Desrr. Humano"},
  {-1, NULL},
};


static const struct MENU_DAT KPagosMenu  = {
  "MENU PAGOS",
  KPagosItem,
};

static const struct MENU_ITEM KRecaudaItem [] =
{
  {  1, "Empresas"},
  {  2, "Distribuidores"},
  {  3, "Recargas / Prepago"},
  {  4, "Compra Stock"},
  {-1, NULL},
};


static const struct MENU_DAT KRecaudaMenu  = {
  "RECAUDACIONES",
  KRecaudaItem,
};



static const struct MENU_ITEM KPagoBonoItem [] =
{
  {  1, "Cedula del Beneficiario"},
  {  2, "Tarjeta del Beneficiario"},
  {-1, NULL},
};


static const struct MENU_DAT KPagoBonoMenu  = {
  "PAGO DE BONO",
  KPagoBonoItem,
};



static const struct MENU_ITEM KCuentaItem [] =
{
  {  1, "Cuenta Corriente"},
  {  2, "Cuenta Ahorros"},
  {  3, "Cuenta Basica"},
  {-1, NULL},
};

static const struct MENU_DAT KCuentaMenu  = {
  "DEPOSITO",
  KCuentaItem,
};


static const struct MENU_DAT KCuentaMenu2  = {
  "RECAUDACIONES",
  KCuentaItem,
};


static const struct MENU_ITEM KRecaudacionesItem [] =
{
  {  1, "Efectivo"},
  {  2, "Debito a Cuenta"},
  {-1, NULL},
};


struct MENU_DAT KRecaudacionesMenu  = {
  "RECAUDACIONES",
  KRecaudacionesItem,
};

static const struct MENU_DAT KRetiroMenu  = {
  "RETIRO",
  KCuentaItem,
};

static const struct MENU_ITEM KDepositoItem [] =
{
  {  1, "Cedula"},
  {  2, "Ruc"},
  {-1, NULL},
};
static const struct MENU_DAT KDepositoMenu  = {
  "DEPOSITO",
  KDepositoItem,
};

static const struct MENU_ITEM KPowerOffRebootItem [] =
{
  {  1, "Apagar"},
  {  2, "Reiniciar"},
  {-1, NULL},
};

static const struct MENU_DAT KPowerOffRebootMenu  = {
  "Apagar/Reiniciar",
  KPowerOffRebootItem,
};

static const struct MENU_ITEM KXpertaItem [] =
{
  {  1, "Creacion de cuenta"},
  {  2, "Reposicion de kit"},
  {-1, NULL},
};
static const struct MENU_DAT KXpertaMenu  = {
  "CUENTA XPERTA",
  KXpertaItem,
};


static const struct MENU_ITEM KConsultaItem [] =
{
  {  1, "SALDO EN CUENTA"},
  {  2, "1O ULTIMOS MOV"},
  {-1, NULL},
};



static const struct MENU_DAT KConsultaMenu  = {
  "CONSULTA IMPRESA",
  KConsultaItem,
};



static const struct MENU_ITEM KConsultaItem_2 [] =
{
  {  1, "Cuenta Corriente"},
  {  2, "Cuenta de Ahorros"},
  {  3, "Cuenta Xperta"},
  {-1, NULL},
};

static const struct MENU_DAT KConsultaMenu_2  = {
  "CONSULTA DE SALDO",
  KConsultaItem_2,
};



struct MENU_ITEM KMultplesCuentasItem[5];

struct MENU_DAT KMultiplesCuentasMenu  = {
  "SELECCIONE CUENTA",
  KMultplesCuentasItem,
};


struct MENU_ITEM KMultplesFactura[20];

struct MENU_DAT KMultiplesFacturaMenu  = {
  "SELECCIONE FACTURA",
  KMultplesFactura,
};



static const struct MENU_ITEM KRecargastem [] =
{
  {  1, "Movistar"},
  {  2, "Claro"},
  {  3, "CNT"},
  {  4, "Directv"},
  {  5, "Otros"},
  {-1, NULL},
};
static const struct MENU_DAT KRecargasMenu  = {
  "RECARGAS",
  KRecargastem,
};



struct MENU_ITEM KCnbOPciones [] =
{
  {  1, "CONSULTA DE SALDO"},
  {  2, "CONSULTA ULT. MOV."},
  {-1, NULL},
};

static const struct MENU_DAT KMenuConsultasCnb  = {
  "CONSULTAS DEL CNB",
  KCnbOPciones,
};


/*BOOLEAN MenuPrincipal(void)
{
  int select = 0,  key = 0;

  while(TRUE)
  {
      os_lpt_open(NULL);

      if(no_paper())
      {

        do{
        printf("\f");
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        LongBeep();
        key=APM_WaitKey(200,0);

        if(!no_paper())
          break;

        }
        while(key != MWKEY_ENTER);  
      }
      os_lpt_close();


    select = MenuSelect(&KMainMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        } 
        Retiro();
        break;

      case 2:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        Deposito();
        break;

      case 3:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        recaudacion2();
        break;

      case 4:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        menuXperta();
        break;  

      
      case 5:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        CambioClaveTrans();
        break;

      case 6:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        MenuPagos();
        break;

      case 7:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        PagoTarjeta();
        break;

      case 8:
        //if( gUsers_tbl.b_type == KESPECIALISTA )
       // {
        //  MsgError("ACCESO DENEGADO");
        //  break;
        //}
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
      ReprintLast();
        break;
    
	  
      case 9:
       // if( gUsers_tbl.b_type == KOPERADOR )
       // {
       //   MsgError("ACCESO DENEGADO");
       //   break;
       // }

        AdminMenu();
        break;

      case 10:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        identificarBPTrans();
        break;

      case 11:
          if( gUsers_tbl.b_type == KESPECIALISTA )
          {
            MsgError("ACCESO DENEGADO");
            break;
          }
          if( gTerminalInit == FALSE )
          {
            MsgError("NO HAY CONEXION");
            break;
          }
          ventaDeSeguros();
    	  break;

      case 12:
    	  //consulta impresa
    	  consultaImpresa();
    	  break;

      case 13:
    	  clearReversoPichincha();
    	  break;

      case 14:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        PowerOffRebootMenu();
        break;
    }
  }
  return TRUE;
}*/




void subMenu01(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenu01, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	Retiro();
    	break;

    case 2:
    	//Compra debito xperta
    	break;

    case 3:
    	Deposito();
    	break;

    case 4:
    	consultaImpresa();
    	break;

    case 5:
    	//Compra debito xperta
    	break;

    case 6:
    	CambioClaveTrans();
    	break;

    default:
    	break;
    }
  }
}





void subMenu02(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenu02, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	//Cliente referido
    	break;

    case 2:
    	menuXperta();
    	break;

    case 3:
    	ventaDeSeguros();
    	break;

    case 4:
    	//Recaudacion Loterias
    	break;

    case 5:
    	select = recaudoRecargasTrans();
    	switch(select){
    	case 1:
    		ConfirmaRecargaTrans();
    		break;

    	case 2:
  		  if( RecargasMulticuentaTrans() )
  			  ConfirmaRecargaTrans();
  		  break;
    	}

    	break;

    case 6:
    	//Recaudacion otros productos
    	break;

    default:
    	break;
    }
  }
}



void subMenu03(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenu03, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	RecaudaEmpresa();
    	break;

    case 2:
    	PagoTarjeta();
    	break;

    case 3:
    	recaudoDistribuidores();
    	break;

    case 4:
    	compraDeStock();
    	break;

    case 5:
    	//Transferencias a otros bancos
    	break;

    case 6:
    	//Giros
    	break;

    default:
    	break;
    }
  }
}



void subMenu04(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenu04, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	MenuPagoDeBono();
    	break;

    case 2:
    	//Orden de pago Cash
    	break;

    case 3:
    	//Transferencia pago directo
    	break;

    case 4:
    	//Avance tarjeta
    	break;

    case 5:
    	//Remesas
    	break;

    default:
    	break;
    }
  }
}




void subMenuOtros(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenuOtros, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	ConfigMenu();
    	break;

    case 2:
    	SrvEthMenu();
    	break;

    case 3:
    	IccTmkInject();
    	break;

    case 4:
    	OperadorMenu();
    	break;

    case 5:
    	verVersionSyspack();
    	break;

    case 6:
    	printParam();
    	break;

    case 7:	//OJO NOHORA 12 - MAR 2013//
    	flagFtpManual = TRUE;
    	if( DownloadFtp() == TRUE )
    		ResetMW();

    	break;

    default:
    	break;
    }
  }
}




void subMenu05(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSubMenu05, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	identificarBPTrans();
    	break;

    case 2:
    	MenuComprobante();
    	break;

    case 3:
    	MenuInformes();
    	break;

    case 4:
    	OperadorMenu();
    	break;

    case 5:
    	subMenuOtros();
    	break;

    case 6:
    	MenuConsultasDelCnb();
      break;

    case 7:
      PrintMsg60Test2();
      break;     

    default:
    	break;
    }
  }
}







BOOLEAN MenuPrincipal(void)
{
  int select = 0,  key = 0;
 
  while(TRUE)
  {
      os_lpt_open(NULL);

      if(no_paper())
      {

        do{
        printf("\f");
        Disp2x16Msg(GetConstMsg(EDC_PN_NO_PAPER), MW_LINE5, MW_BIGFONT);
        LongBeep();
        key=APM_WaitKey(200,0);

        if(!no_paper())
          break;

        }
        while(key != MWKEY_ENTER);
      }
      os_lpt_close();


    select = MenuSelect(&KMenuPrincipal, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        subMenu01();
        break;

      case 2:
       
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        subMenu02();
        break;

      case 3:
      
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        subMenu03();
        break;

      case 4:
      
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        if( gTerminalInit == FALSE )
        {
          MsgError("NO HAY CONEXION");
          break;
        }
        subMenu04();
        break;


      case 5:
      
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        subMenu05();
        break;

      case 6:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        PowerOffRebootMenu();
        break;
    }
  }
  return TRUE;
}


BOOLEAN PowerOffRebootMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KPowerOffRebootMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        PromptPowerOff();
        break;
      case 2:
        PromptReboot();
        break;
    }
  }
  return TRUE;
}



BOOLEAN Retiro(void)
{
  int select = 0;
  int typeAcc, ret;
  gMultiCuenta = FALSE;

  select = MenuSelect(&KRetiroMenu, select);
  if (select == -1)

    return FALSE;

  typeAcc = select ;
  ret = RetiroTrans( typeAcc );
  if( ret == 2 )
  {
	 // Arma Menu Multiples Cuentas
    if( !MultiCuentaMenu() )
    	return FALSE;

    if( RetiroMultiCuentaTrans( )  == TRUE )
    {
    	//printf("\fConfirmar Retiro");WaitKey(3000, 0);
    	ConfirmaRetiroTrans();
    }

  }
  else if( ret  == TRUE )
  {
    ConfirmaRetiroTrans();
	  //printf("\fConfirmar Retiro");WaitKey(3000, 0);
  }
  else
	  return FALSE;

  return TRUE;
}



BOOLEAN MultiCuentaMenu(void)
{
  WORD i;
  int select;
  BYTE aux[3];
  memset(aux,0x00,sizeof(aux));
  memcpy(aux,gInfo_98[0].cantCuentas,2);

  for( i = 0; i < atoi(aux); i++ )
  {
    KMultplesCuentasItem[i].iID = i + 1;
    memcpy( KMultplesCuentasItem[i].scDescDat, gInfo_98[i].cuentas, 18 );
  }
  KMultplesCuentasItem[i].iID = -1;
  KMultplesCuentasItem[i].pcDesc = NULL;

  select = 0;
  select = MenuSelect(&KMultiplesCuentasMenu, select);
  if (select == -1)
    return FALSE;

  memcpy( gInfo_98_pack.cantCuentas, "01", 2 );
  memcpy( gInfo_98_pack.cuentas, gInfo_98[select - 1].cuentas, 18 );

  return TRUE;

}


BOOLEAN MultiFactura(void)
{
	WORD i, cantFacturas;
	int select, intMonto = 0, intMonto2 = 0, totalMonto=0;
	BYTE auxFac[20 + 1], monto[12 + 1], costServ[13], lineaAux[30];

	memset(auxFac,0x00,sizeof(auxFac));
	memset(monto,0x00,sizeof(monto));
	memset(costServ,0x00,sizeof(costServ));
	memset(lineaAux,0x00,sizeof(lineaAux));

	cantFacturas = bcd2bin(gToken_5.cantFacturas);

	if(cantFacturas == 0){
		return 2;
	}

	while(true){
		for( i=0; i<cantFacturas; i++ )
		{
			KMultplesFactura[i].iID = i + 1;
			memcpy( KMultplesFactura[i].scDescDat, gToken_5.facturaEmp[i].factura, 10 );
		}

		KMultplesFactura[i].iID = -1;
		KMultplesFactura[i].pcDesc = NULL;

		select = 0;
		select = MenuSelect(&KMultiplesFacturaMenu, select);
		if (select == -1){
			return FALSE;
		}

		memset(auxFac,0x00,sizeof(auxFac));
		memcpy(auxFac, gToken_5.facturaEmp[select - 1].factura, 20);

		//******Armo el token 5 para enviar al host**********//
		memset(&gToken_5_pack, 0x00, sizeof(struct TOKEN_5));
		memcpy(gToken_5_pack.nomEmpresa, gToken_5.nomEmpresa, 20);
		memcpy(gToken_5_pack.nomPersona, gToken_5.nomPersona, 20);
		memcpy(gToken_5_pack.costServ, gToken_5.costServ, 2);
		gToken_5_pack.cantFacturas = 0x01;	// El POS siempre enviara una factura
		//memcpy(lineaAux, gToken_5.facturaEmp[select - 1].fecha, 10);
		//printf("\f|%s|", lineaAux);APM_WaitKey(3000, 0);
		memcpy(gToken_5_pack.facturaEmp[0].fecha, gToken_5.facturaEmp[select - 1].fecha, 10);
		memcpy(gToken_5_pack.facturaEmp[0].factura, gToken_5.facturaEmp[select - 1].factura, 20);
		memcpy(gToken_5_pack.facturaEmp[0].valor, gToken_5.facturaEmp[select - 1].valor, 12);


		DispLineMW( "CONFIRMACION", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
		DispLineMW( "EMPRESA:", MW_LINE2 + 2, MW_SPFONT|MW_LEFT );
		//DispLineMW( INPUT.sb_cod_empresa, MW_LINE2 + 11, MW_SPFONT|MW_LEFT );
		memcpy(lineaAux, gToken_5.nomEmpresa, 20);
		DispLineMW( lineaAux, MW_LINE3 + 2, MW_SPFONT|MW_LEFT );

		DispLineMW( "FACTURA:", MW_LINE4 + 2, MW_SPFONT|MW_LEFT );
		DispLineMW( auxFac, MW_LINE5 + 2, MW_SPFONT|MW_LEFT );

		DispLineMW( "VALOR:", MW_LINE6, MW_LEFT|MW_SPFONT );
		memcpy(monto, gToken_5.facturaEmp[select - 1].valor, 12);
		intMonto = atoi(monto);
		DispAmnt(intMonto, MW_LINE6, MW_SMFONT);

		DispLineMW( "Servicio:", MW_LINE7, MW_LEFT|MW_SPFONT );
		split(costServ, gToken_5.costServ, 2);
		intMonto2 = atoi(costServ);
		DispAmnt(intMonto2, MW_LINE7, MW_SMFONT);

		totalMonto = intMonto + intMonto2;

		DispLineMW( "TOTAL:", MW_LINE8, MW_LEFT|MW_SMFONT );
		DispAmnt(totalMonto, MW_LINE8, MW_SMFONT);

		if( SiNo() != 2 ) {  // 2 = Press Enter
			//APM_ResetComm();
			continue;
		}
		else
			break;
	}

	memset(gAmount,0x00,sizeof(gAmount));
	sprintf(gAmount,"%012d",totalMonto);

	return TRUE;
}


BOOLEAN Deposito(void)
{
  int select = 0;
  gMsg60Show = FALSE;

  select = MenuSelect(&KCuentaMenu, select);
  if (select == -1)
    return FALSE;

  switch (select)
  {
    case 1:
      strcpy( gCuenta, "01" );  // Global y no parametro porque en futuro gCuenta se utilizara en otros archivos
      DepositoMenu();
      break;
    case 2:
      strcpy( gCuenta, "02" );
      DepositoMenu();
      break;
    case 3:
      strcpy( gCuenta, "03" );
      DepositoMenu();

      break;

  }
  return TRUE;
}



BOOLEAN DepositoMenu(void)
{
  int select = 0;
  int ret;

  select = MenuSelect(&KDepositoMenu, select);
  if (select == -1)
    return FALSE;

  if( ConsultaDepositoTrans( select ) == TRUE )
  {
	ret = DepositoTrans();
	if( ret == TRUE)
    {
		ConfirmaDepositoTrans();
    }
  }

  return TRUE;
}



int MenuRecaudaciones(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KRecaudacionesMenu, select);
    if (select == -1)
      break;
    else
    	return select;
  }
  return -1;
}



int MenuRecaudaCuenta(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KCuentaMenu2, select);
    if (select == -1)
      break;
    else
    	return select;
  }
  return -1;
}



BOOLEAN MenuPagos(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KPagosMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  if( MenuPagoDeBono() == FALSE )
    		  return FALSE;
        break;

      case 2:
        return FALSE;
        break;
    }
  }
  return TRUE;
}



BOOLEAN MenuPagoDeBono(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KPagoBonoMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  if( pagoBonoConCedulaTrans() == -1 )
    		  return FALSE;
        break;

      case 2:
    	  if( pagoBonoConTarjetaTrans() == -1 )
    		  return FALSE;
        break;
    }
  }
  return TRUE;
}



BOOLEAN RecaudaEmpresa(void)
{
  int ret;
  int ret2 = 0;
  gRecaudoVar = FALSE;

  ret2 = ConsultaPagoServiciosTrans();
  if( ret2 == TRUE )
  {
    ret = PagoServiciosTrans();
    if(ret == 2)
    {
      // Arma Menu Multiples Cuentas
      if( !MultiCuentaMenu() ){
    	  gPagoTarjeta = FALSE;
    	  return -1;
      }
      if( PagoServicioMultiCuentaTrans( )  == TRUE )
      {
        // Confirmacion de pago de servicios
    	    if( gPagoTarjeta == TRUE )
    	        ConfirmaPagoTarjetaTrans();
    	    else
    	        ConfirmaPagoServicioTrans();
      }
    }
    else if (ret == TRUE)
    {
      if( gPagoTarjeta == TRUE )
        ConfirmaPagoTarjetaTrans();
      else
        ConfirmaPagoServicioTrans();
    }

  }
  else if(ret2 == 2){	// La recaudacion es variable
	  RecaudacionVariable();
  }
  gPagoTarjeta = FALSE;
  return TRUE;
}




DWORD RecaudacionVariable(void){
	int ret =0;
	int intMonto;
	BYTE empresa[21];
  BYTE nmPersona[21]; /// LFLF
	BYTE monto[13];
	//DDWORD dd_monto;
	BYTE titulo[26];

	memset(empresa, 0x00, sizeof(empresa));
  memset(nmPersona, 0x00, sizeof(nmPersona));         /// LFLF
	memset(monto, 0x00, sizeof(monto));
	memset(titulo, 0x00, sizeof(titulo));

	gRecaudoVar = TRUE;
	memcpy(empresa, gToken_6.nomEmpresa, 20);
  memcpy(nmPersona, gToken_6.nomPersona, 20);  /// LFLF 
	if( gPagoTarjeta == TRUE )
		strcpy(titulo, "PAGO DE TARJETA");
	else
		strcpy(titulo, "RECAUDACIONES");
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Empresa:", MW_LINE2 + 2, MW_SMFONT|MW_LEFT );
	DispLineMW( empresa, MW_LINE3, MW_SPFONT|MW_LEFT );
  //DispLineMW( gToken_6.nomPersona, MW_LINE4, MW_SPFONT|MW_CENTER );
  DispLineMW( nmPersona, MW_LINE4, MW_SPFONT|MW_CENTER ); //  LFLF
	DispLineMW( "Servicio:", MW_LINE5, MW_LEFT|MW_SPFONT );
	split(monto, gToken_6.costo_servicio, 6);
	intMonto = atoi(monto);
	memset(monto, 0x00, sizeof(monto));
	DispAmnt(intMonto, MW_LINE5, MW_SMFONT);
	DispLineMW( "Adecuado:", MW_LINE6, MW_LEFT|MW_SMFONT );
	split(monto, gToken_6.valor_adeudado, 6);
	intMonto = atoi(monto);
	DispAmnt(intMonto, MW_LINE6, MW_SMFONT);
	DispLineMW( "Adecuado:", MW_LINE6, MW_LEFT|MW_SMFONT );
	DispLineMW( "Monto:", MW_LINE7, MW_LEFT|MW_SMFONT );

	if( !GetAmnt(MW_LINE7) ){
		//APM_ResetComm();
		return -1;
	}

	if( SiNo() != 2 ) {  // 2 = Press Enter
		//APM_ResetComm();
		return -1;
	}
	if ( ConsultaPagoServiciosTrans() == TRUE ){
		ret = PagoServiciosTrans();
		if(ret == 2){
			if( !MultiCuentaMenu() ){
				//APM_ResetComm();
				return -1;
			}
			if( PagoServicioMultiCuentaTrans( )  == TRUE ){
				// Confirmacion de pago de servicios
				if( gPagoTarjeta == TRUE ){
					ConfirmaPagoTarjetaTrans();
				}
				else
					ConfirmaPagoServicioTrans();
			}
		}
		else if (ret == TRUE)
		{
		  if( gPagoTarjeta == TRUE ){
			  ConfirmaPagoTarjetaTrans();
		  }
		  else
		    ConfirmaPagoServicioTrans();
		}
		else
			return -1;
	}
	return TRUE;
}



/*int recaudacion2(void){

  int select = 0;
  int opcion;

  while(TRUE)
  {
    select = MenuSelect(&KRecaudaMenu, select);
    if (select == -1)
      return -1;

    switch (select)
    {
      case 1:
        RecaudaEmpresa();
        break;

      case 2:	// Recaudo de distribuidores
    	  recaudoDistribuidores();
        break;

      case 3:	//Recaudo de recargas
    	  opcion = recaudoRecargasTrans();
    	  switch(opcion){
    	  case 1:
    		  ConfirmaRecargaTrans();
    		  break;

    	  case 2:
    		  if( RecargasMulticuentaTrans() ){
    			  ConfirmaRecargaTrans();
    		  }
    		  break;

    	  default:
    		  break;
    	  }
    	  break;
      
      case 4:
    	  compraDeStock();
        break;    
    }
  }
  return TRUE;
}*/




int PagoTarjeta(void)
{

  gPagoTarjeta = TRUE;

  // EL PAGO DE TARJETA DE CREDITO SE VA A MAEJAR COMO UNA RECAUDACION
  //Recaudaciones();
  RecaudaEmpresa();

  return TRUE;
}



BOOLEAN menuXperta(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KXpertaMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        consultaXperta(1);
        break;

      case 2:
        //reponerXperta();
    	  consultaXperta(2);
        break;
    }
  }
  return TRUE;
}






BYTE menuRecargas(BYTE *nombreEmpresa)
{
  int select = 0;

  memset(INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa));
  memset(INPUT.sb_contrapar,   0x00, sizeof(INPUT.sb_contrapar));

  while(TRUE)
  {
    select = MenuSelect(&KRecargasMenu, select);
    //printf("\fSelect 1: %d", select);APM_WaitKey(3000, 0);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  strcpy(INPUT.sb_cod_empresa, "1");
    	  strcpy(nombreEmpresa, "Movistar");
    	  LSetStr( INPUT.sb_cod_empresa, 10, '0' );
    	  return select;
        break;

      case 2:
    	  strcpy(INPUT.sb_cod_empresa, "2");
    	  strcpy(nombreEmpresa, "Claro");
    	  LSetStr( INPUT.sb_cod_empresa, 10, '0' );
    	  return select;
        break;

      case 3:
    	  strcpy(INPUT.sb_cod_empresa, "3");
    	  strcpy(nombreEmpresa, "CNT");
    	  LSetStr( INPUT.sb_cod_empresa, 10, '0' );
    	  return select;
        break;

      case 4:
    	  strcpy(INPUT.sb_cod_empresa, "4");
    	  strcpy(nombreEmpresa, "Directv");
    	  LSetStr( INPUT.sb_cod_empresa, 10, '0' );
    	  return select;
        break;

      case 5:
    	  DispLineMW( "", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
    	  DispLineMW( "Codigo de Empresa:", MW_LINE2, MW_LEFT|MW_SMFONT );
    	  if( !setCodigoEmpresa(MW_LINE3, 4, 1, 10) )
    		  break;
    	  memcpy(nombreEmpresa, INPUT.sb_cod_empresa, strlen(INPUT.sb_cod_empresa));
    	  DispLineMW( "Contrapartida:", MW_LINE5, MW_LEFT|MW_SMFONT );
    	  if( !setContrapartida(MW_LINE6, 4, 1, 20) )
    		  break;
    	  LSetStr( INPUT.sb_cod_empresa, 10, '0' );
    	  RSetStr( INPUT.sb_contrapar, 34, ' ' );
    	  DispLineMW( "Valor:", MW_LINE8, MW_LEFT|MW_SMFONT );
    	  if( !GetAmnt(MW_LINE9) ){
    		break;
    	  }
    	  return select;
        break;

      default:
        break;
    }
  }
  return select;
}



BOOLEAN ventaSegurosVariable(void){
	int intMonto;
	BYTE empresa[21];
    BYTE nmPersona[21]; /// LFLF
	BYTE monto[13];

	memset(empresa, 0x00, sizeof(empresa));
    memset(nmPersona, 0x00, sizeof(nmPersona));         /// LFLF
	memset(monto, 0x00, sizeof(monto));

	gRecaudoVar = TRUE;
	memcpy(empresa, gToken_6.nomEmpresa, 20);
    memcpy(nmPersona, gToken_6.nomPersona, 20);  /// LFLF
	DispClrBelowMW(MW_LINE2);
	DispLineMW( "Empresa:", MW_LINE2 + 2, MW_SMFONT|MW_LEFT );
	DispLineMW( empresa, MW_LINE3, MW_SPFONT|MW_LEFT );
    DispLineMW( nmPersona, MW_LINE4, MW_SPFONT|MW_CENTER ); //  LFLF
	DispLineMW( "Servicio:", MW_LINE5, MW_LEFT|MW_SPFONT );
	split(monto, gToken_6.costo_servicio, 6);
	intMonto = atoi(monto);
	memset(monto, 0x00, sizeof(monto));
	DispAmnt(intMonto, MW_LINE5, MW_SMFONT);
	DispLineMW( "Adecuado:", MW_LINE6, MW_LEFT|MW_SMFONT );
	split(monto, gToken_6.valor_adeudado, 6);
	intMonto = atoi(monto);
	DispAmnt(intMonto, MW_LINE6, MW_SMFONT);
	DispLineMW( "Adecuado:", MW_LINE6, MW_LEFT|MW_SMFONT );
	DispLineMW( "Monto:", MW_LINE7, MW_LEFT|MW_SMFONT );

	if( !GetAmnt(MW_LINE7) )
		return FALSE;

	if( SiNo() != 2 )   // 2 = Press Enter
		return FALSE;

	return TRUE;
}




void ventaDeSeguros(void){
	BOOLEAN rta;
	int select;

	gRecaudoVar = FALSE;
  gMsg60Show = FALSE;

	rta = consultaVentaDeSegurosTrans();
	if(rta == TRUE){
		if( !validarPagoDeSeguro() )	// Valida la informacion y elige si desea pagar
			return;

		//OJO NOHORA 14 - MAR 2013//
		/*select = MenuRecaudaciones();	// se elige si se desea pagar en efectivo o con debito a cuenta

		if (select == -1)
		  return;

		if(select == 1){	//Venta de seguros en efectivo
			if( procesoRecaudoEfectivo() ){
				if( pagoDeSegurosTrans(FALSE) == 1 )
					ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
			}
		}*/
		//else if(select == 2){	//Venta de seguros con debito a cuenta
			if( procesoRecaudoDebitoCuenta("VENTA DE SEGUROS") ){
				select = pagoDeSegurosTrans(FALSE);
				switch(select){
				case 1:
					ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
					break;

				case 2:
					if( pagoDeSegurosTrans(TRUE) ){ //Pago de Seguros con multiple cuentas
						ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
					}
					break;

				default:
					break;
				}

			}
		//}
	}
	else if(rta == 2){
		//printf("\fPago Variable");APM_WaitKey(3000, 0);
		if( !ventaSegurosVariable() )
			return;
		rta = consultaVentaDeSegurosTrans();
		if(rta == TRUE){
			if( !validarPagoDeSeguro() )	// Valida la informacion y elige si desea pagar
				return;

			/*select = MenuRecaudaciones();	// se elige si se desea pagar en efectivo o con debito a cuenta

			if (select == -1)
			  return;

			if(select == 1){	//Venta de seguros en efectivo
				if( procesoRecaudoEfectivo() ){
					if( pagoDeSegurosTrans(FALSE) == 1 )
						ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
				}
			}*/
			//else if(select == 2){	//Venta de seguros con debito a cuenta
				if( procesoRecaudoDebitoCuenta("VENTA DE SEGUROS") ){
					select = pagoDeSegurosTrans(FALSE);
					switch(select){
					case 1:
						ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
						break;

					case 2:
						if( pagoDeSegurosTrans(TRUE) ){ //Pago de Seguros con multiple cuentas
							ConfirmarTransaccion(CONFIRMA_VENTA_SEGURO);	//Confirmar pago de seguros
						}
						break;

					default:
						break;
					}

				}
			//}
				//OJO NOHORA 14 - MAR 2013//
		}
	}
}



void recaudoDistribuidores(void){
	int select;

	if( consultaRecaudoDistriTrans() ){
		select = MultiFactura();
		if(select == FALSE){
			showMessage(14, 3, "RECAUDO");
			return;
		}
		else if(select == 2){
			showMessage(16, 3, "RECAUDO");
			return;
		}
		select = MenuRecaudaciones();	// se elige si se desea pagar en efectivo o con debito a cuenta

		if (select == -1)
		  return;

		if( select == 2 )	// Pago con debito a cuenta
		{
		    if( !procesoRecaudoDebitoCuenta("REC. DISTRIBUIDOR") ){
				return;
			}
			else{
				select = pagoRecaudoDistriTrans(FALSE);
				switch(select){
					case 1:
						ConfirmarTransaccion(CONFIRMA_RECAUDO_DIST);	//Confirmar Recaudo de Distribuidores
					break;

					case 2:
						if( pagoRecaudoDistriTrans(TRUE) )
							ConfirmarTransaccion(CONFIRMA_RECAUDO_DIST);	//Confirmar Recaudo de Distribuidores
					break;

					default:
					break;
				}
			}
		}
		else	// Pago en efectivo
		{
			if( !procesoRecaudoEfectivo("REC. DISTRIBUIDOR") ){
				return;
			}
			else{
				if( pagoRecaudoDistriTrans(FALSE) == TRUE )
					ConfirmarTransaccion(CONFIRMA_RECAUDO_DIST);	//Confirmar Recaudo de Distribuidores
			}
		}
	}
}



BOOLEAN setCompraDeStock(void){
	BYTE kbdbuff[50];
	BYTE tmpEmpresa[10 + 1];
	BYTE tmpEstablecimiento[20 + 1];

	memset(tmpEmpresa, 		   0x00, sizeof(tmpEmpresa));
	memset(tmpEstablecimiento, 0x00, sizeof(tmpEstablecimiento));

	INPUT.b_trans = COMPRA_STOCK;

	//PackComm(TRUE, FALSE);
	//APM_PreConnect();

	DispLineMW( "COMPRA DE STOCK", MW_LINE1, MW_CENTER|MW_REVERSE|MW_SMFONT|MW_CLRDISP );
	DispLineMW( "COD DE EMPRESA", MW_LINE2, MW_LEFT );
	APM_SetCol(4); // Set Columna for TextBox
	memset(kbdbuff, 0x00, sizeof(kbdbuff));

	if( !APM_GetKbd(NUMERIC_INPUT + MW_SMFONT + MW_LINE4, IMIN(1) + IMAX(10), kbdbuff) ){
		//APM_ResetComm();
		return FALSE;
	}

	memset( INPUT.sb_cod_empresa, 0x00, sizeof(INPUT.sb_cod_empresa) );
	memcpy( INPUT.sb_cod_empresa, &kbdbuff[1], kbdbuff[0] );
	memcpy( tmpEmpresa, &kbdbuff[1], kbdbuff[0] );
	LSetStr( INPUT.sb_cod_empresa, 10, '0' );

	memset( kbdbuff, 0x00, sizeof(kbdbuff) );
	DispLineMW( "COD. ESTABLECIMIENTO", MW_LINE6, MW_CENTER );

	if( !APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SMFONT + MW_LINE7, IMIN(1) + IMAX(20), kbdbuff) ){
		//APM_ResetComm();
		return FALSE;
	}

	memset( INPUT.sb_contrapar, 0x00, sizeof(INPUT.sb_contrapar) );
	memcpy( INPUT.sb_contrapar, &kbdbuff[1], kbdbuff[0] );
	memcpy( tmpEstablecimiento, &kbdbuff[1], kbdbuff[0] );
	RSetStr( INPUT.sb_contrapar, 34, ' ' );

	DispLineMW( "VALOR:", MW_LINE9, MW_LEFT );
	if( !GetAmnt(MW_LINE9) ){
		//APM_ResetComm();
		return FALSE;
	}

	if( !confirmarCompraStock(tmpEmpresa, tmpEstablecimiento) ){
		//APM_ResetComm();
		return FALSE;
	}
	return TRUE;
}


void compraDeStock(void){	// Esta transaccion no hace una consulta previa
	int rta;
	int select;

	if( setCompraDeStock() == TRUE){
		PackComm(TRUE, FALSE);
		APM_PreConnect();

		select = MenuRecaudaciones();	// se elige si se desea pagar en efectivo o con debito a cuenta

		if (select == -1)
		  return;

		if(select == 2){	// Pago con debito a cuenta
			if( !procesoRecaudoDebitoCuenta("COMPRA DE STOCK") ){
				APM_ResetComm();
				return;
			}
			else{
				rta = compraDeStockTrans(FALSE);
				switch(rta){
				case 1:
					ConfirmarTransaccion(CONF_COMPRA_STOCK);	//Confirmar compra de stock
					break;

				case 2:
					if( compraDeStockTrans(TRUE) ){	// Multiples cuentas
						ConfirmarTransaccion(CONF_COMPRA_STOCK);	//Confirmar compra de stock
					}
					break;

				default:
					break;
				}
			}
		}
		else	// Pago en efectivo
		{
			if( !procesoRecaudoEfectivo("COMPRA DE STOCK") ){
				APM_ResetComm();
				return;
			}
			else{
				rta = compraDeStockTrans(FALSE);
				if(rta == TRUE){
					ConfirmarTransaccion(CONF_COMPRA_STOCK);	//Confirmar compra de stock
				}
			}
		}

	}
}




void consultaDeSaldo(BYTE aOpcion){
	int select = -1;
	BOOLEAN salir = FALSE;

	while(salir == FALSE)
	{
		select = MenuSelect(&KConsultaMenu_2, select);
		if (select == -1)
			return;

		switch (select)
		{
		case 1:
			strcpy(INPUT.sb_typeAccount, "01");
			salir = TRUE;
			break;

		case 2:
			strcpy(INPUT.sb_typeAccount, "02");
			salir = TRUE;
			break;

		case 3:
			strcpy(INPUT.sb_typeAccount, "03");
			salir = TRUE;
			break;

		default:
			break;
		}
	}
	PackComm(TRUE, FALSE);
	APM_PreConnect();
	if( !procesoConsultaSaldoImp() ){
		APM_ResetComm();
		return;
	}
	if( consultaTrans(aOpcion, FALSE) == 2 ) // Si el switch devuelve multiples cuentas
		consultaTrans(aOpcion, TRUE);
}


void consultaDeSaldoCNB(BYTE aOpcion){
  int select = -1;
  BOOLEAN salir = FALSE;

  /*while(salir == FALSE)
  {
    select = MenuSelect(&KConsultaMenu_2, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
      strcpy(INPUT.sb_typeAccount, "01");
      salir = TRUE;
      break;

    case 2:
      strcpy(INPUT.sb_typeAccount, "02");
      salir = TRUE;
      break;

    case 3:
      strcpy(INPUT.sb_typeAccount, "03");
      salir = TRUE;
      break;

    default:
      break;
    }
  }*/
  PackComm(TRUE, FALSE);
  APM_PreConnect();
  if( !procesoConsultaSaldoCNB() ){
    APM_ResetComm();
    return;
  }
  if( consultaTrans(aOpcion, FALSE) == 2 ) // Si el switch devuelve multiples cuentas
    consultaTrans(aOpcion, TRUE);
}





void consultaImpresa(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KConsultaMenu, select);
    if (select == -1)
      return;

    switch (select)
    {
      case 1:
    	  if( consultaComisionTrans(1)  == TRUE){
    		  consultaDeSaldo(1);
    	  }
        break;

      case 2:
    	  if( consultaComisionTrans(2) == TRUE){
    		  consultaDeSaldo(2);
    	  }
        break;

      default:
    	  break;
    }
  }
}




void MenuConsultasDelCnb(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KMenuConsultasCnb, select);
    if (select == -1)
      return;

    switch (select)
    {
    case 1:
    	consultaDeSaldoCNB(3);
    	break;

    case 2:

    	break;

    default:
    	break;
    }
  }
}
