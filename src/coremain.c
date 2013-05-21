//-----------------------------------------------------------------------------
//  File          : coremain.c
//  Module        :
//  Description   : Main Entry.
//  Author        : Auto Generate by coremain_c.bat
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//
//  Date          Who         Action
//  ------------  ----------- -------------------------------------------------
//  DD MMM  YYYY  coremain_c  Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "ecrdll.h"
#include "sysutil.h"
#include "hardware.h"
#include "apm.h"
#include "corevar.h"
#include "constant.h"
#include "FUNC.H"
#include "others.h"
#include "message.h"
#include "emvtrans.h"
#include "input.h"
#include "chkoptn.h"
#include "sale.h"
#include "auth.h"
#include "print.h"
#include "tranutil.h"
#include "ecr.h"
//#include "testcfg.h"
#include "coremain.h"
#include "CNBTrans.h"
#include "CNBmain.h"
#include "keytrans.h"
#include "lptutil.h"
#include "func.h"
#include "download.h"
#include "util.h"
#include "tokensUnpack.h"
//#include "LCD_LOGO.h"
//#include "SPGLib.h"
//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define RETURN_MASK         0xF0000000
#define RETURN_2_APM        0x80000000
//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------
const BYTE KSoftName[] = {"CNB_PICH"};        // Application Name

//-----------------------------------------------------------------------------
//      Export Functions
//-----------------------------------------------------------------------------
//void XF_ExportTest(void)                 { os_appcall_return(ExportTest()); }

const DWORD KExportFunc[] =    // Export Function Table
{
  (DWORD) 0,                   // Importance!! Number of export functions!!!
  //(DWORD) 1,                   // Importance!! Number of export functions!!!
  //(DWORD) XF_ExportTest,       // 01
  (DWORD) NULL,
};

//*****************************************************************************
//  Function        : VirginInit
//  Description     : Perform virgin initialization.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int VirginInit(void)
{
  struct MW_FILE_HDR file_hdr;
  int    found;
  struct TERM_DATA    term_data;
  struct ACQUIRER_TBL acq_tbl;
  int    max_acq, i;
  DispLineMW("EDC INIT.", MW_LINE3, MW_CLRDISP|MW_CENTER|MW_BIGFONT);
  DispLineMW("PLEASE WAIT...", MW_LINE5, MW_CENTER|MW_BIGFONT);

  // Clear Up all old files
  found = fDSearchMW(TRUE, &file_hdr);
  while (found != -1) {
    fDeleteMW(file_hdr.sb_name);
    found = fDSearchMW(TRUE, &file_hdr);
  }

  //// Clean old batch record if trans_mode
  APM_GetTermData(&term_data);
  if (term_data.b_stis_mode==TRANS_MODE) {
    max_acq = APM_GetAcqCount();
    for (i = 0; i < max_acq; i++) {
      if (!APM_GetAcqTbl(i, &acq_tbl))
        break;
      if (CorrectHost(acq_tbl.b_host_type)) {
        APM_SetPending(i, NO_PENDING);
        APM_BatchClear(i);
      }
    }
  }
  fCommitAllMW();
  return 0;
}
//*****************************************************************************
//  Function        : GlobalsInit
//  Description     : Initialization global variables.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GlobalsInit(void)
{
  // Init Global Memory
  MallocGDS();
  APM_GetTermCfg(&STIS_TERM_CFG);
  APM_GetTermData(&STIS_TERM_DATA);

  // Check All files
  //Disp2x16Msg("CHECK EDC FILES! PLEASE WAIT... ", MW_LINE3, MW_CLRDISP|MW_BIGFONT);
  DispLineMW("INICIANDO", MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
  DispLineMW("SISTEMA", MW_LINE5, MW_CENTER|MW_SMFONT);
  DataFileInit();
  JournalFileInit();
  Msg60FileInit();
  RevTableInit();
  TCPIPInit();
  UsersInit();
  InitAllHw();
  KeysInit();		//NOHORA 10Dic
  FTPInit();		//OJO NOHORA 12 - MAR 2013//

  //ExtPPadSetup();  // Set EMVDLL to use external PINPAD
}
//*****************************************************************************
//  Function        : GlobalsCleanUp
//  Description     : Clean Up all allocated globals.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GlobalsCleanUp(void)
{
  FreeGDS();
  CloseAllHw();
  DataFileClose();
}
//*****************************************************************************
//  Function        : SetupModeProcess
//  Description     : Process for SETUP/INIT MODE.
//  Input           : N/A
//  Return          : 0;        // normal return
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*static int SetupModeProcess(void)
{
  static const char *KModeText[4] = {
    "SETUP MODE",
    "INIT MODE ",
    "TRANS MODE",
    "KEYS SYNC "
  };
  int ret;

  if (gGDS->b_disp_chgd) {
    gGDS->b_disp_chgd = 0;
    DispLineMW("EDC:", MW_LINE1, MW_CLRDISP|MW_BIGFONT);
    DispLineMW(KModeText[STIS_TERM_DATA.b_stis_mode], MW_LINE1+6, MW_BIGFONT);
    DispLineMW(KSoftName, MW_LINE7, MW_BIGFONT);
  }

  ret = 0;
  APM_SetKbdTimeout(KBD_TIMEOUT);
  switch (APM_GetKeyin()) {
    #if (PR608D|TIRO|PR608)
    case MWKEY_UP:
    case MWKEY_DN:
    case MWKEY_LEFT:
    case MWKEY_RIGHT:
    case MWKEY_SELECT:
    #elif (R700|T800)
    case MWKEY_LEFT1:
    case MWKEY_FUNC:
    case MWKEY_FUNC1:
    #endif
    #if (T1000)
    case MWKEY_LEFT:
    #endif
      MerchantFunc(-1);
      break;
    case MWKEY_CANCL:
      RefreshDispAfter(0);
      break;
    case MWKEY_ENTER:
    case MWKEY_LEFT3:
    case MWKEY_FUNC4:
    case MWKEY_POWER:
      ret = APM_SelApp(MY_APPL_ID);
      if ((ret != -1) && (ret != MY_APPL_ID))
        ret |= RETURN_2_APM;
      RefreshDispAfter(0);
  }
  return(ret);
}*/
//*****************************************************************************
//  Function        : DispDTG
//  Description     : Show data/time or training mode status on idle menu.
//  Input           : aOn;        // TRUE => show ':'
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void DispDTG(DWORD aOn)
{
  static BYTE hdr_cnt;
  struct DATETIME dtg;
  BYTE  tmp[MW_MAX_LINESIZE+1];

  TimerSetMW(gTimerHdl[TIMER_MAIN],(TrainingModeON()?100:50));

  memset(tmp, 0, sizeof(tmp));
  if ((hdr_cnt > 6) && (TrainingModeON()))
    strcpy(tmp, "*TRAINING  MODE*");
  else {
    ReadRTC(&dtg);
    ConvDateTime(tmp, &dtg, 1);
    memmove(&tmp[3], &tmp[4], 14);
    memmove(&tmp[6], &tmp[7], 10);
    if (aOn == FALSE) {
      if (hdr_cnt % 2)
        tmp[13] = ' ';
    }
  }

  tmp[16] = 0;
  //DispLineMW(tmp, MW_LINE1, MW_REVERSE|MW_CENTER|/*MW_SPFONT*/MW_BIGFONT);

  if (++hdr_cnt > 12)
    hdr_cnt = 0;
}
//*****************************************************************************
//  Function        : TransModeDisp
//  Description     : Handle idle menu for TRANS MODE.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*static void TransModeDisp(void)
{
  if (TimerGetMW(gTimerHdl[TIMER_DISP]) == 0) {
    if (gGDS->b_disp_chgd) {
      DispCtrlMW(MW_CLR_DISP);
      DispDTG(FALSE);
      if (ICCReaderRdy()) {
        Disp2x16Msg(GetConstMsg(EDC_CO_SWIPE_INS), MW_LINE5, MW_BIGFONT);
      }
      else
        Disp2x16Msg(GetConstMsg(EDC_CO_REMOVE_ICC), MW_LINE5, MW_BIGFONT);
      gGDS->b_disp_chgd = FALSE;
    }
    if (TimerGetMW(gTimerHdl[TIMER_MAIN]) == 0)
      DispDTG(FALSE);
  }
}*/


static void TransModeDisp(void)
{
  //struct POINT point1, point2;

	BYTE PtoTele[5 + 1];	//OJO NOHORA 11 - MAR 2013//

  if (TimerGetMW(gTimerHdl[TIMER_DISP]) == 0) {
    if (gGDS->b_disp_chgd) {
      DispCtrlMW(MW_CLR_DISP);
      //DispDTG(FALSE);


      if (ICCReaderRdy()) {
        //Disp2x16Msg(GetConstMsg(EDC_CO_SWIPE_INS), MW_LINE5, MW_BIGFONT);

          /*GIFLoadMem( logoPichincha, sizeof(logoPichincha));
          GIFPlayOn( 0, 0 );
          GIFPlay();
          GIFStop(0);
          GIFUnLoad();*/
        //LogoCNB();
    	  showLogo();
    	  if( checkMessageInit() == TRUE){	// Se debe enviar el mensaje inicial automaticamente
    		  gTerminalInit = FALSE;
    		  InitMsg(FALSE);
    	  }
    	  if(checkTeleCarga()== TRUE)	 /// telecarga 1	//OJO NOHORA 11 - MAR 2013//
    	  {
    		  flagFtpManual = FALSE;
    		  memset(PtoTele, 0x00, sizeof(PtoTele));
    		  memcpy(PtoTele, gToken_96.puertoTeleCarga, 5);
    		  //printf("\fentro TELECARGA3\nport: |%d|", atoi(PtoTele)); WaitKey(3000, 0);
    		  if( atoi(PtoTele) == 21){
    			  DownloadFtp();
    		  	  ResetMW();
    		  }
    		  else
    			  setParamRemoteDownload();
    	  }

/*#if(NEW_MIDWARE == TRUE)
        CPutCharMW(MW_LPUSH_CURSOR);
#else
		DispPutCMW(K_PushCursor);
#endif
        os_disp_textc(COLOR_YELLOW);
        DispLineMW( "BANCO PICHINCHA", MW_LINE5, MW_CENTER|MW_BIGFONT );
#if(NEW_MIDWARE == TRUE)
        CPutCharMW(MW_LPOP_CURSOR);
#else
		DispPutCMW(K_PopCursor);
#endif*/

/*
    	  os_disp_textc( COLOR_WHITE );
    	  DispLineMW(GetConstMsg(EDC_CO_SWIPE_INS), MW_LINE9, MW_SPFONT| MW_CENTER);
    	  os_disp_textc( COLOR_BLACK );
*/
      }
      else
        Disp2x16Msg(GetConstMsg(EDC_CO_REMOVE_ICC), MW_LINE7, MW_BIGFONT);
      gGDS->b_disp_chgd = FALSE;

    }
    if (TimerGetMW(gTimerHdl[TIMER_MAIN]) == 0)
      DispDTG(FALSE);
  }
}

//*****************************************************************************
//  Function        : TransModeProcess
//  Description     : Process for TRANS MODE.
//  Input           : N/A
//  Return          : 0;     // Normal return;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int TransModeProcess(void)
{
	DWORD keyin;
	int  ret;
	BYTE *pMsgInit;
	WORD cntTok = 0;
	int indice = 0;
	int len;


	// Show mesg Init
	if( gShowMsgInit == 1 )
	{
		ClearDispMW();
#if(NEW_MIDWARE == TRUE)
		CPutCharMW(MW_LPUSH_CURSOR);
#else
		DispPutCMW(K_PushCursor);
#endif
		os_disp_backc(COLOR_YELLOW);	// COLOR_YELLOW
		os_disp_textc(COLOR_BLACK);
		//pMsgInit = gMsgInit;
		len = strlen(gMsgInit);

		while( indice < len )
		{
			pMsgInit = strtok( gMsgInit + indice, "@" );
			indice += strlen( pMsgInit ) + 1;
			switch(cntTok){
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
			cntTok++;
		}
		// END Show msg Init
		APM_WaitKey(3000,0);
		gShowMsgInit = 0;
#if(NEW_MIDWARE == TRUE)
		CPutCharMW(MW_LPOP_CURSOR);
#else
		DispPutCMW(K_PopCursor);
#endif
	}

	TransModeDisp();
	//printf("Salio del logo");
	keyin = APM_GetKeyin();

	if (keyin == 0)
		return 2;

	ret = 1;
	switch (keyin) {

	case MWKEY_0:
	case MWKEY_1:
	case MWKEY_2:
	case MWKEY_3:
	case MWKEY_4:
	case MWKEY_5:
	case MWKEY_6:
	case MWKEY_7:
	case MWKEY_8:
	case MWKEY_9:
	case MWKEY_ENTER:

		do
		{
			ret = GetPassword( );
			//printf("\fRET 1 = %d", ret);WaitKey(3000, 0);
			if( ret == -1 )
			{
				ret = 1;
				RefreshDispAfter(0);
				return ret;
				break;
			}
			else if( ret == TRUE )
				break;
		}while( ret == FALSE );

		MenuPrincipal();
		RefreshDispAfter(0);

		break;

#if (PR608D|TIRO|PR608)
	case MWKEY_UP:
	case MWKEY_DN:
	case MWKEY_LEFT:
	case MWKEY_RIGHT:
	case MWKEY_SELECT:
#elif (R700|T800)
	case MWKEY_LEFT1:
	case MWKEY_FUNC:
	case MWKEY_FUNC1:
#endif
#if (T1000)
	case MWKEY_LEFT:
#endif
		//MerchantFunc(-1);
		//RefreshDispAfter(0);
		//ResetTerm();
		break;
#if (PR608D|TIRO|PR608)
	case MWKEY_FUNC:
	case MWKEY_FUNC1:
#elif (T800)
	case MWKEY_UP:
	case MWKEY_DN:
	case MWKEY_LEFT:
	case MWKEY_RIGHT:
	case MWKEY_SELECT:
	case MWKEY_LEFT2:
#endif
		//OtherTrans();
		ResetTerm();
		break;
#if (T1000)
	case MWKEY_RIGHT:
		//OtherTrans();
		ResetTerm();
		break;
#endif

	case MWKEY_POWER:
		PowerOffRebootMenu();
		RefreshDispAfter(0);
		/*
	      ret = APM_SelApp(MY_APPL_ID);
	      if ((ret != -1) && (ret != MY_APPL_ID))
	        ret |= RETURN_2_APM;
	      RefreshDispAfter(0);
		 */
		break;
	case MWKEY_CANCL:
		RefreshDispAfter(0);
		ResetTerm();
		break;
	case MWKEY_LEFT4:
		ManualFeed();
		break;
#if (R700)
	case MWKEY_LEFT5:
#endif
	case MWKEY_FUNC5:
		ReprintLast();
		RefreshDispAfter(0);
		break;
	default :
		LongBeep();
		break;
	}
	return(ret);
}
//*****************************************************************************
//  Function        : Coremain
//  Description     : Your application code start here.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int Coremain(DWORD aParam1, DWORD aParam2, DWORD aParam3)
{
  //T_APP_INFO app_info;
  int ret = 0;
  //BYTE *bufferAux;
  /*BYTE wk2[16];

  memset(wk2, 0x00, sizeof(wk2));
  memcpy( wk2, "\xD8\x65\x77\xE7\x45\x18\x2B\x3A\x50\xF2\xCF\xA3\x61\x9F\xFA\x33", 16);*/
  /*bufferAux = (BYTE *)MallocMW(256);
  set_pptr(bufferAux,256);
  pack_mem( "\x05\x02\x11\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		  "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x00\x00\x04"
		  "\x32\x30\x30\x32\x30\x33\x33\x31\x20\x20\x31\x30\x30\x30\x31\x30\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
		  "\x30\x30\x30\x30\x30\x30\x30\x30\x32\x30\x32\x31\x32\x30\x31\x33\x30\x33\x32\x31\x20\x20"
		  "\x31\x30\x30\x30\x30\x31\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x30\x30\x30\x30\x30\x30\x30\x30\x31\x31\x32\x32", 130);

  getToken5();

  FreeMW(bufferAux);*/

  BYTE PtoTele[5 + 1];	//OJO NOHORA 11 - MAR 2013//
  
  RefreshDispAfter(0);
  ResetTerm();

  //extKeyBoardTest();
  /*ResetKey(MKEY_IDX);
  return 0;*/
  SetMasterKey();		//NOHORA 06Dic

  //recaudoRecargasTrans();
  //printf("\fTermino la vaina...");APM_WaitKey(3000, 0);

  /*memset( gtoken_97.worKeyPin , 0x00, sizeof( gtoken_97.worKeyPin ));
  memset( gtoken_97.worTrack2 , 0x00, sizeof( gtoken_97.worTrack2 ));

  memcpy( gtoken_97.worKeyPin, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x15", 16);
  memcpy(  gtoken_97.worTrack2, "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x10\x11\x12\x13\x14\x16", 16);

  LoadEWKPin( gtoken_97.worKeyPin );
  LoadEWKCard( gtoken_97.worTrack2 );

  SaveKeysFile(&gtoken_97);	//NOHORA 10Dic*/

  while (TRUE)
   {
     SleepMW();
     APM_SetKbdTimeout(KBD_TIMEOUT);

     if( (ret == 0))
     {
    	 ret = InitMsg(FALSE);

    	 if(checkTeleCarga()== TRUE)	 /// telecarga 1	//OJO NOHORA 11 - MAR 2013//
    	 {
    		 flagFtpManual = FALSE;
    		 memset(PtoTele, 0x00, sizeof(PtoTele));
    		 memcpy(PtoTele, gToken_96.puertoTeleCarga, 5);
    		 //printf("\fentro TELECARGA\nport: |%d|", atoi(PtoTele)); WaitKey(3000, 0);
    		 if( atoi(PtoTele) == 21){
    			 DownloadFtp();
    			 ResetMW();
    		 }
    		 else
    			 setParamRemoteDownload();
    	 }

    	 //ret = 1;
    	 RefreshDispAfter(0);
    	 gShowMsgInit = ret;
     }
     else if( (ret != -1) && (gTcpIp.sb_ip[0] != 0x00) )
     {
    	 /*memset(gToken_8.cupon, 0x00, sizeof(gToken_8.cupon));
    	 strcpy(gToken_8.cupon, "HOLA@MUNDO@PRUEBA DE IMPRSION@	");
    	 PackVoucher( FALSE );*/
    	 ret=TransModeProcess();
    	 //printf("\fSalio");WaitKey(3000, 0);
     }
     else
     {
 #if(NEW_MIDWARE == TRUE)
       CPutCharMW(MW_LPUSH_CURSOR);
 #else
 	  DispPutCMW(K_PushCursor);
 #endif
       os_disp_textc(COLOR_RED);
       DispLineMW("FALLA EN CONEXION",  MW_LINE4, MW_CLRDISP|MW_CENTER|MW_SMFONT);
 #if(NEW_MIDWARE == TRUE)
       CPutCharMW(MW_LPOP_CURSOR);
 #else
 	  DispPutCMW(K_PopCursor);
 #endif
       Delay1Sec( 3, TRUE );
       ret = TRUE;


       while( ret == TRUE )
       {
    	   if(checkTeleCarga()== TRUE)	 /// telecarga 1	//OJO NOHORA 11 - MAR 2013//
    	   {
    		   flagFtpManual = FALSE;
    		   memset(PtoTele, 0x00, sizeof(PtoTele));
    		   memcpy(PtoTele, gToken_96.puertoTeleCarga, 5);
    		   //printf("\fentro TELECARGA2\nport: |%d|", atoi(PtoTele)); WaitKey(3000, 0);
    		   if( atoi(PtoTele) == 21){
    			   DownloadFtp();
    			   ResetMW();
    		   }
    		   else
    			   setParamRemoteDownload();
    	   }

    	   ret = GetPassword( );
        // printf("\fRET 2 = %d", ret);WaitKey(3000, 0);
         if( ret == TRUE )
         {
           MenuPrincipal( );

           if( gTcpIp.sb_ip[0] != 0x00 && gTcpIp.w_port != 0)
             ret = 0;
         }
         else
         {
           if( gTcpIp.sb_ip[0] != 0x00 && gTcpIp.w_port != 0)
             ret = 0;
         }
       }
     }

     if ((ret & RETURN_MASK) == RETURN_2_APM)
       break;
   }
  DispLineMW("RETURN TO APM", MW_LINE1, MW_CLRDISP|MW_CENTER|MW_REVERSE|MW_BIGFONT);
  Disp2x16Msg(GetConstMsg(EDC_CO_PROCESSING), MW_LINE5, MW_BIGFONT);
  return(ret&(~RETURN_2_APM));
}
