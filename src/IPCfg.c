//-----------------------------------------------------------------------------
//  File          : ipcfg.c
//  Module        :
//  Description   : Terminal IP management.
//  Author        : Lewis
//  Notes         :
//
//  Naming conventions
//  ~~~~~~~~~~~~~~~~~~
//             Constant : Leading K
//      Global Variable : Leading g
//    Function argument : Leading a
//       Local Variable : All lower case
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  13 May  2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "basecall.h"
#include "midware.h"
#include "hwdef.h"
#include "sysutil.h"
#include "corevar.h"
#include "toggle.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//    Globals
//-----------------------------------------------------------------------------
struct MW_TCP_PORT gTcpPort;
static int    gTcpHandle=-1;
//-----------------------------------------------------------------------------
//    Constants
//-----------------------------------------------------------------------------

static const BYTE KIPSetup[]            = {"  TERM IP SETUP  "};
static const BYTE KMenu1[]              = {"Terminal IP     "};
static const BYTE KMenu2[]              = {"DHCP Setup      "};
static const BYTE KIpHdr[]              = {"IP SETUP"};
static const BYTE KDHCPHdr[]            = {"DHCP SETUP"};
static const BYTE KEnterSave[]          = {"Enter To Save!"};

//*****************************************************************************
//  Function        : DispTermIP
//  Description     : Display terminal ip, netmask, gateway ip.
//  Input           : aIP;      // pointer to ip info.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DispTermIP(BYTE *aIP)
{
  DispClrBelowMW(MW_LINE2);
  DispLineMW("IP Address", MW_LINE2, MW_SMFONT);
  ShowIp(aIP,MW_LINE3,FALSE);
  DispLineMW("Netmask", MW_LINE4, MW_SMFONT);
  ShowIp(&aIP[4],MW_LINE5,FALSE);
  DispLineMW("Gateway IP", MW_LINE6, MW_SMFONT);
  ShowIp(&aIP[8],MW_LINE7,FALSE);
}
//*****************************************************************************
//  Function        : ConfigTermIP
//  Description     : Setup ip, netmask, gateway ip
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ConfigTermIP(void)
{
  BYTE   ip[12];

#if (DOCK_SUPPORT)
  bs_config_read(K_CF_BS_IP,      &ip[0]);
  bs_config_read(K_CF_BS_Netmask, &ip[4]);
  bs_config_read(K_CF_BS_Gateway, &ip[8]);
#else
  GetSysCfgMW(MW_SYSCFG_IP, &ip[0]);
  GetSysCfgMW(MW_SYSCFG_NETMASK, &ip[4]);
  GetSysCfgMW(MW_SYSCFG_GATEWAY, &ip[8]);
#endif

  DispLineMW(KIpHdr, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP(ip);

  if (!EditIp(ip, MW_LINE3))
    return;

  if (!EditIp(&ip[4], MW_LINE5))
    return;

  if (!EditIp(&ip[8], MW_LINE7))
    return;

  DispLineMW(KEnterSave, MW_MAX_LINE, MW_REVERSE|MW_CENTER|MW_SMFONT);

  if (GetChar() != MWKEY_ENTER)
    return;

#if (DOCK_SUPPORT)
  bs_config_write(K_CF_BS_IP,      &ip[0], 4);
  bs_config_write(K_CF_BS_Netmask, &ip[4], 4);
  bs_config_write(K_CF_BS_Gateway, &ip[8], 4);
  bs_config_update();
  DispLineMW("Reboot BASE", MW_LINE5, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
  DispLineMW("PLEASE WAIT", MW_LINE7, MW_CENTER|MW_REVERSE|MW_BIGFONT);
  bs_reboot();
  // wait base ready.
  while (bs_loopback(ip, sizeof(ip), ip) == 0) {
    Delay1Sec(1, TRUE);
  }
#else
  PutSysCfgMW(MW_SYSCFG_IP, &ip[0]);
  PutSysCfgMW(MW_SYSCFG_NETMASK, &ip[4]);
  PutSysCfgMW(MW_SYSCFG_GATEWAY, &ip[8]);
  UpdSysCfgMW();
  Disp2x16Msg("  Reiniciar ?         (Y/N)     ", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  if (WaitKey(KBD_TIMEOUT, 0) == MWKEY_ENTER) {
      ResetMW();
  }

#endif
  //PromptReboot();
}
//*****************************************************************************
//  Function        : ToggleDHCP
//  Description     : Toggle DHCP setting
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void ToggleDHCP(void)
{
#if (LAN_SUPPORT)
  struct MW_NIF_INFO nif_info;
  BOOLEAN ret;
  BYTE cur_val, org_val;
  int  option;

  ret = FALSE;

  NetInfoMW(MW_NIF_ETHERNET, &nif_info);
  DispLineMW(KDHCPHdr, MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispTermIP((BYTE *)&nif_info);

  #if (DOCK_SUPPORT)
  bs_config_read(K_CF_BS_DHCPEnable, &org_val);
  #else
  GetSysCfgMW(MW_SYSCFG_DHCP_ENABLE, &org_val);
  #endif
  cur_val = org_val = (org_val != 0) ? 1 : 0;
  option = ToggleOption(NULL, KEnable, org_val|TOGGLE_MODE_8LINE);
  if (option == -1)
    return;

  cur_val = (BYTE) option;
  if (cur_val != org_val) {
    #if (DOCK_SUPPORT)
    bs_config_write(K_CF_BS_DHCPEnable, &org_val, 1);
    bs_config_update();
    #else
    PutSysCfgMW(MW_SYSCFG_DHCP_ENABLE, &cur_val);
    UpdSysCfgMW();
    #endif
    PromptReboot();
  }
#else // !LAN_SUPPORT
  return;
#endif //(LAN_SUPPORT)
}


void TcpCfg(void)
{
  //struct MW_TCP_PORT port;

  ClearDispMW();

  if (!SetIPAddr( 1, MW_LINE2))
    return;

  if (!SetPort( 1, MW_LINE4))
    return;
  /*
  if (!GetBufSize(MW_LINE6, MIN_TEST_SIZE, MAX_TEST_SIZE))
    return;


  if (!GetSSLKeyIdx(MW_LINE8, 0, 0xFF))
    return;
  */

  SaveTcpIp(&gTcpIp);

  //memcpy(&gTcpPort.d_ip, gIpAddr, 4);

  gTcpPort.w_port = gPort;

  //port.b_sslidx = gSSLKeyIdx;
  //if (port.b_sslidx != 0)
    //port.b_option = MW_TCP_SSL_MODE;
  //else
    //port.b_option = 0;

  gTcpPort.b_option = 0;

  /*
  if (GetPPPStatus() == MW_PPP_PHASE_READY) {
    int option = 1;

    DispClrBelowMW(MW_LINE2);
    option = ToggleOption("Use PPP Channel?", KTrueFalse, option);
    if (option == -1)
      return;
    if (option == 1) {
      port.b_option |= MW_TCP_PPP_MODE;
    }
  }
  */
  if (((gTcpPort.b_option & MW_TCP_PPP_MODE) == 0) && !LanCableInsertedMW()) {
    DispLineMW("Check Lan Cable!", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT);
    GetChar();
    return;
  }
  //LoopBack(&port, gTestSize);
}

//*****************************************************************************
//  Function        : ConnectTcpip
//  Description     : Send Connection command to TCPIP Port
//  Input           : aWaitReady;   // Wait for line ready.
//                    aNoCancel;    // TRUE => No Cancel Allow
//  Return          : '0'*256+'0';  // Connected
//                    others;       // Fail
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
WORD ConnectTcpip(BOOLEAN aWaitReady, BOOLEAN aNoCancel)
{
  DWORD status;
  BOOLEAN ret=TRUE;
  DWORD host_timeout;
  BYTE filename[32];
//  struct COMMPARAM  comm_param;
  struct MW_TCP_PORT port;

  DispClrBelowMW(MW_LINE3);
  DispLineMW("CONECTANDO...", MW_LINE4, MW_CENTER|MW_SMFONT);
  //Disp2x16Msg(KCommConnectWait, MW_LINE5, MW_CENTER|MW_BIGFONT);

  /*
  if ((gCommParam.sPPP.dDevice & 0x0F) != 0) {
    if (ConnectPPP(TRUE) != COMM_OK)
      return COMM_NO_LINE;
  }
  */
  // new make connection
  if (gTcpHandle < 0) {
    // open
    strcpy(filename, DEV_TCP1);
    gTcpHandle = OpenMW(filename, MW_RDWR);
    ret = (gTcpHandle >= 0)? TRUE : FALSE;
    // connect
    if (ret) {
      memcpy(&port, &gTcpPort, sizeof(struct MW_TCP_PORT));
      if (IOCtlMW(gTcpHandle, IO_TCP_CONNECT, &port) < 0) {
        ret = FALSE;
      }
    }
  }

  // wait for connection ready
  if (ret && aWaitReady) {
    host_timeout = 60;
    host_timeout *= TIMER_1SEC;
    TimerSetMW(gTimerHdl[TIMER_COMM], host_timeout);
    ret = FALSE;
    do {
      SleepMW();
      if (StatMW(gTcpHandle, MW_TCP_STATUS, NULL) == MW_TCP_STAT_CONNECTED) {
        ret = TRUE;
        break;
      }
      if (GetCharMW()==MWKEY_CANCL) {
        if (!aNoCancel)
          break;
        LongBeep();
      }
    } while (TimerGetMW(gTimerHdl[TIMER_COMM]) > 0);
  }

  // return
  if (ret) {
    return COMM_OK;
  }
  else
  {
      // Disconnect Line
      //DispLineMW("Disconnecting", MW_LINE7, MW_CENTER|MW_SPFONT);
      IOCtlMW(gTcpHandle, IO_TCP_DISCONNECT, NULL);
      do
      {
        SleepMW();
        //status = StatMW(gTcpHandle, MW_TCP_STATUS, gTcpPort);
        status = StatMW(gTcpHandle, MW_TCP_STATUS, NULL);
        //sprintf(tmp, "Status: %08X", status);
        //DispLineMW(tmp, MW_LINE8, MW_SMFONT);
      } while ((status != MW_TCP_STAT_CLOSING) && (status != MW_TCP_STAT_IDLE));
      CloseMW(gTcpHandle);
      gTcpHandle = -1;
    return COMM_NOT_CONNECT;
  }
}

//*****************************************************************************
//  Function        : ResetTcpip
//  Description     : Reset TCPIP Port.
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*
BOOLEAN ResetTcpip(void)
{
  DWORD status;

  if (gTcpHandle >= 0) {
    IOCtlMW(gTcpHandle, IO_TCP_DISCONNECT, NULL);
    do{
      status = StatMW(gTcpHandle, MW_TCP_STATUS, NULL);
      status &= 0xFF;
    } while ((status != MW_TCP_STAT_IDLE) && (status != MW_TCP_STAT_CLOSED));
    CloseMW(gTcpHandle);
    gTcpHandle = -1;
  }
  if (((gCommParam.sPPP.dDevice&0x0F) != 0) && !gCommParam.sPPP.bKeepAlive) {
    DisconnectPPP();
  }
  return TRUE;
}
*/

//*****************************************************************************
//  Function        : Server1Cfg
//  Description     : Set IP and Port Server Configuration (PRIMARIO)
//  Input           : void
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Server1Cfg()
{

  //ClearDispMW();

  DispLineMW("SERVIDOR", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispLineMW("PRIMARIO", MW_LINE3, MW_CENTER|MW_SMFONT);

  if (!SetIPAddr( 1, MW_LINE5))
    return;

  if (!SetPort( 1, MW_LINE7))
    return;

  SaveTcpIp(&gTcpIp);

  gTcpPort.b_option = 0;


  if (((gTcpPort.b_option & MW_TCP_PPP_MODE) == 0) && !LanCableInsertedMW()) {
    DispLineMW("Revise Cable LAN!", MW_LINE4, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT);
    GetChar();
    return;
  }

  return;

}

//*****************************************************************************
//  Function        : Server2Cfg
//  Description     : Set IP and Port Server Configuration (SECUNDARIO)
//  Input           : void
//  Return          : TRUE/FALSE;     // TRUE=>line reseted.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void Server2Cfg(void)
{
  //ClearDispMW();

  DispLineMW("SERVIDOR", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  DispLineMW("SECUNDARIO", MW_LINE3, MW_CENTER|MW_SMFONT);

  if (!SetIPAddr( 2, MW_LINE5))
    return;

  if (!SetPort( 2, MW_LINE7))
    return;

  SaveTcpIp(&gTcpIp);

  gTcpPort.b_option = 0;


  if (((gTcpPort.b_option & MW_TCP_PPP_MODE) == 0) && !LanCableInsertedMW()) {
    DispLineMW("Revise Cable LAN!", MW_LINE4, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SPFONT);
    GetChar();
    return;
  }

  return;

}

void GprsCfg( BYTE aPriOrSec )
{
  BYTE tmp[33];

  memset( tmp, 0, sizeof(tmp) );

  if( aPriOrSec == 1){
    DispLineMW("GPRS PRIMARIO", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  }
  else
  {
    DispLineMW("GPRS SCUNDARIO", MW_LINE1, MW_CLRDISP|MW_REVERSE|MW_CENTER|MW_SMFONT);
  }
  // APN
  DispLineMW("DOMINIO :", MW_LINE2, MW_LEFT|MW_SMFONT);
  if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_SPFONT+MW_LINE3, IMIN(1)+IMAX(32), tmp))
      return ;
  tmp[tmp[0]+1] = 0;

  if( aPriOrSec == 1)
    strcpy(gTcpIp.sb_APN, &tmp[1]);
  else
    strcpy(gTcpIp.sb_APN2, &tmp[1]);
  //sprintf(gPDPContext, "AT+CGDCONT=1,\"IP\",\"%s\"", gGPRSDat.scAPN);

  // Login UserName
  DispLineMW("USUARIO :", MW_LINE4, MW_LEFT|MW_SMFONT);
  if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_SMFONT+MW_LINE7+RIGHT_JUST, IMIN(0)+IMAX(16), tmp))
    return ;
  tmp[tmp[0]+1] = 0;

  if( aPriOrSec == 1)
    strcpy(gTcpIp.sb_UserName, &tmp[1]);
  else
    strcpy(gTcpIp.sb_UserName2, &tmp[1]);

  // Login Password
  DispLineMW("PASSWORD:", MW_LINE5, MW_LEFT|MW_SMFONT);
  if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_SMFONT+MW_LINE7+RIGHT_JUST, IMIN(0)+IMAX(16), tmp))
    return ;
  tmp[tmp[0]+1] = 0;

  if( aPriOrSec == 1)
    strcpy(gTcpIp.sb_Password, &tmp[1]);
  else
    strcpy(gTcpIp.sb_Password2, &tmp[1]);

  if( aPriOrSec == 1)
    Server1Cfg();
  else
    Server2Cfg();
}
