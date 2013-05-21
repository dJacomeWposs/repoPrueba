/*
 * CNBAdmin.c
 *
 *  Created on: Jul 10, 2012
 *      Author: danijacm
 */


#include <string.h>
//#include <stdio.h>
//#include "util.h"
//#include "sysutil.h"
#include "menu.h"
#include "input.h"
#include "CNBAdmin.h"
#include "corevar.h"
#include "IPCfg.h"
#include "Corevar.h"
#include "keytrans.h"
#include "tranutil.h"
#include "CNBTrans.h"
#include "Print.h"
#include "FUNC.H"
//============ ADMINISTRATIVO ===============
static const struct MENU_ITEM KAdminItem [] =
{
  {  1, "Comprobante"},
  {  2, "Informe"},
  {  3, "Configuracion"},
  {  4, "Modo Conexion"},
  {  5, "Cargar MK"},
  {  6, "Otros"},
  {-1, NULL},
};

static const struct MENU_DAT KAdminMenu  = {
  "ADMINISTRATIVO",
  KAdminItem,
};

//============ COMPROBANTE ===============
static const struct MENU_ITEM KComprobanteItem [] =
{
  {  1, "Emision ultimo"},
  {  2, "Consulta cupon"},
  {-1, NULL},
};

static const struct MENU_DAT KComprobanteMenu  = {
  "COMPROBANTE",
  KComprobanteItem,
};
//-------------- CONSULTA CUPON -------------------
static const struct MENU_ITEM KConsultaCuponItem [] =
{
  {  1, "Ultimo Cupon"},
  {  2, "Definir NSU"},
  {-1, NULL},
};

static const struct MENU_DAT KConsultaCuponMenu  = {
  "CONSULTA CUPON",
  KConsultaCuponItem,
};

//============ INFORME ===============
static const struct MENU_ITEM KInformeItem [] =
{
  {  1, "Transacciones"},
  {  2, "Diario de Trans"},
  {  3, "Limite de Operc."},
  {  4, "Venta de productos"},
  {-1, NULL},
};

static const struct MENU_DAT KInformeMenu  = {
  "INFORME",
  KInformeItem,
};

//============ CONFIGURACION ===============
static const struct MENU_ITEM KConfigItem [] =
{
  {  1, "Operador"},
  {  2, "Terminal"},
  {  3, "Comunicacion"},
  {  4, "Pruebas"},
  {-1, NULL},
};

static const struct MENU_DAT KConfigMenu  = {
  "CONFIGURACION",
  KConfigItem,
};
//--------------- OPERADOR -------------------
static const struct MENU_ITEM KOperadorItem [] =
{
  {  1, "Administrador"},
  {  2, "Operador 1"},
  {  3, "Operador 2"},
  {-1, NULL},
};

static const struct MENU_DAT KOperadorMenu  = {
  "OPERADOR",
  KOperadorItem,
};
//--------------- TERMINAL -------------------
static const struct MENU_ITEM KTerminalItem [] =
{
  {  1, "Fecha y Hora"},
  {  2, "Localizacion"},
  {  3, "Identificacion"},
  {  4, "Impresion"},
  {  5, "Impresora"},
  {  6, "Tiempos"},
  {-1, NULL},
};

static const struct MENU_DAT KTerminalMenu  = {
  "TERMINAL",
  KTerminalItem,
};
//--------------- TIEMPOS -------------------
static const struct MENU_ITEM KTiemposItem [] =
{
  {  1, "Tiempo Logo"},
  {  2, "Tiempo Form"},
  {-1, NULL},
};

static const struct MENU_DAT KTiemposMenu  = {
  "TIEMPOS",
  KTiemposItem,
};

//============ COMUNICACION ===============
static const struct MENU_ITEM KCommItem [] =
{
  {  1, "Red"},
  {  2, "Protocolo"},
  //{  3, "Tipo De Comunicacion"},
  {-1, NULL},
};

static const struct MENU_DAT KCommMenu  = {
  "COMUNICACION",
  KCommItem,
};
//--------------- RED -------------------
static const struct MENU_ITEM KRedItem [] =
{
  {  1, "GPRS"},
  {  2, "ETHERNET"},
  {-1, NULL},
};

static const struct MENU_DAT KRedMenu  = {
  "RED",
  KRedItem,
};
//--------------- GPRS -------------------
static const struct MENU_ITEM KGprsItem [] =
{
  {  1, "OPERADORA 1"},
  {  2, "OPERADORA 2"},
  {  3, "PERSONALIZADO"},
  {-1, NULL},
};

static const struct MENU_DAT KGprsMenu  = {
  "GPRS",
  KGprsItem,
};


static const struct MENU_ITEM KGprsItem2 [] =
{
  {  1, "PRIMARIO"},
  {  2, "SECUNDARIO"},
  {  3, "PERSONALIZADO"},
  {-1, NULL},
};

static const struct MENU_DAT KGprsMenu2  = {
  "GPRS",
  KGprsItem2,
};

//--------------- ETHERNET -------------------
static const struct MENU_ITEM KEthernetItem [] =
{
  {  1, "CONFIGURACION"},
  {  2, "SERVIDOR"},
  {-1, NULL},
};

static const struct MENU_DAT KEthernetMenu  = {
  "ETHERNET",
  KEthernetItem,
};
//----------- CONFIGURACION ETHERNET-----------
static const struct MENU_ITEM KConfigEthItem [] =
{
  {  1, "IP FIJA"},
  {  2, "DHCP"},
  {-1, NULL},
};

static const struct MENU_DAT KConfigEthMenu  = {
  "CONFIGURACION",
  KConfigEthItem,
};
//--------------- SERVER ETHERNET ---------------
static const struct MENU_ITEM KServerEthItem [] =
{
  {  1, "Default"},
  {  2, "Personalizado"},
  {-1, NULL},
};

static const struct MENU_DAT KServerEthMenu  = {
  "TIPO SERVIDOR",
  KServerEthItem,
};
//--------------- SrvEth -------------------
static const struct MENU_ITEM KSrvEthItem [] =
{
  {  1, "PRIMARIO"},
  {  2, "SECUNDARIO"},
  {-1, NULL},
};

static const struct MENU_DAT KSrvEthMenu  = {
  "SERVIDOR",
  KSrvEthItem,
};

//--------------- SERVER -------------------
static const struct MENU_ITEM KServerItem [] =
{
  {  1, "PRIMARIO"},
  {  2, "SECUNDARIO"},
  {-1, NULL},
};

static const struct MENU_DAT KServerMenu  = {
  "SERVIDOR",
  KServerItem,
};
///----------------------otros ----------------------
static const struct MENU_ITEM OtrosItem [] =
{
  {  1, "VERSIONES"},
  {  2, "PARAMETROS"},
  {-1, NULL},
};

static const struct MENU_DAT OtroMenu  = {
  "OTROS",
  OtrosItem,
};

//============ PRUEBAS ===============
static const struct MENU_ITEM KPruebasItem [] =
{
  {  1, "Activacion"},
  {  2, "Instalacion"},
  {  3, "Mantenimiento"},
  {-1, NULL},
};

static const struct MENU_DAT KPruebasMenu  = {
  "PRUEBAS",
  KPruebasItem,
};

//============ MODO DE CONEXION ===============
static const struct MENU_ITEM KCommModeItem [] =
{
  {  1, "SERVER 1"},
  {  2, "SERVER 2"},
  {-1, NULL},
};

static const struct MENU_DAT KCommModeMenu  = {
  "MODO DE CONEXION",
  KCommModeItem,
};

//============ SESION ===============
static const struct MENU_ITEM KSessionItem [] =
{
  {  1, "Conecta"},
  {  2, "Desconecta"},
  {  3, "Evalua protocolo"},
  {  4, "Evalua red"},
  {  5, "Evalua llamada"},
  {-1, NULL},
};

static const struct MENU_DAT KSessionMenu  = {
  "SESION",
  KSessionItem,
};

//============ OTROS ===============
static const struct MENU_ITEM KOthersItem [] =
{
  {  1, "Versiones"},
  {  2, "Lista de Antenas"},
  {-1, NULL},
};

static const struct MENU_DAT KOthersMenu  = {
  "ADMINISTRATIVO",
  KOthersItem,
};


//=========INFORMES=================


static const struct MENU_ITEM KInformesItem [] =
{
  {  1, "Transacciones"},
  {  2, "Diario de trans"},
  {  3, "Limite de operacion"},
  {  4, "Venta de productos"},
  //{  5, "Reimpresion documento"},
  {-1, NULL},
};

static const struct MENU_DAT KInformesMenu  = {
  "INFORMES",
  KInformesItem,
};



//============ OPERADORES ===============
static const struct MENU_ITEM KAdminOpr [] =
{
  {  1, "Administrador"},
  {  2, "Operador 1"},
  {  3, "Operador 2"},
  {  4, "Operador 3"},
  {  5, "Operador 4"},
  {  6, "Operador 5"},
  {-1, NULL},
};

static const struct MENU_DAT KAdminOperador  = {
  "OPERADOR",
  KAdminOpr,
};



BOOLEAN MenuComprobante(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KComprobanteMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  ReprintLast();
        break;

      case 2:
    	  ConsultaComprobanteTrans();
        break;
    }
  }
  return TRUE;
}





BOOLEAN MenuInformes(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KInformesMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
	      //PrintTransTest2();
		  PrintTrans();
		 // APM_WaitKey(3000,0);
        break;

      case 2:
    	  CoberturaLoteTrans();
        break;

      case 3:
    	  PackLimoper();
        break;

	  case 4:
    	  resumenVentaProd();//Nuevo
        break;

	  /*case 5:
    	  reimpresionDoc();
        break;*/
    }
  }
  return TRUE;
}





BOOLEAN AdminMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KAdminMenu, select);
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
        MenuComprobante();
        break;

      case 2:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        MenuInformes();
        break;

      case 3:
	   if( gUsers_tbl.b_type == KOPERADOR )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }

        ConfigMenu();
        break;

      case 4:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
		 if( gUsers_tbl.b_type == KOPERADOR )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }

        SrvEthMenu();
        break;

      case 5:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
		 if( gUsers_tbl.b_type == KOPERADOR )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }

        IccTmkInject();
        break;

      case 6:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        OtrosMenu();
        break;
    }
  }
  return TRUE;
}





BOOLEAN ConfigMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KConfigMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  OperadorMenu();
        break;
      case 2:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        //TerminalMenu();
        break;
      case 3:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        CommMenu();
        break;
      case 4:
        if( gUsers_tbl.b_type == KESPECIALISTA )
        {
          MsgError("ACCESO DENEGADO");
          break;
        }
        //PruebasMenu();
        break;
    }
  }
  return TRUE;
}



BOOLEAN CommMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KCommMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  GprsMenu();//RedMenu();
        break;
      case 2:
        break;
      /*case 3:
		RedMenu2();
        break;*/
    }
  }
  return TRUE;
}


BOOLEAN RedMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KRedMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        //GprsMenu();
        break;
      case 2:
        //EthernetMenu();
        break;
    }
  }
  return TRUE;
}



BOOLEAN RedMenu2(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KRedMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
      	setCommMode(APM_COMM_GPRS);
        break;

      case 2:
		setCommMode(APM_COMM_TCPIP);
        break;
    }
  }
  return TRUE;
}



BOOLEAN GprsMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KGprsMenu2, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        ModoConexion(TRUE);
        break;
      case 2:
    	  ModoConexion(FALSE);
        break;
      case 3:
    	  ConexPerson();
        break;
    }
  }
  return TRUE;
}

BOOLEAN ServerMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KServerMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  ModoConexion(TRUE);
        break;
      case 2:
    	  ModoConexion(FALSE);
        break;
    }
  }
  return TRUE;
}



BOOLEAN EthernetMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KEthernetMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        ConfigEthMenu();
        break;
      case 2:
        ServerEthMenu();
        break;
    }
  }
  return TRUE;
}

BOOLEAN ConfigEthMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KConfigEthMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        ConfigTermIP( );
        break;
      case 2:
        ToggleDHCP( );
        break;
    }
  }
  return TRUE;
}



BOOLEAN ServerEthMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KServerEthMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
        SrvEthMenu();
        break;
      case 2:
        // Configuracion de Servidor - Perzonalizado
        Server1Cfg();
        Server2Cfg();
        break;
    }
  }
  return TRUE;
}

BOOLEAN SrvEthMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&KSrvEthMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  ModoConexion(TRUE);
        break;
      case 2:
    	  ModoConexion(FALSE);
        break;
    }
  }
  return TRUE;
}

BOOLEAN OtrosMenu(void)
{
  int select = 0;

  while(TRUE)
  {
    select = MenuSelect(&OtroMenu, select);
    if (select == -1)
      return FALSE;

    switch (select)
    {
      case 1:
    	  verVersionSyspack();
        break;
      case 2:
		  
    	 printParam();
        break;
    }
  }
  return TRUE;
}



BOOLEAN OperadorMenu (void){

	int select = 0;

	while(TRUE)
	{
		select = MenuSelect(&KAdminOperador, select);
		if (select == -1)
			return FALSE;

		switch (select)
		{
		case 1:
			ConfigOpr("ADMINISTRADOR",select);
			break;

		case 2:
			ConfigOpr("OPERADOR 1",select);
			break;

		case 3:
			ConfigOpr("OPERADOR 2",select);
			break;

		case 4:
			ConfigOpr("OPERADOR 3",select);
			break;

		case 5:
			ConfigOpr("OPERADOR 4",select);
			break;
		case 6:
			ConfigOpr("OPERADOR 5",select);
			break;

		}
	}
	return TRUE;

}
