//-----------------------------------------------------------------------------
//  File          : corevar.h
//  Module        :
//  Description   : Declrartion & Defination for corevar.c
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
#ifndef _COREVAR_H_
#define _COREVAR_H_
#include "emv2dll.h"
#include "ecrdll.h"
#include "apm.h"
#include "hardware.h"
#include "sysutil.h"

extern BYTE gIpAddr[4];
extern WORD gPort;

extern BOOLEAN gRecaudoVar;
extern BOOLEAN gPagoTarjeta;
extern BOOLEAN gMsg60Show;
extern BOOLEAN flagFtpManual;	//OJO NOHORA 12 - MAR 2013//

extern int gShowMsgInit;
extern BYTE gTypeAcount;
extern BYTE gMsgInit[100];
extern BYTE gMsgPOS[200];
extern BOOLEAN gTerminalInit;
extern BYTE gCuenta[3];
extern BYTE gAmount[13];
extern BYTE glocAmount[6];
extern BYTE tmpAmount[13];
extern BYTE pinBlock[17];
extern BYTE pinBlock2[17];
extern BYTE NSU[13];
extern BYTE gRrnTemporal[12 + 1];
extern BYTE *ptrFacturas;
//extern BYTE gMensajeError[80];
extern BOOLEAN gMultiCuenta;
extern BOOLEAN gTeleCarga;
extern DWORD lenCupon;
extern BYTE g_ip[ 15 + 1];
extern BYTE g_ip1;
extern BYTE g_ip2;
extern BYTE g_ip3;
struct JOURNAL *gJournalOffset;

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

#define CX_TCP	FALSE	// Daniel Jacome 19/02/2013  Con este flag se habilita la comunicacion TCP para pruebas locales en Quito si esta en FALSE el POS sera GPRS
#define JTRACE	FALSE	// Daniel Jacome 27 de Febrero de 2013 si este flag esta en TRUE, la comunicacion se hara por puerto serie para tomar LOGS con el JTRACE

#define HANDLE_HOST_TYPE  0x0B      // Amex Host

#define COMMON_DATA                    \
    BYTE            b_trans;           \
    BYTE            b_trans_status;    \
    BYTE            sb_trace_no[3];    \
    BYTE            b_acc_ind;         \
    BYTE            sb_roc_no[3];      \
    BYTE            sb_pan[10];        \
    BYTE            sb_exp_date[2];    \
    DDWORD          dd_amount;         \
    DDWORD          dd_tip;            \
    DDWORD          dd_org_amount;     \
    BYTE            sb_product[4];     \
    BYTE            b_entry_mode;      \
    BYTE            sb_holder_name[26];\
    BYTE            sb_ecr_ref[16];    \
    BYTE            sb_auth_code[6];   \
    WORD            w_rspcode;         \
    BYTE            b_PinVerified;     \
    BYTE            sb_rrn[12];        \
    WORD            w_host_idx;        \
    WORD            w_issuer_idx;      \
    BYTE			sb_serialTerm[8];       \
    BYTE            sb_operador[11];     \
    BYTE            sb_typeAccount[3];   \
    BYTE            sb_cedula_ruc[16];   \
    BYTE            sb_num_cuenta[14];   \
    BYTE            sb_costo_serv[9];    \
    BYTE            sb_cod_empresa[11];  \
    BYTE            sb_contrapar[35];    \
    BYTE 			sb_tipotarjeta[3];  \
    BYTE 			sb_numtarjeta[43];  \
    WORD 			Msgtype;		\
    BYTE 			procCode[3];	\
    BYTE 			montorev[13];	\
    struct DATETIME s_dtg;             \
    struct ICC_DATA s_icc_data;        \
 
//-----------------------------------------------------------------------------
//      Transaction ENTRY mode
//-----------------------------------------------------------------------------
#define MANUAL            'M'
#define SWIPE             'S'
#define ICC               'I'
#define MANUAL_4DBC       'm'  // manual with 4DBC
#define SWIPE_4DBC        's'  // swipe with 4DBC
#define FALLBACK          'F'  // Swipe during ICC not available
#define FALLBACK_4DBC     'f'  // Swipe during ICC with 4DBC

//-----------------------------------------------------------------------------
//      Common element for struct
//-----------------------------------------------------------------------------
struct ICC_DATA {
  BYTE sb_label[16];
  BYTE b_aid_len;
  BYTE sb_aid[7];
  BYTE sb_tag_95[5];
  BYTE sb_tag_9b[2];
  BYTE sb_tag_9f26[8];
  WORD w_misc_len;
  BYTE sb_misc_content[280];
};

struct TXN_RECORD {       // Transaction record
  COMMON_DATA
  WORD                  w_crc;
};

struct ACQUIRER_REV {     // Reversal data
  COMMON_DATA
};

struct PRN_DATA {         // Print data
  COMMON_DATA
  BYTE                  b_type;
  DDWORD                dd_base_amount;
};

struct TOTAL {            // Total
  WORD                  w_count;
  DDWORD                dd_amount;
};

struct TOTAL_STRUCT {     // Transaction totals
  DWORD                 d_adjust_count;
  struct TOTAL          s_sale_tot;
  struct TOTAL          s_tips_tot;
  struct TOTAL          s_refund_tot;
  struct TOTAL          s_void_sale;
  struct TOTAL          s_void_refund;
};

struct STIS_DATA {        // STIS data
  struct TERM_CFG       s_term_cfg;
  struct DESC_TBL       s_desc_tbl[16];  // !TODO
  struct ACQUIRER_TBL   s_acquirer_tbl[2];
  struct ISSUER_TBL     s_issuer_tbl[2];
  struct CARD_TBL       s_card_tbl[2];
};

struct BS_DATE {          // Business date
  BYTE                  b_mm;
  BYTE                  b_dd;
};

//-----------------------------------------------------------------------------
//      Global Data Structure
//-----------------------------------------------------------------------------
#define MSG_BUF_LEN        3*1024
struct MSG_BUF {
  DWORD d_len;
  BYTE sb_content[MSG_BUF_LEN];
};

#define MAX_TAG_LIST   150
struct GDS {
  // Add Global Variables here
  struct STIS_DATA    s_stis_data;
  struct TERM_DATA    s_term_data;
  //struct SELECTMENU   select_menu[NBR_ACQUIRER+1];
  struct TXN_RECORD   s_record_buf;
  struct ACQUIRER_REV s_acq_rev;
  struct PRN_DATA     s_p_data;
  struct TOTAL_STRUCT s_term_tot;
  struct MSG_BUF      s_MsgBuf;
  struct COMMBUF      s_TxBuf;
  struct COMMBUF      s_RxMsg;
  BYTE   sb_IOBuf[1500];
  sIO_t  s_EMVIn;
  sIO_t  s_EMVOut;
  TLIST  s_TagList[MAX_TAG_LIST];
  BOOLEAN b_disp_chgd;
  struct BS_DATE s_bs_date;
  struct ECRDATA s_ecr_data;
  int    i_ecr_len;
};
extern struct GDS *gGDS;

#define STIS_TERM_CFG     gGDS->s_stis_data.s_term_cfg
#define STIS_ACQ_TBL(x)   gGDS->s_stis_data.s_acquirer_tbl[x]
#define STIS_ISS_TBL(x)   gGDS->s_stis_data.s_issuer_tbl[x]
#define STIS_CARD_TBL(x)  gGDS->s_stis_data.s_card_tbl[x]
#define STIS_TERM_DATA    gGDS->s_term_data

#define RECORD_BUF        gGDS->s_record_buf
#define P_DATA            gGDS->s_p_data
#define TERM_TOT          gGDS->s_term_tot
#define RX_BUF            gGDS->s_RxMsg
#define TX_BUF            gGDS->s_TxBuf
#define MSG_BUF           gGDS->s_MsgBuf



#define KADMINISTRADOR  0
#define KESPECIALISTA   1
#define KOPERADOR       2

#define MAX_JOURNAL		200
#define MAX_MSG         5
						
//-----------------------------------------------------------------------------
//      Protected RAM area structure
//-----------------------------------------------------------------------------
struct APPDATA {
	//BYTE 	timeTrans[3];
	//BYTE  	dateTrans[2];
	BYTE    auto_auth_code[3];
	BYTE 	sb_trace_no[3];
	WORD    reversal_count;
	WORD	transactionCont;	// Contador de transacciones
	BOOLEAN journalFull;		// TRUE -> El Journal se lleno y volvio a comenzar
  WORD  msg60Cont;
};
extern struct APPDATA gAppDat;



struct REVTABLE {	// tabla para almacenar los reversos de la aplicacion pichincha
	WORD	MTI;
	BYTE 	procCode[3];
	BYTE 	STAN[3];
	BYTE 	timeTrans[3];
	BYTE  	dateTrans[2];
	BOOLEAN	enableReverse;
  BYTE r_User[15];
};
extern struct REVTABLE gRevTable;



struct TCPIP
{
  BYTE sb_ip[4];
  WORD w_port;
  BYTE sb_ip2[4];
  WORD w_port2;
  BYTE sb_APN[33];
  BYTE sb_UserName[17];
  BYTE sb_Password[17];
  BYTE sb_DialNum[17];
  BYTE sb_APN2[33];
  BYTE sb_UserName2[17];
  BYTE sb_Password2[17];
  BYTE sb_DialNum2[17];
  BYTE commMode;
  BYTE cxPrimaria;
  //struct MW_TCP_PORT s_TcpIp;
};
extern struct TCPIP gTcpIp;


//OJO NOHORA 12 - MAR 2013//
struct FTPPARAM
{
	BYTE ipHost[4];
	WORD portFtp;
	BYTE userID[16];
	BYTE Password[16];
};
extern struct FTPPARAM gFtpParam;
//OJO NOHORA 12 - MAR 2013//


struct JOURNAL
{
	WORD msgType;
	BYTE procCode[3];
	BYTE STAN[3];
	BYTE RRN[12];
	BYTE horaTrans[3];
	BYTE fechaTrans[2];
	DDWORD amount;

};
extern struct JOURNAL gJournal;

struct MSG60
{

BYTE msg[200];
};
extern struct MSG60 gMsg60[MAX_MSG];


struct USERS_TBL
{
  BYTE sb_user[15];
  BYTE sb_pass[11];
  BYTE b_type;
  BYTE b_attempts;
  BOOLEAN b_activo;
};
extern struct USERS_TBL gUsers_tbl;
						




struct TOKEN_80
{
    BYTE timeMaxConnect[2];	// Tiempo maximo que debe estar conectado
    BYTE timeMaxWait[2];	// Tiempo maximo que debe esperar respuesta
    BYTE maxRetiros;		// Numero de retiros permitidos en un dia
    BYTE valMinRetiro[2];	// Valor minimo permitido para retiros
    BYTE valMaxRetiro[2];	// Valor maximo permitido para retiros
    BYTE maxDepositos;		// Numero de depositos permitidos en un dia
    BYTE valMinDeposito[2];	// Valor minimo permitido para depositos
    BYTE valMaxDeposito[2];	// Valor maximo permitido para depositos
    BYTE costoServicio[2];	// Costo del servicio
};
extern struct TOKEN_80 gToken_80;

struct INFO_2
{
    BYTE abrePro[3];
    BYTE verAplicativo[5];
    BYTE verIso[5];
};
extern struct INFO_2 gInfo_2;



struct TOKEN_6
{
    BYTE nomEmpresa[20];
    BYTE nomPersona[20];
    BYTE fecha_contable[4];
    BYTE valor_documento[6];
    BYTE valor_descuento[6];
    BYTE valor_cuenta[6];
    BYTE costo_servicio[6];
    BYTE valor_multa[6];
    BYTE valor_intereses[6];
    BYTE valor_adeudado[6];
};
extern struct TOKEN_6 gToken_6;// Total bytes del token = 86

struct TOKEN_7
{
    //BYTE cod_retorno[7];
    //BYTE msg_corto[20];
    BYTE msg_largo[60];
};
extern struct TOKEN_7 gToken_7;

struct TOKEN_8
{
   DWORD flag_more;
   BYTE cupon[2000];
};
extern struct TOKEN_8 gToken_8;

// DATOS DE LA CUENTA
struct TOKEN_11
{
    BYTE banco_credito[2];
    BYTE agencia_credito[3];
    BYTE cuenta_credito[7];
    BYTE digito_credito[2];
    BYTE nombre_credito[32];
};
extern struct TOKEN_11 gToken_11;


// DATOS PAGO DE BONO
struct INFO_12
{
    BYTE cedula[12];
};
extern struct INFO_12 gInfo_12;


// DATOS TIPO DE PAGO
struct INFO_13
{
    BYTE tipoPagoBono;
};
extern struct INFO_13 gInfo_13;

// DATOS ESPECIALISTA BP
struct TOKEN_15
{
    BYTE nombre[50];
    BYTE cargo[30];
    BYTE cedula[5];
};
extern struct TOKEN_15 gToken_15;

// DEPOSITOS
struct TOKEN_47
{
    BYTE cedula_ruc[8];
    BYTE nombre[30];
	BYTE cuenta[7];
    /*BYTE total_dinero[12];
    BYTE total_cheque[12];
    BYTE cant_cheques[2];
    BYTE codigo[20];*/
};
extern struct TOKEN_47 gToken_47;

// CUUENTA EXPERTA
struct TOKEN_48
{
    BYTE numero_kit[5];
    BYTE numero_cel[6];
	BYTE nombre[30];
    /*BYTE total_dinero[12];
    BYTE total_cheque[12];
    BYTE cant_cheques[2];
    BYTE codigo[20];*/
};
extern struct TOKEN_48 gToken_48;



// DATOS CONSULTA DE PAGO DE BONO
struct TOKEN_14
{
    BYTE nombre[50];
    BYTE valor[6];
    BYTE periodo[4];
    BYTE periodoFin[4];
    BYTE tipo_sub[15];
    BYTE informacion[60];
};
extern struct TOKEN_14 gToken_14; //Total bytes del token 14 = 139






// FLAGS PARA LA RECAUDACION
struct TOKEN_39
{
    BYTE flagVarFijo;
    BYTE flagUsoFuturo1;
    BYTE flagUsoFuturo2;
};
extern struct TOKEN_39 gToken_39;



struct TOKEN_91
{
    BYTE fechaHoraInicial[14+1];
    BYTE fechaHoraFinal[14+1];
	BYTE codigoProducto[6];
};
extern struct TOKEN_91 gToken_91;	//Total bytes del token 91 = 28



struct TOKEN_96
{
    BYTE fechaTeleCarga[4];
    BYTE horaTeleCarga[3];
    BYTE ipTeleCarga[4];
    BYTE puertoTeleCarga[5];
};
extern struct TOKEN_96 gToken_96;	//Total bytes del token 96 = 16



// INFORMACION DE CUENTAS
struct INFO_98
{
    BYTE cantCuentas[2];
    BYTE cuentas[19];
};
extern struct INFO_98 gInfo_98[5];
extern struct INFO_98 gInfo_98_pack;

// VERSION
struct TOKEN_1
{
    BYTE version[5];
};
extern struct TOKEN_1 gToken_1;


struct TOKEN_3PSP	// Para pago de servicios
{
    BYTE codEmpresa[10];
    BYTE contrapartida[34];
    BYTE modPago;
    BYTE modEnt;
    BYTE valorDeducciones[6];
    BYTE valorIncremento[6];
};
extern struct TOKEN_3PSP gToken_3psp;


struct TOKEN_3PTC	// Para pago de tarjeta credito
{
    BYTE tipoTarjeta[2];
    BYTE numTarjeta[42];
    BYTE modPago;
    BYTE modEnt;
    BYTE valorDeducciones[6];
    BYTE valorIncremento[6];
};
extern struct TOKEN_3PTC gToken_3ptc;



struct TOKEN_81
{
	BYTE fecha [7];
	BYTE flagServer;
	BYTE lenMsg;
	BYTE msgInit [100];
};
extern struct TOKEN_81 gToken_81;


struct TOKEN_97
{
	BYTE worKeyPin [16];
	BYTE worTrack2[16];

};
extern struct TOKEN_97 gtoken_97;

struct FacturaEmp
{
  BYTE fecha[10];
  BYTE factura[20];
  BYTE valor[12];
};




struct TOKEN_5{
  BYTE nomEmpresa[20];
  BYTE nomPersona[20];
  BYTE costServ[2];
  BYTE cantFacturas;
  struct FacturaEmp facturaEmp[20];
};

/*struct TOKEN_5_PACK{
  BYTE nomEmpresa[20];
  BYTE nomPersona[20];
  BYTE costServ[2];
  BYTE cantFacturas;
  struct FacturaEmp facturaEmp;
};*/

extern struct TOKEN_5 gToken_5;
extern struct TOKEN_5 gToken_5_pack; // Variable usada para enviar el token 5 al host






//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern void DataFileInit(void);
extern void DataFileUpdate(void);
extern void DataFileClose(void);
extern BOOLEAN CorrectHost(BYTE aHostType);
extern void GenAuthCode(BYTE *aAuthCode);
extern void SetDefault(void);

extern void MsgBufSetup(void);
extern void PackMsgBufLen(void);
extern void TxBufSetup(BOOLEAN aAdd2ByteLen);
extern void PackTxBufLen(BOOLEAN aAdd2ByteLen);
extern void RxBufSetup(BOOLEAN aAdd2ByteLen);
extern void PackRxBufLen(BOOLEAN aAdd2ByteLen);

extern int SearchRecord(BOOLEAN aIncVoidTxn);
extern BOOLEAN ValidRecordCRC(WORD aIdx);
extern void RefreshDispAfter(DWORD aSec);

extern void FreeGDS(void);
extern BOOLEAN MallocGDS(void);
extern BOOLEAN TrainingModeON(void);
extern void IncAPMTraceNo(void);

//************FUNCIONES PICHINCHA**************************************/
extern void TCPIPInit(void);
extern void FTPInit(void);	//OJO NOHORA 12 - MAR 2013//
extern void MemoriaInit(void);
extern BOOLEAN WriteSFileSP(DWORD aFileHandle, DWORD aOffset, const void *aBuf, DWORD aLen);
extern BOOLEAN WriteSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen);
extern void UsersInit(void);
extern BOOLEAN SetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat);
//extern BOOLEAN GetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat);
extern void SaveDataFile(struct APPDATA *aApp_data);
extern int GetUsersCount(void);
extern BOOLEAN GetUsersTbl(DWORD aIdx, struct USERS_TBL *aDat);
extern BOOLEAN ReadSFileSP(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen);
extern BOOLEAN UpdUsersTbl(DWORD aIdx, struct USERS_TBL *aDat);
extern void setCommMode(BYTE mode);
extern int GetTcpIp(struct TCPIP *aTcpIp);
extern void SaveTcpIp(struct TCPIP *aTcpIp);
extern int GetFtpParam(struct FTPPARAM *aFtpParam);		//OJO NOHORA 12 - MAR 2013//
extern void SaveFtpParam(struct FTPPARAM *aFtpParam);	//OJO NOHORA 12 - MAR 2013//
extern BOOLEAN SetPort( BYTE aPort, DWORD aLineNo);
extern BOOLEAN SetIPAddr( BYTE aServer, DWORD aLineNo);
BOOLEAN ReadSFile(DWORD aFileHandle, DWORD aOffset, void *aBuf, DWORD aLen);
extern int GetDataFile(struct APPDATA *aAppData);
extern void SaveReverseFile(struct REVTABLE *aRevTable);
extern void RevTableInit(void);
extern int getReverseFile(void);
extern void ModoConexion (BOOLEAN conexion);
extern void ConexPerson (void);
extern void JournalFileInit(void);
extern void Msg60FileInit(void);
extern void ConfigOpr (BYTE *title, int opr);
extern int PrintTrans(void);
extern int PrintMsg60Test2(void);

extern void SaveTransactionPichincha(void);
extern void ReadTransactionPichincha(int idFile, int idTrans, struct JOURNAL *aDat);
extern void SaveMsg60(WORD len);
extern void ReadMsg60(int idFile, int idTrans, struct MSG60 *aDat);

extern  void setParamRemoteDownload(void);

extern BYTE gAuxStan[12 + 1];


extern void KeysInit(void);		//NOHORA 10Dic
extern void SaveKeysFile(struct TOKEN_97 *aKeyTable); 	//NOHORA 10Dic
extern void DispMsg60(void);
extern int PrintMsg60Test2(void);


#endif // _COREVAR_H_




