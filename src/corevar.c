//-----------------------------------------------------------------------------
//  File          : corevar.c
//  Module        :
//  Description   : Include Globals variables for EDC.
//  Author        : Lewis
//  Notes         :
//
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |        Struct define : Leading T                                         |
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
//  29 Sept 2008  Lewis       Initial Version.
//-----------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "midware.h"
#include "hardware.h"
#include "constant.h"
#include "util.h"
#include "corevar.h"
#include "record.h"
#include "Sysutil.h"
#include "CNBTrans.h"
#include "CNBAdmin.h"
#include "print.h"
#include "lptutil.h"
#include "reversal.h"
#include "tranutil.h"


//-----------------------------------------------------------------------------
//      Global Data Structure
//-----------------------------------------------------------------------------
struct GDS *gGDS;
struct APPDATA gAppDat;
struct REVTABLE gRevTable;
struct TCPIP gTcpIp;
struct FTPPARAM gFtpParam;	//OJO NOHORA 12 - MAR 2013//
struct JOURNAL gJournal;
struct MSG60 gMsg60[MAX_MSG];


BOOLEAN gRecaudoVar;
BOOLEAN gPagoTarjeta = FALSE;
BOOLEAN gMsg60Show =FALSE;
BYTE gRrnTemporal[12 + 1];
BOOLEAN flagFtpManual = FALSE;	//OJO NOHORA 12 - MAR 2013//
//BYTE gMensajeError[80];
BYTE *ptrFacturas;

struct USERS_TBL gUsers_tbl;

struct TOKEN_81 gToken_81;
struct TOKEN_97 gtoken_97;
WORD gAttempts;
struct TOKEN_80 gToken_80;
struct INFO_2 gInfo_2;
struct TOKEN_5 gToken_5;
struct TOKEN_5 gToken_5_pack;
struct TOKEN_6 gToken_6;
struct TOKEN_7 gToken_7;
struct TOKEN_8 gToken_8;
struct TOKEN_11	gToken_11;
struct INFO_12 gInfo_12;
struct INFO_13 gInfo_13;
struct TOKEN_14 gToken_14;
struct TOKEN_15 gToken_15;
struct TOKEN_39 gToken_39;
struct TOKEN_47 gToken_47;
struct TOKEN_48 gToken_48;
struct TOKEN_91 gToken_91;
struct TOKEN_96 gToken_96;
struct INFO_98 gInfo_98[5];
struct INFO_98 gInfo_98_pack;
struct TOKEN_1 gToken_1;
struct TOKEN_3PSP gToken_3psp;
struct TOKEN_3PTC gToken_3ptc;

WORD gPort = 7777;
BYTE gIpAddr[4] = { 10, 0, 0, 30 };

int gShowMsgInit;
BYTE gTypeAcount;
BYTE gMsgInit[100];
BYTE gMsgPOS[200];
BOOLEAN gTerminalInit;
BYTE gCuenta[3];
BYTE gAmount[13];
BYTE glocAmount[6];
BYTE tmpAmount[13];
BYTE pinBlock[17];
BYTE pinBlock2[17];
BYTE NSU[13];
BOOLEAN gMultiCuenta;
BOOLEAN gTeleCarga = FALSE;
DWORD lenCupon;
BYTE g_ip[ 15 + 1];
BYTE g_ip1;
BYTE g_ip2;
BYTE g_ip3;
BYTE gAuxStan[12 + 1];

//----------------------------------------------------------------------
//  Application data file
//----------------------------------------------------------------------
static const char KTcpIpTblFile[] = {"TcpIpTbl"};
struct TCPIP *gTcpIpOffset=0;
int gTcpIpFd = -1;

static const char KUsersFile[] = {"UsersTbl"};
struct USERS_TBL *gUsersOffset=0;
int gUsersFd = -1;

int gAppDatHdl = -1;
static const char KAppDatFile[] = {"EDCFILE"};

int gRevTablefd = -1;
static const char KReverseFile[] = {"REVFILE"};	// Archivo para almacenar el reverso

static const char KJournalFile[] = {"JOURNAL"};	// Archivo para almacenar las transacciones en memoria
struct JOURNAL *gJournalOffset=0;
int gJournalFd = -1;

static const char KMsg60File[] = {"MSG60"}; // Archivo para almacenar las transacciones en memoria
struct MSG60 *gMsg60Offset=0;
int gMsg60Fd = -1;

static const char KKeysFile[] = {"KeysTbl"};
struct TOKEN_97 *gKeysOffset=0;
int gKeysFd = -1;

//OJO NOHORA 12 - MAR 2013//
static const char KFtpParamTblFile[] = {"FtpTbl"};
struct FTPPARAM *gFtpParamOffset=0;
int gFtpParamFd = -1;


//*****************************************************************************
//  Function        : DataFileInit
//  Description     : Init. Data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileInit(void)
{
  BYTE filename[32];
  struct APPDATA app_data;

  strcpy(filename, KAppDatFile);
  gAppDatHdl = fOpenMW(filename);
  if (gAppDatHdl < 0) {
    //DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    gAppDatHdl = fCreateMW(filename, FALSE);
    memset(&app_data, 0, sizeof(struct APPDATA));
    fWriteMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));
    fCommitAllMW();
  }
  else
    fReadMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));
  memcpy(&gAppDat, &app_data, sizeof(struct APPDATA));
  fCloseMW(gAppDatHdl);
}



//*****************************************************************************
//  Function        : JournalFileInit
//  Description     : Init. Data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void JournalFileInit(void)
{
  BYTE filename[32];
  struct JOURNAL journalTemp;

  memset(filename, 0x00, sizeof(filename));
  //journalInit = (struct JOURNAL *) MallocMW(sizeof(struct JOURNAL) * MAX_JOURNAL);
  strcpy(filename, KJournalFile);
  gJournalFd = fOpenMW(filename);
  if (gJournalFd < 0) {
    //DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
	gJournalFd = fCreateMW(filename, FALSE);
    memset(&journalTemp, 0x00, sizeof(struct JOURNAL));
    fWriteMW(gJournalFd, &journalTemp, sizeof(struct JOURNAL));
    fCommitAllMW();
  }
  /*else
    fReadMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));
  memcpy(&gAppDat, &app_data, sizeof(struct APPDATA));*/
  //FreeMW(journalInit);
  fCloseMW(gJournalFd);
}


void Msg60FileInit(void)
{
  BYTE filename[32];
  struct MSG60 msg60Temp;

  memset(filename, 0x00, sizeof(filename));
  //journalInit = (struct JOURNAL *) MallocMW(sizeof(struct JOURNAL) * MAX_JOURNAL);
  strcpy(filename, KMsg60File);
  gMsg60Fd = fOpenMW(filename);
  if (gMsg60Fd < 0) {
    //DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
  gMsg60Fd = fCreateMW(filename, FALSE);
    memset(&msg60Temp, 0x00, sizeof(struct MSG60));
    fWriteMW(gMsg60Fd, &msg60Temp, sizeof(struct MSG60));
    fCommitAllMW();
  }
  /*else
    fReadMW(gAppDatHdl, &app_data, sizeof(struct APPDATA));
  memcpy(&gAppDat, &app_data, sizeof(struct APPDATA));*/
  //FreeMW(journalInit);
  fCloseMW(gMsg60Fd);
}



//*****************************************************************************
//  Function        : RevTableInit
//  Description     : Init. Reverse Table.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void RevTableInit(void)
{
  BYTE filename[32];
  struct REVTABLE revTable;

  strcpy(filename, KReverseFile);
  gRevTablefd = fOpenMW(filename);
  if (gRevTablefd < 0) {
    //DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
	gRevTablefd = fCreateMW(filename, FALSE);
    memset(&revTable, 0x00, sizeof(struct REVTABLE));
    fWriteMW(gRevTablefd, &revTable, sizeof(struct REVTABLE));
    fCommitAllMW();
  }
  else
    fReadMW(gRevTablefd, &revTable, sizeof(struct REVTABLE));
  memcpy(&gRevTable, &revTable, sizeof(struct REVTABLE));
  fCloseMW(gRevTablefd);
}




//*****************************************************************************
//  Function        : DataFileUpdate
//  Description     : Update Data File from memory.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileUpdate(void)
{
  fCommitAllMW();
}
//*****************************************************************************
//  Function        : DataFileClose
//  Description     : Close local data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void DataFileClose(void)
{
  struct APPDATA app_dat;

  memcpy(&app_dat, &gAppDat, sizeof(struct APPDATA));
  fWriteMW(gAppDatHdl, &app_dat, sizeof(struct APPDATA));
  fCloseMW(gAppDatHdl);
  fCommitAllMW();
  gAppDatHdl = -1;
}
//*****************************************************************************
//  Function        : CorrectHost
//  Description     : Check Whether Support This host.
//  Input           : aHostType;        // Host Type
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN CorrectHost(BYTE aHostType)
{
  //return (aHostType == HANDLE_HOST_TYPE);
  return TRUE;
}
//*****************************************************************************
//  Function        : GenAuthCode
//  Description     : Auto generate Authorization code.
//  Input           : aAuthCode;        // pointer to 6 byte buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void GenAuthCode(BYTE *aAuthCode)
{
  bcdinc(gAppDat.auto_auth_code, 3);
  split(aAuthCode, gAppDat.auto_auth_code, 3);
}
//*****************************************************************************
//  Function        : SetDefault
//  Description     : Restore to Default Setup.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SetDefault(void)
{
  memset(&gAppDat, 0, sizeof(struct APPDATA));
}
//*****************************************************************************
//  Function        : MsgBufSetup
//  Description     : Initialize pack utils for msgbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void MsgBufSetup(void)
{
  set_pptr(MSG_BUF.sb_content,MSG_BUF_LEN);
}
//*****************************************************************************
//  Function        : PackMsgBufLen
//  Description     : Update the msgbuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackMsgBufLen(void)
{
  MSG_BUF.d_len = get_distance();
}
//*****************************************************************************
//  Function        : TxBufSetup
//  Description     : Initialize pack utils for txbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TxBufSetup(BOOLEAN aAdd2ByteLen)
{
  set_pptr(TX_BUF.sbContent,MSG_BUF_LEN);
  // pack additonal 2 byte msg len for TCP/IP connection
  if (aAdd2ByteLen)
    inc_pptr(2);
}
//*****************************************************************************
//  Function        : PackTxBufLen
//  Description     : Update the TxBuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackTxBufLen(BOOLEAN aAdd2ByteLen)
{
  DWORD len;

  TX_BUF.wLen = get_distance();
  if (aAdd2ByteLen) {
    len = TX_BUF.wLen - 2;
    TX_BUF.sbContent[0] = (BYTE)((len >> 8) & 0xFF);
    TX_BUF.sbContent[1] = (BYTE)(len & 0xFF);
  }
}
//*****************************************************************************
//  Function        : RxBufSetup
//  Description     : Initialize pack utils for txbuf.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void RxBufSetup(BOOLEAN aAdd2ByteLen)
{
  set_pptr(RX_BUF.sbContent,MSG_BUF_LEN);
  // pack additonal 2 byte msg len for TCP/IP connection
  if (aAdd2ByteLen)
    inc_pptr(2);
}
//*****************************************************************************
//  Function        : PackRxBufLen
//  Description     : Update the TxBuf's len;
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void PackRxBufLen(BOOLEAN aAdd2ByteLen)
{
  DWORD len;

  RX_BUF.wLen = get_distance();
  if (aAdd2ByteLen) {
    len = RX_BUF.wLen - 2;
    RX_BUF.sbContent[0] = (BYTE)((len >> 8) & 0xFF);
    RX_BUF.sbContent[1] = (BYTE)(len & 0xFF);
  }
}
//*****************************************************************************
//  Function        : ValidRecordCRC
//  Description     : Valid the record's crc.
//  Input           : aIdx;      // record index
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ValidRecordCRC(WORD aIdx)
{
  return (RECORD_BUF.w_crc == cal_crc((BYTE *)&RECORD_BUF,
                                      (BYTE *)&RECORD_BUF.w_crc-(BYTE *)&RECORD_BUF.b_trans));
}
//*****************************************************************************
//  Function        : RefreshDispAfter
//  Description     : Set Display Timer & display changed flag.
//  Input           : aSec;     // timer in sec.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void RefreshDispAfter(DWORD aSec)
{
  gGDS->b_disp_chgd = TRUE;
  aSec *= 200;  // convert unit of 10ms
  TimerSetMW(gTimerHdl[TIMER_DISP], aSec);
}
//*****************************************************************************
//  Function        : FreeGDS
//  Description     : Free GDS buffer.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void FreeGDS(void)
{
  if (gGDS != NULL)
    FreeMW(gGDS);
}
//*****************************************************************************
//  Function        : MallocGDS
//  Description     : Malloc all buffer required.
//  Input           : N/A
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MallocGDS(void)
{
  gGDS = (struct GDS *) MallocMW(sizeof(struct GDS));
  MemFatalErr(gGDS);  // Check Memory
  memset(gGDS, 0x00, sizeof(struct GDS));
  gGDS->s_EMVIn.bMsg = gGDS->sb_IOBuf;
  gGDS->s_EMVOut.bMsg = gGDS->sb_IOBuf;
  return TRUE;
}
//*****************************************************************************
//  Function        : TrainingModeON
//  Description     : Check training mode status;
//  Input           : N/A
//  Return          : TRUE/FALSE;     // TRUE => On.
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN TrainingModeON(void)
{
  return (STIS_TERM_DATA.w_training_status == MAGIC);
}
//*****************************************************************************
//  Function        : IncTraceNo
//  Description     : Increment the system trace no & save to input & tx_data.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void IncAPMTraceNo(void)
{
  bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  if (memcmp(STIS_TERM_DATA.sb_trace_no, "\x00\x00\x00", 3)==0)
    bcdinc(STIS_TERM_DATA.sb_trace_no, 3);
  APM_SetTrace(STIS_TERM_DATA.sb_trace_no);
}

/**************FUNCIONES PICHINCHA*********************************/

//*****************************************************************************
//  Function        : TCPIPInit
//  Description     : Init IP and Port.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void TCPIPInit(void)
{
  BYTE filename[32];
  struct TCPIP tcpip;
  unsigned long len;

  strcpy(filename, KTcpIpTblFile);
  gTcpIpFd = fOpenMW(filename);
  if (gTcpIpFd < 0)
  {
    //DispLineMW("Create: TCP/IP FILE", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
    //GetChar();

    gTcpIpFd = fCreateMW(filename, FALSE);
    if (gTcpIpFd < 0)
    {
      PrintfMW("fCreate ERR:%d\n", gTcpIpFd);
      GetChar();
    }

    if (fSeekMW(gTcpIpFd, MW_FSEEK_SET) < 0)
    {
        DispLineMW("Seek Failed!", MW_LINE2, MW_SMFONT);
        GetChar();
        return;
    }
    memset(&tcpip, 0, sizeof(struct TCPIP));
#if(CX_TCP == TRUE)
    memcpy( tcpip.sb_ip, "\x0A\x00\x4E\x40", 4);	// AMBIENTE DE DESARROLLO LAN 10.0.78.64 (RUBEN)
    //memcpy( tcpip.sb_ip, "\x0A\x00\x4E\x6D", 4);	// AMBIENTE DE DESARROLLO LAN 10.0.78.109 (EDWIN)
    //memcpy( tcpip.sb_ip, "\x0A\x00\x4E\x8C", 4);	// AMBIENTE DE DESARROLLO LAN CON WIRESHARK
    tcpip.w_port  = 7501;
    tcpip.commMode = APM_COMM_TCPIP;					// Comunicacion LAN por default
    tcpip.cxPrimaria = TRUE;							// Comunicacion con el servidor primario
    //printf("\fCx TCP...");APM_WaitKey(3000, 0);
#else
    memcpy( tcpip.sb_ip, "\x0A\xCF\x86\x8A", 4);	// AMBIETE PRODUCCION  CLARO 10.207.134.138
    memcpy( tcpip.sb_ip2, "\x0A\x74\xCD\xB3", 4);	// AMBIETE produccion PARA GPRS MOVISTAR 10.116.205.179	//NOHORA 20Nov
    //memcpy( tcpip.sb_ip, "\xBE\x92\x85\x9D", 4);	//190.146.133.157 //IP publica T&S
   // memcpy( tcpip.sb_ip, "\x0A\x7C\x41\x3C", 4);	// AMBIETE DESARROLLO PARA GPRS CLARO
    tcpip.w_port  = 7501;
    tcpip.w_port2 = 7501;
    strcpy( tcpip.sb_APN, "BANCOPICHINCHACNB.PORTA.COM.EC");	// APN PARA AMBIEN TEST CON CLARO
   	//strcpy( tcpip.sb_APN, "INTERNET.COMCEL.COM.CO");
   	strcpy( tcpip.sb_APN2, "MTM10P1.MOVISTAR.EC");	// AMBIETE DESARROLLO PARA GPRS MOVISTAR	//NOHORA 20Nov
    tcpip.commMode = APM_COMM_GPRS;						// Comunicacion GPRS por default
    tcpip.cxPrimaria = TRUE;							// Comunicacion con el servidor primario
#endif
    fSeekMW(gTcpIpFd, MW_FSEEK_SET);
    WriteSFile( gTcpIpFd, MW_FSEEK_SET, &tcpip, sizeof(struct TCPIP) );
  }
  else
  {
    len = fLengthMW(gTcpIpFd);
    fSeekMW(gTcpIpFd, MW_FSEEK_SET);
    if( fReadMW(gTcpIpFd, &tcpip, sizeof(struct TCPIP) ) != len )
    {
        PrintfMW ("fRead ERR:%d\n", gTcpIpFd);
        GetChar();
        return;
    }
    //printf("tcpip:|%02x %02x %02x %02x|", tcpip.d_ip[0], tcpip.d_ip[1], tcpip.d_ip[2], tcpip.d_ip[3]);
    //GetChar();
  }
  memset(&gTcpIp, 0x00, sizeof(struct TCPIP));
  memcpy(&gTcpIp, &tcpip, sizeof(struct TCPIP));
  fCloseMW(gTcpIpFd);
}


//OJO NOHORA 12 - MAR 2013//
//*****************************************************************************
//  Function        : FTPInit
//  Description     : Init param for FTP Download.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void FTPInit(void)
{
  BYTE filename[32];
  struct FTPPARAM ftpparam;
  unsigned long len;

  strcpy(filename, KFtpParamTblFile);
  gFtpParamFd = fOpenMW(filename);
  if (gFtpParamFd < 0)
  {
    gFtpParamFd = fCreateMW(filename, FALSE);
    if (gFtpParamFd < 0)
    {
      PrintfMW("fCreate ERR:%d\n", gFtpParamFd);
      GetChar();
    }

    if (fSeekMW(gFtpParamFd, MW_FSEEK_SET) < 0)
    {
        DispLineMW("Seek Failed!", MW_LINE2, MW_SMFONT);
        GetChar();
        return;
    }

    memset(&ftpparam, 0x00, sizeof(struct FTPPARAM));

    //memcpy( ftpparam.ipHost, "\xAE\x8E\xD3\x39", 4);	//174.142.211.57 IP FTP CANADA
    memcpy( ftpparam.ipHost, "\xBA\x03\x03\xE4", 4);	//186.3.3.228 FTP GRUPO MAS
    ftpparam.portFtp  = 21;								//21 PUERTO FTP CANADA
   	strcpy( ftpparam.userID, "CNB");					//USUARIO FTP CANADA
   	//strcpy( ftpparam.Password, "Spectra2011");			//CLAVE FTP CANADA

    fSeekMW(gFtpParamFd, MW_FSEEK_SET);
    WriteSFile( gFtpParamFd, MW_FSEEK_SET, &ftpparam, sizeof(struct FTPPARAM) );
  }
  else
  {
    len = fLengthMW(gFtpParamFd);
    fSeekMW(gFtpParamFd, MW_FSEEK_SET);
    if( fReadMW(gFtpParamFd, &ftpparam, sizeof(struct FTPPARAM) ) != len )
    {
        PrintfMW ("fRead ERR:%d\n", gFtpParamFd);
        GetChar();
        return;
    }
    /*printf("\fIPftp:|%d %d %d %d|", ftpparam.ipHost[0], ftpparam.ipHost[1], ftpparam.ipHost[2], ftpparam.ipHost[3]);
    GetChar();
    printf("\fPtoftp:|%d|", ftpparam.portFtp);
    GetChar();
    printf("\fUsuftp:|%s|", ftpparam.gUsername);
    GetChar();
    printf("\fPassftp:|%s|", ftpparam.gPassword);
    GetChar();*/
  }

  memset(&gFtpParam, 0x00, sizeof(struct FTPPARAM));
  memcpy(&gFtpParam, &ftpparam, sizeof(struct FTPPARAM));
  fCloseMW(gFtpParamFd);
}



//*****************************************************************************
//  Function        : WriteSFile
//  Description     : Write to structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN
WriteSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen)
{
  DWORD len;
  BYTE *ptr;

  ptr = (BYTE*)MallocMW( aLen );
  memcpy( ptr, aBuf, aLen );

  fSeekMW(aFileHandle, aOffset);
  len = fWriteMW(aFileHandle, ptr, aLen);
  if (len != aLen)
  {
    DispLineMW("File WRITE Error!", MW_LINE3, MW_CLRDISP | MW_BIGFONT);
    ErrorDelay();
  }

  FreeMW( ptr );
  return (len == aLen) ? TRUE : FALSE;
}




//*****************************************************************************
//  Function        : WriteSFile
//  Description     : Write to structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN
WriteSFileSP(DWORD aFileHandle, DWORD aOffset, const void *aBuf, DWORD aLen)
{
  DWORD len;

  fSeekMW(aFileHandle, aOffset);
  len = fWriteMW(aFileHandle, aBuf, aLen);
  if (len != aLen)
  {
    DispLineMW("File WRITE Error!", MW_LINE3, MW_CLRDISP | MW_BIGFONT);
    ErrorDelay();
  }
  return (len == aLen) ? TRUE : FALSE;
}



//*****************************************************************************
//  Function        : UsersInit
//  Description     : Init. Users Data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void UsersInit(void)
{
  BYTE filename[32];
  struct USERS_TBL users_tbl;

  strcpy(filename, KUsersFile);
  gUsersFd = fOpenMW(filename);

  if (gUsersFd < 0)
  {

    gUsersFd = fCreateMW(filename, FALSE);
    if (gUsersFd < 0)
    {
      PrintfMW("fCreate ERROR:%d\n", gUsersFd);
      GetChar();
    }

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "1234567890", 10 );
    memcpy( &users_tbl.sb_pass, "1234", 4 );
    users_tbl.b_type = KADMINISTRADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 0, &users_tbl );

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "1111111111", 10 );
    memcpy( &users_tbl.sb_pass, "1111", 4 );
    users_tbl.b_type = KOPERADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 1, &users_tbl );

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "2222222222", 10 );
    memcpy( &users_tbl.sb_pass, "2222", 4 );
    users_tbl.b_type = KOPERADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 2, &users_tbl );

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "3333333333", 10 );
    memcpy( &users_tbl.sb_pass, "3333", 4 );
    users_tbl.b_type = KOPERADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 3, &users_tbl );

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "4444444444", 10 );
    memcpy( &users_tbl.sb_pass, "4444", 4 );
    users_tbl.b_type = KOPERADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 4, &users_tbl );

    memset(&users_tbl, 0, sizeof(struct USERS_TBL));

    memcpy( &users_tbl.sb_user, "5555555555", 10 );
    memcpy( &users_tbl.sb_pass, "5555", 4 );
    users_tbl.b_type = KOPERADOR;
    users_tbl.b_attempts = 0;
    users_tbl.b_activo = TRUE;
    SetUsersTbl( 5, &users_tbl );

    //fSeekMW(gUsersFd, MW_FSEEK_SET);
    //WriteSFile( gUsersFd, MW_FSEEK_SET, &users_tbl, sizeof(struct USERS_TBL) );

  }

  fCloseMW(gUsersFd);
}



//*****************************************************************************
//  Function        : SetUsersTbl
//  Description     : Update Users Table File
//  Input           : aIdx;       // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN SetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat)
{

  if (gUsersFd < 0)
    return FALSE;

  if( !WriteSFileSP(gUsersFd, (DWORD) &gUsersOffset[aIdx], aDat, sizeof(struct USERS_TBL)) )
    return FALSE;

  return TRUE;
}





//*****************************************************************************
//  Function        : GetUsersTbl
//  Description     : Read Users Table File
//  Input           : aIdx;       // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*BOOLEAN GetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat)
{

  if (gUsersFd < 0)
    return FALSE;

  if( !ReadSFileSP(gUsersFd, (DWORD) &gUsersOffset[aIdx], aDat, sizeof(struct USERS_TBL)) )
    return FALSE;

  return TRUE;
}*/


//*****************************************************************************
//  Function        : SaveDataFile
//  Description     : Save APPDATA.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveDataFile(struct APPDATA *aApp_data)
{
  int fd;
  char filename[32];

  strcpy(filename, KAppDatFile);

  fd = fOpenMW(filename);

  if(fd < 0)
  {
    DispLineMW("APPDATA File ERROR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    LongBeep();
    GetChar();
    return;
  }

  WriteSFile( fd, MW_FSEEK_SET, aApp_data, sizeof(struct APPDATA));
  fCloseMW(fd);
}



void SaveReverseFile(struct REVTABLE *aRevTable){

	int fd;
	char filename[32];

	memset(filename, 0x00, sizeof(filename));
	strcpy(filename, KReverseFile);

	fd = fOpenMW(filename);

	if(fd < 0)
	{
		DispLineMW("REVERSE File ERROR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
		LongBeep();
		GetChar();
		return;
	}

	WriteSFile( fd, MW_FSEEK_SET, aRevTable, sizeof(struct REVTABLE));
	fCloseMW(fd);
}


//*****************************************************************************
//  Function        : GetUsersCount
//  Description     : Get Number of Users data in the table.
//  Input           : N/A
//  Return          : count;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetUsersCount(void)
{
  char filename[32];
  int fd;
  int len;

  strcpy(filename, KUsersFile);
  fd = fOpenMW(filename);

  if (fd == -1)
    return 0;

  len = fLengthMW(fd) / sizeof(struct USERS_TBL);

  fCloseMW(fd);

  return len;
}



//*****************************************************************************
//  Function        : GetIssuerTbl
//  Description     : Get Issuer Table Data.
//  Input           : aIdx;         // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN GetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat)
{
  char filename[32];
  int fd;

  strcpy(filename, KUsersFile);
  fd = fOpenMW(filename);

  CheckPointerAddr(aDat);

  if (!ReadSFileSP(fd, (DWORD) &gUsersOffset[aIdx], aDat, sizeof(struct USERS_TBL)))
  {
    memset(aDat, 0xFF, sizeof(struct USERS_TBL));
    fCloseMW(fd);
    return FALSE;
  }

  fCloseMW(fd);

  return TRUE;
}



//*****************************************************************************
//  Function        : ReadSFile
//  Description     : Read from structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN
ReadSFileSP(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen)
{
  DWORD len;

  fSeekMW(aFileHandle, aOffset);
  len = fReadMW(aFileHandle, aBuf, aLen);
  return (len == aLen) ? TRUE : FALSE;
}



//*****************************************************************************
//  Function        : UpdUsersTbl
//  Description     : Update Users Table File
//  Input           : aIdx;       // Table Index
//                    aDat;         // pointer data buffer
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN UpdUsersTbl(DWORD aIdx, struct USERS_TBL *aDat)
{
  char filename[32];
  int fd;

  strcpy(filename, KUsersFile);
  fd = fOpenMW(filename);

  if (fd < 0)
    return FALSE;

  if( !WriteSFileSP(fd, (DWORD) &gUsersOffset[aIdx], aDat, sizeof(struct USERS_TBL)) )
  {
    fCloseMW(fd);
    return FALSE;
  }

  fCloseMW(fd);
  return TRUE;
}



void setCommMode(BYTE mode){
  struct TCPIP tcpip;

  /*if(mode == APM_COMM_TCPIP)
  	AMP_DisconnectPPP();*/
  GetTcpIp(&tcpip);
  tcpip.commMode = mode;
  memcpy(&gTcpIp, &tcpip, sizeof(struct TCPIP));
  SaveTcpIp(&gTcpIp);
}



//*****************************************************************************
//  Function        : GetTcpIp
//  Description     : Get TCP/IP.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetDataFile(struct APPDATA *aAppData)
{
  int fd, len;
  char filename[32];

  strcpy(filename, KAppDatFile);
  fd = fOpenMW(filename);

  len = fLengthMW(fd);

  if (fd < 0) {
    return -1;
  }

  ReadSFile( fd, MW_FSEEK_SET, aAppData, sizeof(struct APPDATA));

  //len = fReadMW(fd, aDat, aLen);
  fCloseMW(fd);
  //fCommitAllMW();
  return len;
}



int getReverseFile(void){
	int fd, len;
	char filename[32];
	struct REVTABLE revTable;

	memset(&revTable, 0x00, sizeof(struct REVTABLE));
	memset(filename, 0x00, sizeof(filename));
	strcpy(filename, KReverseFile);

	fd = fOpenMW(filename);

	len = fLengthMW(fd);

	if (fd < 0) {
	    return -1;
	}
	ReadSFile( fd, MW_FSEEK_SET, &revTable, sizeof(struct REVTABLE));
	fCloseMW(fd);
	memcpy(&gRevTable, &revTable, sizeof(struct REVTABLE));
	return len;
}



//*****************************************************************************
//  Function        : GetTcpIp
//  Description     : Get TCP/IP.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetTcpIp(struct TCPIP *aTcpIp)
{
  int fd, len;
  char filename[32];

  strcpy(filename, KTcpIpTblFile);
  fd = fOpenMW(filename);

  len = fLengthMW(fd);

  if (fd < 0) {
    return -1;
  }

  ReadSFile( fd, MW_FSEEK_SET, aTcpIp, sizeof(struct TCPIP));

  //len = fReadMW(fd, aDat, aLen);
  fCloseMW(fd);
  //fCommitAllMW();
  return len;
}


//*****************************************************************************
//  Function        : SaveLastRcpt
//  Description     : Save Last Rcpt Copy.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveTcpIp(struct TCPIP *aTcpIp)
{
  int fd;
  char filename[32];

  strcpy(filename, KTcpIpTblFile);

  fd = fOpenMW(filename);

  if(fd < 0)
  {
    DispLineMW("TCP/IP File ERROR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    LongBeep();
    GetChar();
    return;
  }

  //printf("tcpip:|%02x %02x %02x %02x|", aTcpIp->d_ip[0], aTcpIp->d_ip[1], aTcpIp->d_ip[2], aTcpIp->d_ip[3]);
  //GetChar();

  WriteSFile( fd, MW_FSEEK_SET, aTcpIp, sizeof(struct TCPIP));
  //fWriteMW(fd, aTcpIp, sizeof(struct TCPIP));
  fCloseMW(fd);
}


//OJO NOHORA 12 - MAR 2013//
//*****************************************************************************
//  Function        : GetFtpParam
//  Description     : Get FTP param
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
int GetFtpParam(struct FTPPARAM *aFtpParam)
{
  int fd, len;
  char filename[32];

  strcpy(filename, KFtpParamTblFile);
  fd = fOpenMW(filename);

  len = fLengthMW(fd);

  if (fd < 0) {
    return -1;
  }

  ReadSFile( fd, MW_FSEEK_SET, aFtpParam, sizeof(struct FTPPARAM));

  //len = fReadMW(fd, aDat, aLen);
  fCloseMW(fd);
  //fCommitAllMW();
  return len;
}

//*****************************************************************************
//  Function        : SaveFtpParam
//  Description     : Save Last Rcpt Copy.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void SaveFtpParam(struct FTPPARAM *aFtpParam)
{
  int fd;
  char filename[32];

  strcpy(filename, KFtpParamTblFile);

  fd = fOpenMW(filename);

  if(fd < 0)
  {
    DispLineMW("FTP PARAM File ERROR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
    LongBeep();
    GetChar();
    return;
  }

  //printf("tcpip:|%02x %02x %02x %02x|", aTcpIp->d_ip[0], aTcpIp->d_ip[1], aTcpIp->d_ip[2], aTcpIp->d_ip[3]);
  //GetChar();

  WriteSFile( fd, MW_FSEEK_SET, aFtpParam, sizeof(struct FTPPARAM));
  //fWriteMW(fd, aTcpIp, sizeof(struct TCPIP));
  fCloseMW(fd);
}


//*****************************************************************************
//  Function        : SetIPAddr
//  Description     : Get IP Address.
//  Input           : aLine;    // Line No.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gIpAddr;
//*****************************************************************************
BOOLEAN SetIPAddr( BYTE aServer, DWORD aLineNo)
{
  BYTE tmp[32];

  struct TCPIP tcpip;

  GetTcpIp(&tcpip);

  //memcpy(tmp, &gTcpIp.sb_ip, 4);
  if( aServer == 1 )
    memcpy( tmp, tcpip.sb_ip, 4 );
  else
    memcpy( tmp,tcpip.sb_ip2, 4 );

  DispLineMW("IP: ", aLineNo, MW_SMFONT);

  if (!EditIp(tmp, aLineNo+5))
    return FALSE;

  if( aServer == 1 )
    memcpy( &gTcpIp.sb_ip, tmp, 4 );
  else  // 2
    memcpy( &gTcpIp.sb_ip2, tmp, 4 );

  return TRUE;
}
//*****************************************************************************
//  Function        : SetPort
//  Description     : Get Port No.
//  Input           : aLineNo;  // Line No.
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : gPort;
//*****************************************************************************
BOOLEAN SetPort( BYTE aPort, DWORD aLineNo)
{
  BYTE tmp[32];
  struct TCPIP tcpip;

  GetTcpIp(&tcpip);

  if( aPort == 1 )
    SprintfMW ((char *)tmp, "%d", tcpip.w_port );
  else
    SprintfMW ((char *)tmp, "%d", tcpip.w_port2 );

  DispLineMW("Puerto: ", aLineNo, MW_SMFONT);
  if (GetString ((char *)tmp, aLineNo+8, 5 + 1) < 0) {
    return FALSE;
  }
  //gPort = atoi ((char *)tmp);
  if( aPort == 1 )
    gTcpIp.w_port = atoi ((char *)tmp);
  else
    gTcpIp.w_port2 = atoi ((char *)tmp);

  return TRUE;
}


//*****************************************************************************
//  Function        : ReadSFile
//  Description     : Read from structure data file.
//  Input           : aFileHandle;      // file handle
//                    aOffset;          // file offset from beginning
//                    aBuf;             // data buffer
//                    aLen;             // len to read.
//  Return          : TRUE/FALSE;
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN
ReadSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen)
{
  DWORD len;
  BYTE *ptr;

  ptr = (BYTE*)MallocMW( aLen );

  fSeekMW(aFileHandle, aOffset);
  len = fReadMW(aFileHandle, ptr, aLen);
  memcpy( aBuf, ptr, aLen );

  FreeMW( ptr );
  return (len == aLen) ? TRUE : FALSE;
}




void ModoConexion (BOOLEAN conexion){

	gTcpIp.cxPrimaria = conexion;       // Comunicacion con el servidor seleccionado
	SaveTcpIp(&gTcpIp);            // Guarda los cambios realizados

	DispCtrlMW(MW_CLR_DISP);
	if(conexion){
		DispLineMW("OPERADOR PRIMARIO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
	}
	else
	{
		DispLineMW("OPERADOR SECUNDARIO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
	}


	DispLineMW("Espere por favor", MW_LINE5, MW_SMFONT| MW_LEFT);
	DispLineMW("================", MW_LINE6, MW_SMFONT| MW_LEFT);
	DispLineMW("Desconectando", MW_LINE7, MW_SMFONT| MW_LEFT);
	Delay1Sec(3,1);

	APM_ResetComm();
	APM_DisconnectPPP();
	InitMsg(TRUE);
	return;
}




void ConexPerson (void){

	//BYTE filename[32];
	BYTE tmp [50 + 1];
	BYTE portAscii [7];
	struct TCPIP tcpip;
	//unsigned long len;


	DispCtrlMW(MW_CLR_DISP);

	memcpy(&tcpip, &gTcpIp, sizeof(struct TCPIP));

	DispLineMW("GPRS PRIMARIO", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	DispLineMW("Domio", MW_LINE2, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_APN);
	memcpy(&tmp[1],tcpip.sb_APN,strlen(tcpip.sb_APN));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE3+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
		return ;
	memset(tcpip.sb_APN,0x00,sizeof(tcpip.sb_APN));
	memcpy(tcpip.sb_APN,&tmp[1],tmp[0]);


	DispLineMW("Usuario", MW_LINE5, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_UserName);
	memcpy(&tmp[1],tcpip.sb_UserName,strlen(tcpip.sb_UserName));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE6+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_UserName,0x00,sizeof(tcpip.sb_UserName));
	memcpy(tcpip.sb_UserName,&tmp[1],tmp[0]);


	DispLineMW("Clave", MW_LINE7, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_Password);
	memcpy(&tmp[1],tcpip.sb_Password,strlen(tcpip.sb_Password));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE8+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_Password,0x00,sizeof(tcpip.sb_Password));
	memcpy(tcpip.sb_Password,&tmp[1],tmp[0]);


	DispCtrlMW(MW_CLR_DISP);

	DispLineMW("GPRS PRIMARIO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	memset(tmp,0x00,sizeof(tmp));
	DispLineMW("Servidor", MW_LINE2, MW_SMFONT| MW_LEFT);
	DispLineMW("IP: ", MW_LINE3, MW_SPFONT);
	memcpy(tmp ,tcpip.sb_ip, 4);
	if (!EditIp(tmp, MW_LINE3+5))
		return ;
	memcpy( tcpip.sb_ip, tmp, 4);

	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = 5;
	sprintf(&tmp[1], "%05d", tcpip.w_port);
	memset(portAscii,0x00,sizeof(portAscii));
	DispLineMW("Puerto", MW_LINE6, MW_SMFONT| MW_LEFT);
	/*if (!APM_GetKbd(NUMERIC_INPUT +ECHO + MW_SMFONT + MW_LINE7, IMIN(1) + IMAX(6), tmp))
     return ;*/
	if (!APM_GetKbdSpectra(NUMERIC_INPUT+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(5), tmp))
		return ;

	memcpy(portAscii,&tmp[1], tmp[0]);
	tcpip.w_port=atoi(portAscii);
	//printf("\f puerto uno |%d|",tcpip.w_port);    // sebastian debug


	DispCtrlMW(MW_CLR_DISP);

	DispLineMW("GPRS SECUNDARIO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	DispLineMW("Domio", MW_LINE2, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_APN2);
	memcpy(&tmp[1],tcpip.sb_APN2,strlen(tcpip.sb_APN2));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE3+MW_BIGFONT+RIGHT_JUST, IMAX(32), tmp))
		return ;
	memset(tcpip.sb_APN2,0x00,sizeof(tcpip.sb_APN2));
	memcpy(tcpip.sb_APN2,&tmp[1],tmp[0]);

	DispLineMW("Usuario", MW_LINE5, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_UserName2);
	memcpy(&tmp[1],tcpip.sb_UserName2,strlen(tcpip.sb_UserName2));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE6+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_UserName2,0x00,sizeof(tcpip.sb_UserName2));
	memcpy(tcpip.sb_UserName2,&tmp[1],tmp[0]);


	DispLineMW("Clave", MW_LINE7, MW_SMFONT| MW_LEFT);
	memset(tmp,0x00,sizeof(tmp));
	tmp[0] = strlen(tcpip.sb_Password2);
	memcpy(&tmp[1],tcpip.sb_Password2,strlen(tcpip.sb_Password2));
	if (!APM_GetKbdSpectra(ALPHA_INPUT_NEW+ECHO+MW_LINE8+MW_BIGFONT+RIGHT_JUST, IMAX(16), tmp))
		return ;
	memset(tcpip.sb_Password2,0x00,sizeof(tcpip.sb_Password2));
	memcpy(tcpip.sb_Password2,&tmp[1],tmp[0]);


	DispCtrlMW(MW_CLR_DISP);

	DispLineMW("GPRS SECUNDARIO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	memset(tmp,0x00,sizeof(tmp));
	DispLineMW("Servidor", MW_LINE2, MW_SMFONT| MW_LEFT);
	DispLineMW("IP: ", MW_LINE3, MW_SPFONT);
	memcpy(tmp ,tcpip.sb_ip2, 4);
	if (!EditIp(tmp, MW_LINE3+5))
		return ;
	memcpy( tcpip.sb_ip2, tmp, 4);

	memset(tmp,0x00,sizeof(tmp));
	memset(portAscii,0x00,sizeof(portAscii));
	tmp[0]=5;
	sprintf(&tmp[1],"%05d",tcpip.w_port2);
	DispLineMW("Puerto", MW_LINE6, MW_SMFONT| MW_LEFT);
	/*if (!APM_GetKbd(NUMERIC_INPUT +ECHO + MW_SMFONT + MW_LINE7, IMIN(1) + IMAX(6), tmp))
      return ;*/

	if (!APM_GetKbdSpectra(NUMERIC_INPUT+ECHO+MW_LINE7+MW_BIGFONT+RIGHT_JUST, IMAX(5), tmp))
		return ;

	memcpy(portAscii,&tmp[1],6);
	tcpip.w_port2=atoi(portAscii);

	memcpy(&gTcpIp, &tcpip, sizeof(struct TCPIP));

	SaveTcpIp(&gTcpIp);            // Guarda los cambios realizados

	DispCtrlMW(MW_CLR_DISP);
	DispLineMW("PERSONALIZADO ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
	DispLineMW("CAMBIO EXITOSO!!!", MW_LINE5, MW_SMFONT| MW_LEFT | MW_CENTER);
	AcceptBeep();
	APM_DisconnectPPP();
	Delay1Sec(2,0);
	SrvEthMenu();
	return;
}





void ConfigOpr (BYTE *title, int opr){
	BYTE filename[32];
	struct USERS_TBL users_tbl;
	//struct USERS_TBL users_tbl2;
	//unsigned long len;
	int i;
	WORD j;
	BYTE tmp[11];
	BYTE usr[11];
	int contUser;
	BYTE kbdbuff[11];
	BYTE pass[11];
	BYTE tmpPass[11];

	//memset(&users_tbl,0x00,sizeof(users_tbl2));
	memset(filename, 0x00, sizeof(filename));
	strcpy(filename, KUsersFile);
	//printf("\fopr = %d", opr);WaitKey(3000, 0);
	DispCtrlMW(MW_CLR_DISP);
	DispLineMW(title, MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);

	DispLineMW("Codigo:", MW_LINE3, MW_SMFONT| MW_LEFT );
	DispLineMW("Clave:", MW_LINE5, MW_SMFONT| MW_LEFT );
	DispLineMW("Repite:", MW_LINE7, MW_SMFONT| MW_LEFT );


	memset(usr,0x00,sizeof(usr));
	memset(tmp,0x00,sizeof(tmp));
	APM_SetCol(8); // Set Columna for TextBox
	if (!APM_GetKbd(NUMERIC_INPUT +ECHO + MW_SMFONT + MW_LINE3 , IMIN(10) + IMAX(10), tmp))
		return ;

	memcpy(usr,&tmp[1],tmp[0]);
	//printf("\fusr: |%s|", usr);WaitKey(3000, 0);

	if( tmp[0] < 10 )
    {
		showMessage(10, 3, "ERROR");
	   //	MsgError("Minimo 10 digitos!");
       // ClrBelow(LINE2);
    	return;
    }

	contUser = GetUsersCount();
	//printf("\fCont = %d", contUser);WaitKey(3000, 0);
	for(i=0; i<contUser ;i++){   // for sin terminar ** sebastian **

		if( GetUsersTbl(i, &users_tbl) == FALSE){
			printf("\Error de lectura");WaitKey(3000, 0);
		}
		//printf("\fUsr = %s", users_tbl.sb_user);WaitKey(3000, 0);
		if(memcmp(users_tbl.sb_user,usr,strlen(usr))==0){
			DispCtrlMW(MW_CLR_DISP);
			DispLineMW("AVISO", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
			DispLineMW("Codigo ya existe !!!", MW_LINE6, MW_SMFONT| MW_LEFT | MW_CENTER);
			LongBeep();
			Delay1Sec(3,0);
			return;
		}


	}

	memset(pass, 0x00, sizeof(pass));
	memset(tmpPass, 0x00, sizeof(tmpPass));

	do{
		memset(kbdbuff,0x00,sizeof(kbdbuff));
		APM_SetCol(7); // Set Columna for TextBox
		if (!APM_GetKbd(HIDE_NUMERIC +ECHO + MW_SMFONT + MW_LINE5, IMIN(4) + IMAX(4), kbdbuff))
			return ;

		memcpy(pass, &kbdbuff[1], kbdbuff[0]);

		memset(kbdbuff,0x00,sizeof(kbdbuff));
		APM_SetCol(8); // Set Columna for TextBox
		if (!APM_GetKbd(HIDE_NUMERIC +ECHO + MW_SMFONT + MW_LINE7, IMIN(4) + IMAX(4), kbdbuff))
			return ;

		memcpy(tmpPass, &kbdbuff[1], kbdbuff[0]);

		j = memcmp(tmpPass, pass, sizeof(pass));

	}while(j != 0);

	//printf("\fpass: |%s|", pass);WaitKey(3000, 0);
	memset(&gUsers_tbl, 0x00, sizeof(struct USERS_TBL));
	memcpy(gUsers_tbl.sb_user, usr, 10);
	memcpy(gUsers_tbl.sb_pass, pass, 6);

	gUsers_tbl.b_activo = TRUE;
	gUsers_tbl.b_attempts = 0;

	if(opr==1){
		//printf("\fAdmin...");WaitKey(3000, 0);
		gUsers_tbl.b_type = KADMINISTRADOR;
	}
	else
		gUsers_tbl.b_type = KOPERADOR;


	memset(&users_tbl,0x00,sizeof(users_tbl));
	memcpy(&users_tbl.sb_user, usr, strlen(usr));
	memcpy(&users_tbl.sb_pass, pass, strlen(pass));
	users_tbl.b_activo = TRUE;
	users_tbl.b_attempts = 0;
	users_tbl.b_type = KADMINISTRADOR;


	gUsersFd = fOpenMW(filename);
	SetUsersTbl( opr-1, &users_tbl );
	fCloseMW(gUsersFd);

	DispCtrlMW(MW_CLR_DISP);
	DispLineMW("OPERADOR ", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);
	DispLineMW("CAMBIO EXITOSO!!!", MW_LINE5, MW_SMFONT| MW_LEFT | MW_CENTER);
	AcceptBeep();
	Delay1Sec(2,0);
////////////////////////////
	/*i=0;
	memset(&users_tbl,0x00,sizeof(users_tbl));
	GetUsersTbl(i, &users_tbl);
	printf("\fUsr = %s", users_tbl.sb_user);WaitKey(3000, 0);*/
//////////////////////////////
	return;
}

void SaveTransactionPichincha(void){
	int fd;
	BYTE fileName[8 + 1];
	struct JOURNAL journalTem;
	int len = 0;

	clearReversoPichincha();	// Borra el reverso antes de escribir en el archivo JOURNAL
	len = sizeof(struct JOURNAL);
	memset(fileName, 0x00, sizeof(fileName));
	memset(&journalTem, 0x00, sizeof(struct JOURNAL));
	strcpy(fileName, KJournalFile);

	journalTem.msgType = TX_DATA.Msgtype;
	memcpy( journalTem.procCode, &TX_DATA.procCode, 3);
	memcpy( journalTem.STAN, &TX_DATA.sb_trace_no, 3);
	memcpy( journalTem.RRN, &RSP_DATA.sb_rrn, 12);
	journalTem.amount = TX_DATA.dd_amount;
	memcpy( journalTem.fechaTrans, &RSP_DATA.s_dtg.b_month, 2);
	memcpy( journalTem.horaTrans, &RSP_DATA.s_dtg.b_hour, 3);

  //	printf("\f ContTX %d", gAppDat.transactionCont);WaitKey(3000, 0);
	
	if(gAppDat.transactionCont >= MAX_JOURNAL){
		gAppDat.transactionCont = 0;
		gAppDat.journalFull = TRUE;
	}

	fd = fOpenMW(fileName);
	if(fd < 0){
		printf("\fError al abrir Journal");WaitKey(3000, 0);
		return;
	}

	if( !WriteSFileSP(fd, (DWORD) &gJournalOffset[gAppDat.transactionCont], &journalTem, sizeof(struct JOURNAL)) ){
		printf("\Error De Escritura");WaitKey(3000, 0);
		return;
	}

	fCloseMW(fd);
	gAppDat.transactionCont++;
	SaveDataFile( &gAppDat );
}




void ReadTransactionPichincha(int idFile, int idTrans, struct JOURNAL *aDat){
	//int len = 0;
	//struct JOURNAL journalTem;

	//len = sizeof(struct JOURNAL);
	//printf("\f llego 3");APM_WaitKey(3000,0);
	//memset(&journalTem, 0x00, sizeof(journalTem));
	if(idTrans > MAX_JOURNAL){
		printf("\fIndice Journal Errado");WaitKey(3000, 0);
		return;
	}

	//fSeekMW(idFile, idTrans*len);
	//fReadMW(idFile, aJournalTem, sizeof(struct JOURNAL));
	ReadSFileSP(idFile, (DWORD) &gJournalOffset[idTrans], aDat, sizeof(struct JOURNAL));
}


void SaveMsg60(WORD lenMsg){
  int fd;
  BYTE fileName[8 + 1];
  struct MSG60 msg60Tem;
  int len = 0;

  len = sizeof(struct MSG60);
  memset(fileName, 0x00, sizeof(fileName));
  memset(&msg60Tem, 0x00, sizeof(struct MSG60));
  strcpy(fileName, KMsg60File);


  memcpy( msg60Tem.msg, gMsgPOS, lenMsg);

  //  printf("\f ContTX %d", gAppDat.transactionCont);WaitKey(3000, 0);
  
  if(gAppDat.msg60Cont >= MAX_MSG){
    gAppDat.msg60Cont = 0;
    
  }

  fd = fOpenMW(fileName);
  if(fd < 0){
    printf("\fError al abrir msg60");WaitKey(3000, 0);
    return;
  }

  if( !WriteSFileSP(fd, (DWORD) &gMsg60Offset[gAppDat.msg60Cont], &msg60Tem, sizeof(struct MSG60)) ){
    printf("\Error De Escritura");WaitKey(3000, 0);
    return;
  }

  fCloseMW(fd);
  gAppDat.msg60Cont++;
  SaveDataFile( &gAppDat );
}


void ReadMsg60(int idFile, int idTrans, struct MSG60 *aDat){
  //int len = 0;
  //struct JOURNAL journalTem;

  //len = sizeof(struct JOURNAL);
  //printf("\f llego 3");APM_WaitKey(3000,0);
  //memset(&journalTem, 0x00, sizeof(journalTem));
  if(idTrans > MAX_MSG){
    printf("\fIndice msg60 Errado");WaitKey(3000, 0);
    return;
  }

  //fSeekMW(idFile, idTrans*len);
  //fReadMW(idFile, aJournalTem, sizeof(struct JOURNAL));
  ReadSFileSP(idFile, (DWORD) &gMsg60Offset[idTrans], aDat, sizeof(struct MSG60));
}



int PrintTransTest2(void){
    //BYTE DateTime[14];
    //BYTE DateTimeRec[14 + 1];
    BYTE stan[6 + 1];
    BYTE rrn[12 + 1];
    BYTE procCode[6 + 1];
    BYTE mti[4 + 1];
    BYTE fileName[8 + 1];
    int n;
    int fd;
    struct JOURNAL journalTem;

    memset(fileName, 0x00, sizeof(fileName));
    memset(mti, 0x00, sizeof(mti));
    memset(stan, 0x00, sizeof(stan));
    memset(rrn, 0x00, sizeof(rrn));
    memset(procCode, 0x00, sizeof(procCode));
    //RtcGetMW(DateTime);

    strcpy(fileName, KJournalFile);
    fd = fOpenMW(fileName);
    if (fd < 0)
    	printf("\fError al Abrir Journal");WaitKey(3000, 0);
    for (n = 0; n < MAX_JOURNAL; n++) // 5 es el maximo de transacciones (ver gMaxRec en el APM)
    {
    	ReadTransactionPichincha(fd, n, &journalTem);
        split( stan, journalTem.STAN, 3 );
        split( procCode, journalTem.procCode, 3 );
        sprintf(mti, "%04X", journalTem.msgType);
        memcpy(rrn, journalTem.RRN, 12);
        printf("\fSTAN: %s\n", stan);
        printf("MTI: %s\n", mti);
        printf("RRN: %s\n", rrn);
        printf("PROC CODE: %s\n", procCode);
        printf("MONTO: %d\n", journalTem.amount);
		//printf("FECHA: %04X\n", &journalTem.fechaTrans);
        printf("n = %d", n);
        WaitKey(3000, 0);
    }
    fCloseMW(fd);
  return TRUE;
}


int PrintMsg60Test2(void){
    
    struct MSG60 msg60Tem;
    int n = 0;
    int fd;
    BYTE fileName[8 + 1];
    
    memset(fileName,0x00,sizeof(fileName));
    strcpy(fileName, KMsg60File);
    fd = fOpenMW(fileName);
    if (fd < 0)
      printf("\fError al Abrir msg60");WaitKey(3000, 0);
    for (n = 0; n < MAX_MSG; n++) // 5 es el maximo de transacciones (ver gMaxRec en el APM)
    {
      ReadMsg60(fd, n, &msg60Tem);
      DispLineMW( "MENSAJES", MW_LINE1, MW_CENTER|MW_REVERSE|MW_CLRDISP|MW_SMFONT );
      DispClrBelowMW(MW_LINE2);
      printf("\n%s", msg60Tem.msg);  
      WaitKey(3000, 0);
    }
    fCloseMW(fd);
  return TRUE;
}




int PrintTrans(void){
	BYTE DateTime[14];
//	BYTE DateTimeRec[14];
	BYTE date[11];
	BYTE time[9];
	BYTE serial[9];
	BYTE fileName[8 + 1];
	BYTE mti[10];
	BYTE DateX[5];
	BYTE TimeX[7];
	//BYTE monto[10];
	int n;
    int fd;
    int endFor;
    struct JOURNAL journalTem;
    struct DISP_AMT disp_amt;

    memset(fileName, 0x00, sizeof(fileName));
    //memset(monto, 0x00, sizeof(monto));
    memset(mti, 0x00, sizeof(mti));
	memset(DateX, 0x00, sizeof(DateX));
	memset(TimeX, 0x00, sizeof(TimeX));
    memset(&journalTem, 0x00, sizeof(struct JOURNAL));

    if(gAppDat.journalFull == TRUE){
    	endFor = MAX_JOURNAL;
    }
    else{
    	endFor = gAppDat.transactionCont;
    }
    MsgBufSetup();

	pack_mem( MW_LPT_FONT_SMALL, 3 );
	pack_mem("\x1BW1********INFORME********\x1BW0", 29);
	pack_lf();
	pack_lf();

	memset(DateTime, 0x00, sizeof(DateTime));
	memset(time, 0x00, sizeof(time));
	memset(date, 0x00, sizeof(date));
	memset(serial, 0x00, sizeof(serial));

	RtcGetMW(DateTime);
	GetSysCfgMW( MW_SYSCFG_SERIAL_NO, serial );

	sprintf(date + 0, "%c%c/%c%c/%c%c%c%c", DateTime[6], DateTime[7], DateTime[4], DateTime[5],
			DateTime[0], DateTime[1], DateTime[2], DateTime[3]);
	sprintf(time + 0, "%c%c:%c%c:%c%c", DateTime[8], DateTime[9], DateTime[10], DateTime[11], DateTime[12],
			DateTime[13]);

   //	printf("\f llego 1");APM_WaitKey(3000,0);
	pack_space(13);
	pack_mem( date, 10 );
	pack_mem( " - ", 3 );
	pack_mem( time, 8 );
	pack_space(13);
	pack_lf();
	pack_space(16);
	pack_str( "Serial: " );
	pack_mem( serial, 8 );
	pack_space(16);
	pack_str( "Operador: " );
	pack_mem( gUsers_tbl.sb_user, strlen(gUsers_tbl.sb_user) );

	pack_lf();

	pack_nbyte( '=', 48 );
	strcpy(fileName, KJournalFile);
	fd = fOpenMW(fileName);
	if (fd < 0)
	{
		printf("\fError al Abrir Journal");WaitKey(3000, 0);
	}
	for (n = 0; n < endFor; n++)
	{
		ReadTransactionPichincha(fd, n, &journalTem);
		memset(DateX,0x00,sizeof(DateX));
		sprintf(DateX,"%02X%02X",journalTem.fechaTrans[0],journalTem.fechaTrans[1]);
		memset(TimeX,0x00,sizeof(TimeX));
		sprintf(TimeX,"%02X%02X%02X",journalTem.horaTrans[0],journalTem.horaTrans[1],journalTem.horaTrans[2]);
		//split( DateX, (BYTE*) &journalTem.fechaTrans, 4 );
		//split( TimeX, (BYTE*) &journalTem.horaTrans, 7 );	
	   //	split( DateTimeRec, (BYTE*) &RSP_DATA.s_dtg, 7 );	/// corregir

	   //	if( memcmp(DateTimeRec, DateTime, 8) != 0 )	 /// ajuste mes dia 
	   //		continue;
	   //	printf("\f |%s|",DateTime );APM_WaitKey(3000,0);
		if( memcmp(DateX, &DateTime[4], 4) != 0 )	 /// ajuste mes dia 
	   		continue;


		sprintf(time + 0, "%c%c:%c%c:%c%c", TimeX[0], TimeX[1], TimeX[2], TimeX[3], TimeX[4],
				TimeX[5]);			/// modificar hora del journal.

		pack_space(2);
		pack_mem( time, 8 );
		pack_space(2);

		split_data( journalTem.STAN, 3 );

		pack_space(2);

		pack_mem( journalTem.RRN, 12 );

		sprintf(mti, "%04X", journalTem.msgType);

		pack_space(2);

		pack_mem( mti, 4 );

		pack_lf();

		split_data( journalTem.procCode, 3 );

		pack_space(14);

		ConvAmount(journalTem.amount, &disp_amt, 2, '$');
		pack_mem(disp_amt.content, disp_amt.len);

		pack_lf();
	}
	fCloseMW(fd);	// Cierra el archivo JOURNAL
	PackFF();
	PackMsgBufLen();

	PrintBuf(MSG_BUF.sb_content, MSG_BUF.d_len, FALSE);
	MsgBufSetup();
	PackMsgBufLen();
	return TRUE;
}
  

 void setParamRemoteDownload(void){		 /// token 96 viajan datos de carga remota
	BYTE ip[4];
	BOOLEAN ret;

	memset(ip, 0x00, sizeof(ip));
	memcpy( ip, "\xBE\x92\x85\x9D", 4);					  
	//memcpy( ip, "\x0A\x00\x00\x2C", 4);

	T_RDLL_INF t_rdll_inf;

	memset(&t_rdll_inf, 0x00, sizeof(t_rdll_inf));

	//t_rdll_inf.b_link_type = 1;/// LAN
	t_rdll_inf.b_link_type = 2;/// GPRS
	t_rdll_inf.b_d_type = 0x00;
	t_rdll_inf.b_app_type = 0;
	memset(t_rdll_inf.s_app_id, 0x20, 32);
	t_rdll_inf.w_fport = atoi(gToken_96.puertoTeleCarga);
  	t_rdll_inf.d_fip = (gToken_96.ipTeleCarga[3] << 24) + (gToken_96.ipTeleCarga[2] << 16) + (gToken_96.ipTeleCarga[1] << 8) + gToken_96.ipTeleCarga[0];
   //	t_rdll_inf.d_fip = (gToken_96.ipTeleCarga[0] << 24) + (gToken_96.ipTeleCarga[1] << 16) + (gToken_96.ipTeleCarga[2] << 8) + gToken_96.ipTeleCarga[3];
	APM_WaitKey(100,0);
	
	if(gTcpIp.cxPrimaria == TRUE)
	strcpy(t_rdll_inf.s_gprs_apn, gTcpIp.sb_APN);
	else
	strcpy(t_rdll_inf.s_gprs_apn, gTcpIp.sb_APN2);
	
	ret = os_config_write(K_CF_RdllInf, &t_rdll_inf);
	ret = os_config_update();
	Delay1Sec(3,0);
	os_rdll_app();
}




//NOHORA 10Dic Desde aca
//*****************************************************************************
//  Function        : KeysInit
//  Description     : Init. Keys Data file.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void KeysInit(void)
{
	BYTE filename[32];
	struct TOKEN_97 token_97;

	strcpy(filename, KKeysFile);
	gKeysFd = fOpenMW(filename);
	if (gKeysFd < 0) {
		//DispLineMW("Create: EDCFile", MW_MAX_LINE, MW_CLREOL|MW_SPFONT);
		gKeysFd = fCreateMW(filename, FALSE);
		memset(&token_97, 0x00, sizeof(struct TOKEN_97));
		fWriteMW(gKeysFd, &token_97, sizeof(struct TOKEN_97));
		fCommitAllMW();
	}
	else
		fReadMW(gKeysFd, &token_97, sizeof(struct TOKEN_97));
	memcpy(&gtoken_97, &token_97, sizeof(struct TOKEN_97));
	fCloseMW(gKeysFd);
}


void SaveKeysFile(struct TOKEN_97 *aKeyTable)
{

	int fd;
	char filename[32];

	memset(filename, 0x00, sizeof(filename));
	strcpy(filename, KKeysFile);

	fd = fOpenMW(filename);

	if(fd < 0)
	{
		DispLineMW("KEYS File ERROR!", MW_MAX_LINE, MW_REVERSE|MW_CLREOL|MW_SPFONT);
		LongBeep();
		GetChar();
		return;
	}

	WriteSFile( fd, MW_FSEEK_SET, aKeyTable, sizeof(struct TOKEN_97));
	fCloseMW(fd);
}
//NOHORA 10Dic Hasta aca


void DispMsg60(void)
{
    struct MSG60 msg60Tem;
    
    int fd;
    BYTE fileName[8 + 1];
    
    memset(fileName,0x00,sizeof(fileName));
    strcpy(fileName, KMsg60File);
    fd = fOpenMW(fileName);
    if (fd < 0)
      printf("\fError al Abrir msg60");WaitKey(3000, 0);
   
    ReadMsg60(fd, gAppDat.msg60Cont - 1 , &msg60Tem);
    DispLineMW("MENSAJE", MW_LINE1, MW_SMFONT | MW_REVERSE | MW_CENTER);  
    printf("\n%s\n", msg60Tem.msg);  
    APM_WaitKey(3000,0);  
    
    fCloseMW(fd);
  
}


