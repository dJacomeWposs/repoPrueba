//-----------------------------------------------------------------------------
//  File          : usbkbdtest.h
//  Module        :
//  Description   :
//  Author        : Kenneth
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
//  9 Sep 2011    Kenneth  Initial Version.
//-----------------------------------------------------------------------------
#ifndef _INC_USBKBD_H_
#define _INC_USBKBD_H_
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <string.h>
#include "common.h"
#include "system.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
typedef struct{
	BYTE titulo[26];	/// Titulo de la ventana de captura
	BYTE label[26];		/// label de la captura
	int  contChar;			/// Contador de caracteres
	//BYTE fila;				/// Fila donde se desea hacer la captura
	BYTE colIni;			/// Columna donde se iniciara la captura
	BYTE colFin;			/// Columna donde debe finalizar la captura
	BYTE minChar;			/// Minimo de caracteres que se deben capyurar
	int  maxChar;			/// Maxino de caracteres a capturar
	int  scroll;			/// Controla el scroll horizontal de los datos capturados
}miCapturaExtKeyboard;

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
miCapturaExtKeyboard miCapturaExt;
//-----------------------------------------------------------------------------
//      Constant
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//      Prototypes
//-----------------------------------------------------------------------------
extern void UsbkbdTest(void);
extern void closeExtKeyBoard(int idPort);
extern BOOLEAN openExtKeyboard(int *idPort);
extern BOOLEAN getSExtKeyboard(int idPort, BYTE *buffer, BYTE *auxCed_kbdbuff);	//NOHORA 23Nov agregue el param 3 a esta funcion
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif // _INC_USBKBD_H_
