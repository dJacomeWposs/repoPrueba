//-----------------------------------------------------------------------------
//  File          : input.h
//  Module        :
//  Description   : Declrartion & Defination for input.c
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
#ifndef _INPUT_H_
#define _INPUT_H_
#include "common.h"

extern BOOLEAN InCardTable(void);
extern BOOLEAN ValidCard(void);
extern DWORD PromptYesNo(void);
extern BOOLEAN GetAuthCode(void);
extern BOOLEAN GetAmount(BOOLEAN aTipsNeeded);
extern BOOLEAN GetExpDate(void);
extern BOOLEAN GetProductCode(void);
extern BOOLEAN Get4DBC(void);
//extern BOOLEAN WaitCardData(int aTxnType);
extern DWORD GetCardNbr(DWORD aFirstKey);
extern BOOLEAN GetCard(int aFirstKey, BOOLEAN aManualAllowed );
extern BOOLEAN GetEcrRef(void);
extern DWORD GetPassword( void );
extern BOOLEAN GetAmnt( WORD aLine );
extern BOOLEAN getPinblock(void);
extern BOOLEAN setCodigoEmpresa(DWORD fila, DWORD columna, BYTE min, BYTE max);
extern BOOLEAN setContrapartida(DWORD fila, DWORD columna, BYTE min, BYTE max);
extern BOOLEAN setRecarga(BYTE *titulo);
#endif //_INPUT_H_
