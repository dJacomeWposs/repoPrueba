//-----------------------------------------------------------------------------
//  File          : Others.c
//  Module        :
//  Description   : Include routines for other transactions.
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
#include "apm.h"
#include "sysutil.h"
#include "constant.h"
#include "corevar.h"
#include "tranutil.h"
#include "input.h"
#include "sale.h"
#include "saleoffl.h"
#include "auth.h"
#include "refund.h"
#include "void.h"
#include "adjust.h"
#include "settle.h"
#include "menu.h"
#include "emvtrans.h"
#include "others.h"

// Default Other Trans Menu
/*const struct MENU_ITEM KOtherTxnItem [] = {
  {  0, "Settlement"},
  {  1, "Void"},
  {  2, "Offline"},
  {  3, "Refund"},
  {  4, "Auth"},
  {  5, "Adjustment"},
  {-1, NULL},
};

const struct MENU_DAT KOtherTransMenu  = {
  "Trans Selection",
  KOtherTxnItem,
};*/

//*****************************************************************************
//  Function        : OtherTrans
//  Description     : Show transaction select screen & run user's choice.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
/*void OtherTrans(void)
{
  int select;
  int host_idx;

  select = 0;
  select = MenuSelect(&KOtherTransMenu, select);
  if (select == -1)
    return;

  switch (select) {
    case 0:
      host_idx = APM_SelectAcquirer(TRUE);
      if (host_idx != -1) {
        INPUT.w_host_idx = host_idx;
        SettleTrans();
      }
      break;
    case 1:
      VoidTrans(-1);
      break;
    case 2:
      if (!WaitCardData(SALE_OFFLINE))
        break;
      if (!ICCInserted())
        SaleOfflTrans(INPUT.b_entry_mode);
      break;
    case 3:
      if (!WaitCardData(REFUND))
        break;
      if (!ICCInserted())
        RefundTrans(INPUT.b_entry_mode);
      break;
    case 4:
      if (!WaitCardData(AUTH_SWIPE))
        break;
      if (!ICCInserted())
        AuthTrans(INPUT.b_entry_mode);
      else
        EMVTrans(AUTH_ICC);
      break;
    case 5:
      AdjustTrans(-1);
      break;
  }
}*/
