//-----------------------------------------------------------------------------
//  File          : constant.h
//  Module        :
//  Description   : Declrartion & Defination for constant.c
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
#ifndef _CONSTANT_H_
#define _CONSTANT_H_
#include "common.h"

//-----------------------------------------------------------------------------
//    Defines
//-----------------------------------------------------------------------------

/*-----  BATCH RECORD TRANS STATUS BIT ----------------------*/
#define OFFLINE         0x80
#define ADJUSTED_OFF    0x40
#define ADJUSTED        0x20
#define VOIDED          0x10

/*-----  TRANSACTION TYPES  ---------------------------------*/
enum {
  AUTH_SWIPE              ,   //0
  AUTH_MANUAL             ,   //1
  SALE_SWIPE              ,   //2
  SALE_MANUAL             ,   //3
  ONL_REFUND_SW           ,   //4
  ONL_REFUND_MNL          ,   //5
  REFUND                  ,   //6
  SALE_OFFLINE            ,   //7
  SALE_COMPLETE           ,   //8
  SALE_UNDER_LMT          ,   //9
  VOID                    ,   //10
  ADJUST                  ,   //11
  TRANS_UPLOAD            ,   //12
  SETTLEMENT              ,   //13
  EDC_REV                 ,   //14
  SALE_ICC                ,   //15
  AUTH_ICC                ,   //16
  ESALE_UNDER_LMT         ,   //17
  ESALE_COMPLETE          ,   //18
  TEST_MSG                ,   //19
  INIT                    ,   //20
  RETIRO                  ,   //21
  CONFIRMACION_RETIRO     ,   //22
  CONSULTA_DEPOSITO		  ,	  //23
  DEPOSITO                ,   //24
  CONFIRMACION_DEPOSITO   ,   //25
  CONSULTA_PAGO_SERVICIO  ,   //26
  CONSULTA_PAGO_TARJETA	  ,   //27
  CONFIRMA_PAGO_TARJETA	  ,	  //28
  CONFIRMA_PAGO_SER_N	  ,	  //29
  EFECTIVACION_PAGO_TARJ  ,   //30
  PAGO_SERVICIO			  ,   //31
  CONFIRMA_PAGO_SERVICIO  ,   //32
  CAMBIO_CLAVE			  ,	  //33
  CONFIRMACION_PAGO_TARJ  ,   //34
  CONSULTA_PAGO_BONO      ,   //35
  EFECTIVACION_PAGO_BONO  ,	  //36
  CONFIRMACION_PAGO_BONO  ,   //37
  EMISION_CAPA_LOTES	  ,   //38
  CONSULTA_COMPROBANTE	  ,	  //39
  CONSULTA_ID_XPERTA	  ,	  //40
  EFECTIVACION_CREA_XPERTA,	  //41
  CONFIRMACION_CREA_XPERTA,	  //42
  CONS_REP_XPERTA		  ,	  //43
  EFECT_REP_XPERTA		  ,	  //44
  CONF_REP_XPERTA		  ,	  //45
  RESUME_TX          	  ,   //46
  REPRINT_DOC             ,   //47
  IDENTIF_BP              ,   //48
  CON_REC_DIST            ,   //49
  PAGO_DIST_EFECTIVO      ,   //50
  CONFIRMA_RECAUDO_DIST	  ,   //51
  RECARGA	     		  ,	  //52
  CONFIRMA_RECARGAS		  ,	  //54
  CONSULTA_VENTA_SEGURO	  ,	  //55
  CONSULTA_VENTA_SEGURO_V ,	  //56
  PAGO_VENTA_SEGURO		  ,	  //57
  CONFIRMA_VENTA_SEGURO   ,	  //58
  COMPRA_STOCK			  ,   //59
  CONF_COMPRA_STOCK		  ,   //60
  CONSULTA_COMISION		  ,   //61
  CONSULTA_COMISION_2	  ,   //62
  CONSULTA_SALDO		  ,   //63
  CONSULTA_ULT_MOV	      ,   //64
  CONSULTA_SALDOCNB       ,	  //65
  CONSULTA_ULT_MOV_CNB	  ,	  //66
  MAX_TXN_TYPE            ,
  DEFAULT_TRANS = 255     ,
};

struct TRANS_BITMAP {
  WORD w_txmsg_id;
  BYTE sb_txbitmap[8];
  BYTE b_proc_code2[3];
  BYTE b_proc_code;
  BYTE b_condition_code;
  BYTE b_type;
  BYTE b_sale_flag;  /* 0 not sale, 1 credit card sale, 2 debit sale, 3 redeem */
  BYTE b_status;
};

extern const struct TRANS_BITMAP KTransBitmap[MAX_TXN_TYPE];
extern const BYTE KSetlPCode2[3];
extern const BYTE KTransHeader[];

//-----------------------------------------------------------------------------
//      Functions Handle the data
//-----------------------------------------------------------------------------
extern BOOLEAN SaleType(BYTE aTrans);

#endif // _CONSTANT_H_

