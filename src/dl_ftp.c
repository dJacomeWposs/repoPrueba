//-----------------------------------------------------------------------------
//  File          : dl_ftp.c
//  Module        :
//  Description   : App download from FTP.
//  Author        : Lewis
//  Notes         : N/A
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
//  15 May  2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "midware.h"
#include "basecall.h"
#include "corevar.h"
#include "sysutil.h"
#include "download.h"
//#include "global.h"
//#include "gprs.h"
#include "dl_ftp.h"
#include "tranutil.h"
#include "util.h"
//#include "definiciones.h"
//#include "gui.h"

//==========================================================================
// Defines
//==========================================================================
// in ms
#define TIMEOUT_RESP                15000
#define TIMEOUT_LIST                7000
#define TIMEOUT_STATUS              7000
#define TIMEOUT_SEND_BLOCK          30000
#define TIMEOUT_READ_BLOCK          180000

#define SIZE_BLOCK                  256

//char STR_TEST_DIR_NAME[256]  =   "./T1000/demo/";//"./appsSpectra/Version   \0";//"./home/spectra/appsSpectra/T800"// "./t800/"
char STR_TEST_DIR_NAME[256]  =   "./T1000/pichincha/";  //"./appsSpectra/Version   \0";//"./home/spectra/appsSpectra/T800"// "./t800/"
#define STR_LIST_FILE_NAME          "file.txt"

#define MAX_NUM_OF_FILES    8
#define SIZE_FILENAME       31
#define SIZE_LINE           128
#define NUM_OF_RETRY        3

// State
enum {
  S_IDLE,
  S_CHK_MODEL,
  S_CHK_START_OF_MODULE,
  S_RXING_MODULE,
  S_CHK_MODULE_BREAK,
  S_RXING_DATA,
  S_SUCCESS,
  S_FAILURE
};
struct TransBuffer{ //Buffers de transmisión de datos y longitudes de los datos enviados y recibidos
  BYTE *gRxBuf;
  BYTE *gTxBuf;
  DWORD Tlen; //Longitud de datos a transmitir
  DWORD Rlen; //Longitud de datos recibidos
};
struct TransBuffer TBuffers;

//==========================================================================
// Constants
//==========================================================================

//==========================================================================
// Variable
//==========================================================================
BYTE ipHost[4] = {174,142,211,57};
//BYTE gUsername[16] = "CNB";    /* JJJ */
//static unsigned char gUsername[16] = "1234";
//BYTE gPassword[16] = "Spectra2011";
static char gFileList[MAX_NUM_OF_FILES][SIZE_FILENAME + 1];
static int gNumOfFiles;
static char gLineBuf[SIZE_LINE + 1];

static DWORD gRxSpeed;
static int gFtpHandle;
struct MW_TCP_PORT port;



//*****************************************************************************
//  Function        : FreeRunMark
//  Description     : Get the current free run mark.
//  Input           : N/A
//  Return          : current mark.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD FreeRunMark(void)
{
  return FreeRunMW();
}
//*****************************************************************************
//  Function        : FreeRunElapsed
//  Description     : Calc the Time elapsed in ms start from mark.
//  Input           : aStartMark.
//  Return          : time in ms.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static DWORD FreeRunElapsed(DWORD aStartMark)
{
  DWORD curr_mark = FreeRunMW();

  return ((curr_mark - aStartMark) * 2);   // 5ms per tick.
}
//*****************************************************************************
//  Function        : GetNShowStatus
//  Description     : Get & show ftp status on last line.
//  Input           : aShow;            // TRUE => Show Status on LINE8.
//  Return          : ftp status;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD GetNShowStatus(BOOLEAN aShow)
{
  DWORD status;

  status = StatMW(gFtpHandle, MW_FTP_STATUS, NULL);
  //if (aShow)
    //PrintfMW("\x10\x1B\x09\x01Status: %04X\x05\x12", status);
  return status;
}
//*****************************************************************************
//  Function        : WaitStatus
//  Description     : Wait for FTP status;
//  Input           : aStatus;            // Status code.
//                    aTimeout;           // timeout in ms
//  Return          : 0/-1;               // -1=> Timeout
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int WaitStatus (int aStatus, int aTimeout)
{
  DWORD status;
  DWORD start_cnt;

  start_cnt = FreeRunMark();
  do {
    status = GetNShowStatus(FALSE);
    if ((status & 0x7F) == aStatus)
      return 0;
  } while (FreeRunElapsed(start_cnt) < aTimeout);
  return -1;
}
//*****************************************************************************
//  Function        : GetCmdResp
//  Description     : Get FTP command response.
//  Input           : N/A
//  Return          : response data len;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int GetCmdResp (void)
{
  int i;
  int ret;
  int len;
  TBuffers.gRxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
  memset(TBuffers.gRxBuf,0x00,COMM_BUFF_SIZE);
  len = IOCtlMW(gFtpHandle, IO_FTP_CMD_READ, TBuffers.gRxBuf);

  if (len > 0) {
    ret = 0;
    for (i = 0; i < 3; i ++) {
      if ((TBuffers.gRxBuf[i] < '0') || (TBuffers.gRxBuf[i] > '9'))
        break;
      ret *= 10;
      ret += (TBuffers.gRxBuf[i] - '0');
    }
    if (((TBuffers.gRxBuf[3] == '-') || (TBuffers.gRxBuf[3] == ' ')) && (i == 3))
    {
    	FreeMW(TBuffers.gRxBuf);
    	return ret;
    }
  }
  FreeMW(TBuffers.gRxBuf);
  return 0;
}
//*****************************************************************************
//  Function        : WaitCmdResp
//  Description     : Wait for FTP command response.
//  Input           : aTimeout;         // timeout in 10ms
//  Return          : -1;               // -1=> Timeout
//                    others;           // command response code
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int WaitCmdResp (int aTimeout)
{
  int ret;
  DWORD start_mark;

  start_mark = FreeRunMark();
  do {
    ret = GetCmdResp();
    if (ret > 0)
      return ret;
  } while (FreeRunElapsed(start_mark) < aTimeout);

  return -1;
}
//*****************************************************************************
//  Function        : CloseaAll
//  Description     : Close All FTP channel.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static void CloseAll(void)
{
  DWORD status, start_mark;
  DWORD which;

  // Close Command Channel
  which = MW_FTP_CMD_CHNL;
  IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);
  status = GetNShowStatus(FALSE);

  // Close Data Channel
  if ((status & 0x7F) > MW_FTP_STAT_OPENED) {
    which = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);
    start_mark = FreeRunMark();
    do {
      status = GetNShowStatus(TRUE);
      if (status <= MW_FTP_STAT_OPENED)
        break;
    } while (FreeRunElapsed(start_mark) < TIMEOUT_STATUS);
  }

  // Close Command channel
  if (status) {
    which = MW_FTP_CMD_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);
    do {
      status = GetNShowStatus(TRUE);
      SleepMW();
    } while (status == MW_FTP_STAT_CLOSING);
  }
}
//*****************************************************************************
//  Function        : FlushAll
//  Description     : Flush All Channel.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void FlushAll(void)
{
  int i;
  BYTE tmp[2048];

  for (i = 10; i; i --) {
    IOCtlMW(gFtpHandle, IO_FTP_CMD_READ, tmp);
    ReadMW(gFtpHandle, tmp, sizeof(tmp));
    //SleepMW();
  }
}
//*****************************************************************************
//  Function        : FtpOpen
//  Description     : Open & Login
//  Input           : aCfg;           // TCP port info.
//                    aLoginName;     // Login name
//                    aLoginPassword; // password.
//  Return          : 0/-1;       // 0  => Success
//                                // -1 => Error
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int FtpOpen(struct MW_TCP_PORT *aCfg, char *aLoginName, char *aLoginPassword)
{
  int i;

  // Close all
  CloseAll();
  TBuffers.gRxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
  TBuffers.gTxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
  memset(TBuffers.gRxBuf,0x00,COMM_BUFF_SIZE);
  memset(TBuffers.gTxBuf,0x00,COMM_BUFF_SIZE);
  // Open Port
  graphicWindow("CONEXION FTP");
  DispLineMW("Conectando...",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_BIGFONT);

  FlushAll ();
  if (IOCtlMW(gFtpHandle, IO_FTP_CONNECT, aCfg) == FALSE) {
	  graphicWindow("ERROR");
	  //centerS(3,1,21,"Fallo al intentar");
	  DispLineMW("Fallo al intentar",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  DispLineMW("Abrir!!",MW_LINE5,MW_CLRDISP|MW_CENTER|MW_SMFONT);
//	  centerS(4,1,21,"Abrir!!");
	  Delay10ms(300);
	  FreeMW(TBuffers.gRxBuf);
	  FreeMW(TBuffers.gTxBuf);
    return -1;
  }
  i = WaitCmdResp(TIMEOUT_RESP);
  if (i < 0) {
		graphicWindow("ERROR");
//		centerS(3,1,21,"El tiempo de espera");
//		centerS(4,1,21,"se agoto al");
//		centerS(5,1,21,"intentat abrir!!");
		DispLineMW("El tiempo de espera",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_SMFONT);
		DispLineMW("se agoto al",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
		DispLineMW("intentat abrir!!",MW_LINE5,MW_CLRDISP|MW_CENTER|MW_SMFONT);
//		getch(300);
		 Delay10ms(300);
		 FreeMW(TBuffers.gTxBuf);
		 FreeMW(TBuffers.gRxBuf);
    return -1;
  }
  if (i >= 400) {
    PrintfMW("\x1B\xE0""Open:%d\x05\x1D", i);
    FreeMW(TBuffers.gRxBuf);
    return -1;
  }

  if (WaitStatus(MW_FTP_STAT_CONNECTED, TIMEOUT_STATUS)<0) {
	  graphicWindow("ERROR");
//	  centerS(3,1,21,"Fallo al intentar");
//	  centerS(4,1,21,"Abrir!!");
//	  getch(300);
	  DispLineMW("Fallo al intentar",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  DispLineMW("Abrir 2!!",MW_LINE5,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  Delay10ms(300);
	  FreeMW(TBuffers.gRxBuf);FreeMW(TBuffers.gTxBuf);
    return -1;
  }

  // Login
  DispLineMW("Negociando...",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
  //centerS(4,1,21,"Negociando...");
  FlushAll();
  SprintfMW ((char *)TBuffers.gTxBuf, "LOGIN %s %s", aLoginName, aLoginPassword);
  if (IOCtlMW(gFtpHandle, IO_FTP_CMD_SEND, TBuffers.gTxBuf) == FALSE) {
	  graphicWindow("ERROR");
	  DispLineMW("Fallo al intentar",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  DispLineMW("Login",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
//	  centerS(3,1,21,"Fallo al intentar");
//	  centerS(4,1,21,"login!!");
//	  getch(300);
	  Delay10ms(300);
	  FreeMW(TBuffers.gRxBuf);FreeMW(TBuffers.gTxBuf);
    return -1;
  }
  i = WaitCmdResp(TIMEOUT_RESP);
  if (i < 0) {
		graphicWindow("ERROR");
//		centerS(3,1,21,"El tiempo de espera");
//		centerS(4,1,21,"se agoto al");
//		centerS(5,1,21,"intentat login!!");
		  DispLineMW("El tiempo de espera",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_SMFONT);
		  DispLineMW("se agoto al",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
		  DispLineMW("intentat login!!",MW_LINE5,MW_CLRDISP|MW_CENTER|MW_SMFONT);
//		getch(300);
		  Delay10ms(300);
		  FreeMW(TBuffers.gRxBuf);FreeMW(TBuffers.gTxBuf);
    return -1;
  }
  if (i >= 400) {
	  graphicWindow("ERROR");
//	  centerS(3,1,21,"Fallo al intentar");
//	  centerS(4,1,21,"login!!");
//	  getch(300);
	  DispLineMW("Fallo al intentar",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  DispLineMW("login!!",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  Delay10ms(300);
	  FreeMW(TBuffers.gRxBuf);FreeMW(TBuffers.gTxBuf);
    return -1;
  }
  if (WaitStatus(MW_FTP_STAT_OPENED, TIMEOUT_STATUS) < 0) {
	  graphicWindow("ERROR");
//	  centerS(3,1,21,"Fallo al intentar");
//	  centerS(4,1,21,"login!!");
//	  getch(300);
	  DispLineMW("Fallo al intentar",MW_LINE3,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  DispLineMW("login!!",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
	  Delay10ms(300);
	  FreeMW(TBuffers.gRxBuf);FreeMW(TBuffers.gTxBuf);
    return -1;
  }
  return 0;
}
//==========================================================================
// Remove leading and tailing space on line buffer
//==========================================================================
void TrimLineBuf (void)
{
  int i;
  int idx;
  int len;

  // Remove leading space
  len = strlen (gLineBuf);
  for (idx = 0; idx < len; idx ++)
  {
    if (gLineBuf[idx] != ' ')
      break;
  }
  if (idx)
  {
    for (i = 0; idx < (len + 1); idx ++, i++)
      gLineBuf[i] = gLineBuf[idx];
  }

  // Remove tailing space
  len = strlen (gLineBuf);
  for (idx = (len - 1); idx; idx --)
  {
    if (gLineBuf[idx] == ' ')
      gLineBuf[idx] = 0;
    else
      break;
  }
}
//==========================================================================
// State processing when getting list file
//==========================================================================
static int ProcGetListFileState (int aState)
{
  //BYTE buf[20];

  if (strlen (gLineBuf) == 0)
    return aState;

  switch (aState)
  {
    case S_IDLE:
      if (strcmp(gLineBuf, "[T1000]") == 0)
      {
//      	clear_area_screen(3,2,7,20,FALSE, FALSE);
//      	centerS(3,1,21,"Recibiendo lista"),
//      	centerS(4,1,21,"de archivos..."),
        //SprintfMW((char *)buf, "\x12\x1b\x40Rxing file list.\x5");
        //PrintfMW(buf);
        aState = S_RXING_DATA;
      }
      break;

    case S_RXING_DATA:
      if (gLineBuf[0] == '[')
      {
//      	gotoxy(3,2);printf("                  ",gNumOfFiles);
//      	gotoxy(4,2);printf("                  ",gNumOfFiles);
//      	gotoxy(3,3);printf("No. Archivos: %2d",gNumOfFiles);
        //SprintfMW((char *)buf, "\x12\x1b\x40""Found %d files.\x5", gNumOfFiles);
        //PrintfMW(buf);
        aState = S_SUCCESS;
      }
      else
      {
        if (strlen (gLineBuf) >= SIZE_FILENAME)
        {
//          centerS(4,1,21,"Nombre de archivo");
//          centerS(5,1,21,"muy extenso !!!");
//          getch(0);
          //ShowMsgWait("Filename too large.", TRUE);
          aState = S_FAILURE;
          break;
        }

        if (gNumOfFiles >= MAX_NUM_OF_FILES)
        {
//          centerS(4,1,21,"El numero de archivos");
//          centerS(5,1,21,"excede el maximo !!!");
//          getch(0);
          //ShowMsgWait("Too many files.", TRUE);
          aState = S_FAILURE;
          break;
        }

        strcpy(gFileList[gNumOfFiles], gLineBuf);
        gNumOfFiles++;
      }
      break;
  }
  return aState;
}
//*****************************************************************************
//  Function        : GetListFile
//  Description     : Get list file from host.
//  Input           : N/A
//  Return          : 0/-1;         // 0  => Success
//                                  // -1 => Error
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int GetListFile(void)
{
  int retry;
  int len, rx_len;
  int i, state, line_idx;
  DWORD getfile_mark, rx_mark, which;
  TBuffers.gRxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
  TBuffers.gTxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
   memset(TBuffers.gRxBuf,0x00,COMM_BUFF_SIZE);
   memset(TBuffers.gTxBuf,0x00,COMM_BUFF_SIZE);
  for (retry=0; retry<NUM_OF_RETRY; retry++) {
    FlushAll ();
    which = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);
    FlushAll ();
    graphicWindow("DESCARGANDO INFO");
    printf("\n\nArchivo %s",STR_LIST_FILE_NAME);
    //SprintfMW ((char *)gTxBuf, "\x12\x1b\x20""Dnld %s..\x5", STR_LIST_FILE_NAME);
    //PrintfMW(gTxBuf);
    SprintfMW ((char *)TBuffers.gTxBuf, "GET I /%s%s", STR_TEST_DIR_NAME, STR_LIST_FILE_NAME);
    if (IOCtlMW(gFtpHandle, IO_FTP_CMD_SEND, TBuffers.gTxBuf) == FALSE) {
   //   ShowMsgWait("GET:Command Fail.", TRUE);
      break;
    }

    if (WaitStatus(MW_FTP_STAT_DATA_CONNED, TIMEOUT_STATUS) < 0) {
//        centerS(5,1,21,"Fallo de datos");
  	  DispLineMW("Fallo de datos",MW_LINE4,MW_CLRDISP|MW_CENTER|MW_SMFONT);
//        getch(0);
        WaitKey(300,1);
      break;
    }
    gNumOfFiles = 0;
    gRxSpeed = 0;
    rx_len = 0;
    getfile_mark = FreeRunMark();
    while (1) {
      DWORD timeout=0;
      int ret;

      ret = GetCmdResp();
      if (ret >= 400) {
        PrintfMW("\x1B\xE0\x1CGET:%d\x05\x1D", ret);
        break;
      }

      state = S_IDLE;
      line_idx = 0;
      rx_mark = FreeRunMark();
      do {
        len = ReadMW(gFtpHandle, TBuffers.gRxBuf, COMM_BUFF_SIZE);
        if (len) {
          for (i=0; i<len; i++) {
            // Process data
            if ((TBuffers.gRxBuf[i] == 0x0d) || (TBuffers.gRxBuf[i] == 0x0a)) {
              if (line_idx) {
                gLineBuf[line_idx] = 0;
                line_idx = 0;
                TrimLineBuf();
                state = ProcGetListFileState(state);
              }
            }
            else if (line_idx < SIZE_LINE) {
              gLineBuf[line_idx] = TBuffers.gRxBuf[i];
              line_idx++;
            }
          }
          break;
        }
        if (WaitStatus(MW_FTP_STAT_DATA_CONNED, 1) < 0) {
          timeout = TIMEOUT_READ_BLOCK;
          break;
        }
        if (GetCharMW() == MWKEY_CANCL)
          break;
        //SleepMW();
      } while ((timeout = FreeRunElapsed(rx_mark)) < TIMEOUT_READ_BLOCK);

      if (timeout >= TIMEOUT_READ_BLOCK)   // End ?
        break;

      rx_len += len;
     printf("\fRx: %3d.%d K.bytes",rx_len/1000,(rx_len%1000)/100);
      //dumpMemory(TBuffers.gRxBuf,rx_len,1);
      //printf("\f%s",TBuffers.gRxBuf);getch(0);
      //PrintfMW("\x1B\x80""RXed %d bytes\x05\n\x05", rx_len);
      if (GetCharMW() == MWKEY_CANCL)
        break;
    }

    if (rx_len == 0) {
      graphicWindow("ERROR FTP");
      //clear_area_screen(2,2,7,20,FALSE);
    //  centerS(4,1,21,"RX Timeout.");
      WaitKey(300,1);
      continue;
    }
    which = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);  // Close Data channel
    gRxSpeed = (rx_len/(FreeRunElapsed(getfile_mark)/1000));
    //PrintfMW("\x1B\xA0Rx Spd:%d Bps\n", gRxSpeed);
    FreeMW(TBuffers.gRxBuf);
    FreeMW(TBuffers.gRxBuf);
    return 0;
  }
  FreeMW(TBuffers.gTxBuf);
  FreeMW(TBuffers.gTxBuf);
  return -1;
}
//*****************************************************************************
//  Function        : GetFile
//  Description     : Get a file from host.
//  Input           : aIdx          // index of file to download
//  Return          : 0/-1;         // 0  => Success
//                                  // -1 => Error
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int GetFile(int aIdx)
{
  T_APP_INFO app_list;
  int retry, j;
  int len, rx_len;
  BYTE auxname[30+1];
  BYTE linea[30+1];
  DWORD getfile_mark, rx_mark, which;
  int  handle;
  TBuffers.gRxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
  TBuffers.gTxBuf = MallocMW(COMM_BUFF_SIZE); // buffer de recepcion de datos
   memset(TBuffers.gRxBuf,0x00,COMM_BUFF_SIZE);
   memset(TBuffers.gTxBuf,0x00,COMM_BUFF_SIZE);
  for (retry=0; retry<NUM_OF_RETRY; retry++) {
    FlushAll ();
    which = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);
    FlushAll ();

    handle = os_file_open(KTempFile, K_O_RDWR|K_O_TRUNC|K_O_CREAT);
    if (handle < 0) {
      //ShowMsgWait("Temp File Open Fail.", TRUE);
      FreeMW(TBuffers.gRxBuf);
      FreeMW(TBuffers.gTxBuf);
      return -1;
    }

    //SprintfMW ((char *)gTxBuf, "\x12\x1b\x20""Dnld %s..\x5", gFileList[aIdx]);
    //PrintfMW(gTxBuf);
//    clear_area_screen(4,2,7,20,FALSE,FALSE);
    sprintf ((char *)TBuffers.gTxBuf, "Descarga %.8s", gFileList[aIdx]);
    

    memset(auxname,0x00,sizeof(auxname));
    memcpy(auxname,gFileList[aIdx],sizeof(auxname));


    memset(linea, 0x00, sizeof(linea));
   // printf("\f\x1C      VERSIONES      \x1D");
    for( j = 0; j < 50; j++ )
    {
        if(os_app_search_id(j) == TRUE)
        {
      
            os_app_info(j , &app_list);
       
            memset(linea,0x00,sizeof(linea));
            sprintf(linea, "%s-%04X.tms", &app_list.s_app_name[2], app_list.w_checksum);
            //printf("\f %s \n %s\n %d",linea,gFileList[aIdx], gNumOfFiles);APM_WaitKey(3000,0);

            if(memcmp(linea,auxname,30)==0){
              os_file_close(handle);
              FreeMW(TBuffers.gRxBuf);
              FreeMW(TBuffers.gTxBuf);
              return 1;
            }
        }

    }

    //ShowMsgWait(TBuffers.gTxBuf, FALSE);
//    centerS(4,2,21,TBuffers.gTxBuf);
    SprintfMW ((char *)TBuffers.gTxBuf, "GET I /%s%s", STR_TEST_DIR_NAME, gFileList[aIdx]);
    if (IOCtlMW(gFtpHandle, IO_FTP_CMD_SEND, TBuffers.gTxBuf) == FALSE) {
      //ShowMsgWait("GET:Command Fail.", TRUE);
      break;
    }

    if (WaitStatus(MW_FTP_STAT_DATA_CONNED, TIMEOUT_STATUS) < 0) {
    	graphicWindow("ERROR FTP");
//    	centerS(3,1,21,"Falla al");
//    	centerS(4,1,21,"Obtener los");
//    	centerS(5,1,21,"datos de FTP");
      //ShowMsgWait("GET:Fail.", TRUE);
      break;
    }
    gRxSpeed = 0;
    rx_len = 0;
    getfile_mark = FreeRunMark();
    while (1) {
      DWORD timeout=0;
      int ret;

      ret = GetCmdResp();
      if (ret >= 400) {
        PrintfMW("\x1B\xE0\x1CGET:%d\x05\x1D", ret);
        break;
      }

      rx_mark = FreeRunMark();
      do {
        len = ReadMW(gFtpHandle, TBuffers.gRxBuf, COMM_BUFF_SIZE);
        if (len > 0) {
          os_file_write(handle, TBuffers.gRxBuf, len);      // save to file
          break;
        }
        if (WaitStatus(MW_FTP_STAT_DATA_CONNED, 1) < 0) {
          timeout = TIMEOUT_READ_BLOCK;
          break;
        }
        //SleepMW();
      } while ((timeout = FreeRunElapsed(rx_mark)) < TIMEOUT_READ_BLOCK && GetCharMW() != MWKEY_CANCL);

      if (GetCharMW() == MWKEY_CANCL)
        break;

      if (timeout >= TIMEOUT_READ_BLOCK)   // End ?
        break;

      rx_len += len;
      printf("\fRx: %3d.%d K.bytes",rx_len/1000,(rx_len%1000)/100);
      //PrintfMW("\x1B\x80""RXed %d bytes\x05\n\x05\n\x05", rx_len);
    }
    os_file_close(handle);

    if (rx_len == 0) {
       graphicWindow("ERROR FTP");
//       clear_area_screen(2,2,7,20,FALSE,FALSE);
//       centerS(4,1,21,"RX Timeout.");
    		  ;
      os_file_close(handle);
      continue;
    }
    os_file_delete(gFileList[aIdx]);
    os_file_rename(gFileList[aIdx], KTempFile);
    which = MW_FTP_DATA_CHNL;
    IOCtlMW(gFtpHandle, IO_FTP_DISCONNECT, &which);  // Close Data channel
    gRxSpeed = (rx_len/(FreeRunElapsed(getfile_mark)/1000));
    //PrintfMW("\x1B\xA0Rx Spd:%d Bps\n", gRxSpeed);
    FreeMW(TBuffers.gRxBuf);
    FreeMW(TBuffers.gTxBuf);
    return 0;
  }
  os_file_close(handle);
  FreeMW(TBuffers.gRxBuf);
  FreeMW(TBuffers.gTxBuf);
  return -1;
}
//*****************************************************************************
//  Function        : WriteFileToApp
//  Description     : Write downloaded file to application.
//  Input           : aIdx          // index of file to download
//  Return          : 0/-1;         // 0  => Success
//                                  // -1 => Error
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
static int WriteFileToApp(int aIdx)
{
  return DownloadWrite(gFileList[aIdx]);
}



//return:
//    -1: Error en la comunicacion con el servidor de descarga.
//    -2: Mensaje de respuesta mal formado.
//    -3: Error en servidores FTP.
//    -4: Servidores FTP ocupados, reintentar en retryFTP


//OJO NOHORA 13 - MAR 2013//
void setApn(void)
{

	BYTE tmp [50 + 1];
	struct TCPIP tcpip;

	DispCtrlMW(MW_CLR_DISP);

	memcpy(&tcpip, &gTcpIp, sizeof(struct TCPIP));

	DispLineMW("GPRS PRIMARIO", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	DispLineMW("Dominio", MW_LINE2, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_APN);
	memcpy(&tmp[1],tcpip.sb_APN,strlen(tcpip.sb_APN));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE3+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
		return ;
	memset(tcpip.sb_APN,0x00,sizeof(tcpip.sb_APN));
	memcpy(tcpip.sb_APN, &tmp[1], tmp[0]);
	memset(gTcpIp.sb_APN, 0x00, sizeof(gTcpIp.sb_APN));
	memcpy(gTcpIp.sb_APN, &tmp[1], tmp[0]);


	DispLineMW("Usuario", MW_LINE5, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_UserName);
	memcpy(&tmp[1],tcpip.sb_UserName,strlen(tcpip.sb_UserName));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE6+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_UserName,0x00,sizeof(tcpip.sb_UserName));
	memcpy(tcpip.sb_UserName,&tmp[1],tmp[0]);
	memset(gTcpIp.sb_UserName, 0x00, sizeof(gTcpIp.sb_UserName));
	memcpy(gTcpIp.sb_UserName, &tmp[1], tmp[0]);


	DispLineMW("Clave", MW_LINE7, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_Password);
	memcpy(&tmp[1],tcpip.sb_Password,strlen(tcpip.sb_Password));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE8+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_Password,0x00,sizeof(tcpip.sb_Password));
	memcpy(tcpip.sb_Password,&tmp[1],tmp[0]);
	memset(gTcpIp.sb_Password, 0x00, sizeof(gTcpIp.sb_Password));
	memcpy(gTcpIp.sb_Password, &tmp[1], tmp[0]);


	SaveTcpIp(&gTcpIp);            // Guarda los cambios realizados

	/*DispCtrlMW(MW_CLR_DISP);
	DispLineMW("CONFIGURACION", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
	DispLineMW("CAMBIO EXITOSO!!!", MW_LINE5, MW_SMFONT| MW_LEFT | MW_CENTER);
	AcceptBeep();*/

	APM_ResetComm();
	APM_DisconnectPPP();
	//Delay1Sec(3,0);
	PackComm(TRUE, FALSE);
	return;
}



//*****************************************************************************
//  Function        : DownloadFtp
//  Description     : App download over FTP.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN DownloadFtp(void)
{
  int i, wr_ok=0, gf=0;
  //BYTE lon=0,IP[4];
  char filename[32]/*,*tem*/;
  struct MW_TCP_PORT port2;
  struct FTPPARAM ftpparam;
  BYTE ipHost[4];
  BYTE puerto[6+7];
  BYTE usuario[21];
  BYTE clave[21];


// memset(Toptgral.downloadIP,0x00,sizeof(Toptgral.downloadIP));
// strcpy(Toptgral.downloadIP,"174.142.211.61");
//printf("paso 1");getch(0);
  //  strcpy(Toptgral.downloadIP,"210.3.29.84");
  // download auth
  if (DownloadAuth() < 0) {
	  graphicWindow("FALLA auth");
//	  centerS(3,1,21,"Proceso de");
//	  centerS(4,1,21,"Autenticacion");
//	  centerS(6,1,21,"Fallo !!!");
	  printf("\nAUTH ERR");
//	  getch(0);
	  Delay10ms(500);
	  return TRUE;
  }

  //GetFtpParam(&ftpparam);
  /*printf("\fIPftp:|%d %d %d %d|", ftpparam.ipHost[0], ftpparam.ipHost[1], ftpparam.ipHost[2], ftpparam.ipHost[3]);
  GetChar();
  printf("\fPtoftp:|%d|", ftpparam.portFtp);
  GetChar();
  printf("\fUsuftp:|%s|", ftpparam.gUsername);
  GetChar();
  printf("\fPassftp:|%s|", ftpparam.gPassword);
  GetChar();*/

  memset(usuario,0x00,sizeof(usuario));
  memcpy(&usuario[1], gFtpParam.userID , strlen(gFtpParam.userID));
  usuario[0] = strlen(gFtpParam.userID);
  memset(clave,0x00,sizeof(clave));
  split(&clave[1], gtoken_97.worKeyPin, 2);
  clave[0] = 4;
  memset(puerto,0x00,sizeof(puerto));
  sprintf(puerto, "%d", gFtpParam.portFtp);
  memset(ipHost,0x00,sizeof(ipHost));
  memcpy(ipHost, gFtpParam.ipHost, 4);

  if( flagFtpManual == TRUE )
  {
	  setApn( );	//OJO NOHORA 13 - MAR 2013//

	  printf("\f");
	  DispLineMW("INFORMACION", MW_LINE1, MW_CENTER|MW_SMFONT);
	  DispLineMW("IP:",MW_LINE2,MW_CENTER|MW_SMFONT);
	  EditIp(ipHost, MW_LINE3);
	  //DispLineMW(buf, MW_LINE3, MW_CENTER|MW_SPFONT);
	  //printf("\f");
	  DispLineMW("Puerto:", MW_LINE4, MW_CENTER|MW_SMFONT);
	  GetString(puerto, MW_LINE5, 5);
	  //DispLineMW(puerto, MW_LINE5, MW_CENTER|MW_SPFONT);
	  //printf("\f");
	  DispLineMW("Usuario:", MW_LINE6, MW_CENTER|MW_SMFONT);
	  //memcpy(&usuario[1], gUsername, );
	  APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SPFONT + MW_LINE7 + ECHO, IMIN(1) + IMAX(16), usuario);
	  //DispLineMW(gUsername, MW_LINE7, MW_CENTER|MW_SPFONT);
	  //printf("\f");
	  DispLineMW("Clave:", MW_LINE8, MW_CENTER|MW_SMFONT);
	  APM_GetKbdSpectra(ALPHA_INPUT_NEW + MW_SPFONT + MW_LINE9 + ECHO, IMIN(1) + IMAX(16), clave);
	  //DispLineMW(gPassword, MW_LINE9, MW_CENTER|MW_SPFONT);
	  //WaitKey(1000, 0);

	  DispLineMW( "REALMENTE", MW_LINE3, MW_CENTER|MW_SMFONT|MW_CLRDISP );
	  DispLineMW( "DESEA EJECUTAR", MW_LINE4, MW_CENTER|MW_SMFONT );
	  DispLineMW( "LA DESCARGA FTP ?", MW_LINE5, MW_CENTER|MW_SMFONT );

	  if( SiNo() != 2 )   // 2 = Press Enter
			return FALSE;

	  AcceptBeep();
	  memset(&ftpparam, 0, sizeof(struct FTPPARAM));

	  memcpy(ftpparam.ipHost, ipHost, 4);
	  ftpparam.portFtp = atoi(puerto);
	  memcpy(ftpparam.userID, &usuario[1], usuario[0]);
	  memcpy(ftpparam.Password, &clave[1],clave[0]);
  }
  else{
	  memset(&ftpparam, 0, sizeof(struct FTPPARAM));

	  memcpy(ftpparam.ipHost, gToken_96.ipTeleCarga, 4);
	  memset(puerto, 0x00, sizeof(puerto));
	  memcpy(puerto, gToken_96.puertoTeleCarga, 5);
	  ftpparam.portFtp = atoi(puerto);
	  memcpy(ftpparam.userID, "CNB", 3);
	  split(ftpparam.Password, gtoken_97.worKeyPin, 2);
  }

  memcpy(&gFtpParam, &ftpparam, sizeof(struct FTPPARAM));
  SaveFtpParam(&gFtpParam);

  /*printf("\fIPftp2:|%d %d %d %d|", ftpparam.ipHost[0], ftpparam.ipHost[1], ftpparam.ipHost[2], ftpparam.ipHost[3]);
  GetChar();
  printf("\fPtoftp2:|%d|", ftpparam.portFtp);
  GetChar();
  printf("\fUsuftp2:|%s|", ftpparam.gUsername);
  GetChar();
  printf("\fPassftp2:|%s|", ftpparam.gPassword);
  GetChar();*/

//  printf("\fusuario: %s",gUsername);WaitKey(5000,0);
//  printf("\fpass: %s",gPassword);WaitKey(5000,0);
  /*  DispPutStr("\x12\x1B\x05\x01""Username:\x5\n");
    GetString(gUsername, MW_LLINE6, MW_LCOL1, 15);

    DispPutStr("\x12\x1B\x05\x01Pwd:\x5\n");
    GetString(gPassword, MW_LLINE6, MW_LCOL1, 15);		   */
 // memset(gUsername,0x00,sizeof(gUsername));
//  strcpy(gUsername,"wwtest");
//  strcpy(gUsername,"t800");
 // memset(gPassword,0x00,sizeof(gPassword));
//  strcpy(gPassword,"1234");
//  strcpy(gPassword,"HasFTP..");
// IP[0]=67;IP[1]=210;IP[2]=244;IP[3]=44;
  /*  */
 /*    IP[0]=200;
   IP[1]=26;
   IP[2]=147;
   IP[3]=234;
 
  
  memcpy(&port2.d_ip, IP, 4);
  port2.w_port = 21;*/
//  port.w_port = Toptgral.downloadPort;
  //////////////////
  /*IP[0]=210;
  IP[1]=3;
  IP[2]=29;
  IP[3]=84;*/
  //memcpy(&port2.d_ip, IP, 4);

  memcpy(&port2.d_ip, gFtpParam.ipHost, 4);
  port2.w_port = gFtpParam.portFtp;


  //dumpMemory((BYTE *)&port2,100,1);

  //printf("\f%s y %s",gUsername,gPassword);getch(0);
    ////////////////
  port2.b_sslidx = 0;

  port2.b_option = 0;

//  if (GetPPPStatus() != MW_PPP_PHASE_READY){
//  PPPSetup();
//  PPPConnect();
//  }
  printf("\f");
  APM_ConnectPPP(TRUE);
//
//  if (GetPPPStatus() != MW_PPP_PHASE_READY)
//    return;
  port2.b_option |= MW_TCP_PPP_MODE;
  //port2.b_option |= 0x13;


  /* if (GetPPPStatus() == MW_PPP_PHASE_READY) {
     int option = 1;
     PutCharMW(MW_LPUSH_CURSOR);
     ClrBelow(MW_LLINE2);
     option = ToggleOption("\x11\x1B\x05\x01Use PPP Channel?", KTrueFalse, option);
     if (option == -1)
       return;
     if (option == 1) {
       port.b_option |= MW_TCP_PPP_MODE;
     }
     PutCharMW(MW_LPOP_CURSOR);
   }		*/

  /* if ((port.b_option & MW_TCP_PPP_MODE) == 0) {
     // enable LAN interface if needed
     if ((bs_lan_status(K_BsLanStatus) & 0x02) == 0) {
       bs_lan_open();
       while ((bs_lan_status(K_BsLanStatus) & 0x02) != 1) {
         ShowStatus("Enabling LAN...\x05", STS_ERROR);
         SleepMW();
       }
     }
     // check LAN cable
     if (!LanCableInsertedMW()) {
       ShowStatus("Check Lan Cable!\x05", STS_ERROR);
       WaitKey();
       return;
     }
   }		*/
  strcpy(filename, DEV_FTP1);
  gFtpHandle = OpenMW(filename, 0);
  if (gFtpHandle==-1) {
	graphicWindow("FALLA serv");
//	centerS(4,1,21,"No se pudo");
//	centerS(5,1,21,"Abrir servidor");
    //DispPutStr("\x12\x1B\x08\x01\x1C""FTP:Open Fail!\x05\x1D");
    CloseAll ();
    CloseMW(gFtpHandle);
//    getch(1000);
    Delay10ms(1000);
    return TRUE;
  }

  memset(gFileList, 0, sizeof(gFileList));
  gNumOfFiles = 0;

  //port2.b_option |= MW_TCP_NO_CLOSE_WAIT;
  port2.b_option |= 0x13;
  //ClrBelow(MW_LLINE5);
  do {
    // Open FTP
    if ( FtpOpen(&port2, gFtpParam.userID, gFtpParam.Password ) < 0 )
      break;

    // Get list file
    if (GetListFile() < 0)
      break;

    // Download files
    for (i=0; i<gNumOfFiles; i++) {
      gf=GetFile(i);
      //printf("\f%d\n %d", gf, i);APM_WaitKey(3000,0);
      if (gf < 0)
        break;
      if(gf == 1)
        continue;
      if (WriteFileToApp(i) == K_APPDL_RESULT_OKAY)
        wr_ok = 1;      // reboot even have 1 app written
    }
  } while (0);

//  DispPutStr("\x1B\x08\x01\x1C""Closing...\x05\x1D");
  CloseAll ();
  CloseMW(gFtpHandle);

//  os_disp_putc(MW_LSEL_SMFONT);
//  os_disp_putc(MW_LREV_ON);

  //centerS( 8, 1,16, "                " );
  //printf(" Fin de Proceso \n" );
//  os_disp_putc(MW_LSEL_SPFONT);
//  os_disp_putc(MW_LREV_OFF);
  //ShowMsgWait("All done.", TRUE);
  /*if (wr_ok) {
//	clear_area_screen(3,3,7,20,FALSE,FALSE);
	//printf( " Reseteando " );
    //ResetMW();
  }*/
  return TRUE;
}
