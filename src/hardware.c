//-----------------------------------------------------------------------------
//  File          : hardware.c
//  Module        :
//  Description   : Include hardware handle routines.
//  Author        : Lewis
//  Notes         : Following Device cannot share by applications
//                  AUX
//                  MCOM
//                  MDM
//                  TCP
//                  UDP
//                  Timer
//                  (i.e. Application have to open themselves)
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
#include "hwdef.h"
#include "midware.h"
#include "apm.h"
#include "sysutil.h"
#include "message.h"
//#include "testcfg.h"
#include "hardware.h"

//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//      Globals
//-----------------------------------------------------------------------------
// Device Handle
int    gTimerHdl[MAX_TIMER_SUPPORT];
int    gMsrHandle;
int    gIccHandle;
int    gAux1Handle;
int    gAux2Handle;
int    gLptHandle;
int    gCdc1Handle;

//static BYTE     gMsrBuf[1024];
static BOOLEAN  bSMCLocked=FALSE;         // Flag indicate ICC inserted for transaction.

//-----------------------------------------------------------------------------
//      Constants
//-----------------------------------------------------------------------------

//*****************************************************************************
//  Function        : InitAllHw
//  Description     : Initialize all hardware drivers.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void InitAllHw(void)
{
  int i;
  
  gMsrHandle  = -1;
  gIccHandle  = -1;
  gAux1Handle = -1;
  gAux2Handle = -1;
  gLptHandle  = -1;
  gCdc1Handle = -1;


  gMsrHandle  = APM_GetHwHandle(APM_SDEV_MSR);
  gIccHandle  = APM_GetHwHandle(APM_SDEV_ICC);
  #if (CDC_SUPPORT)
  gCdc1Handle = APM_GetHwHandle(APM_SDEV_CDC);
  #endif

  for (i = 0; i < MAX_TIMER_SUPPORT; i++) {
    gTimerHdl[i] = TimerOpenMW();
  }
}
//*****************************************************************************
//  Function        : CloseAllHw
//  Description     : Close All hardware driver.
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void CloseAllHw(void)
{
  int i;

  if (gAux1Handle != -1)
    CloseMW(gAux1Handle);
  if (gAux2Handle != -1)
    CloseMW(gAux2Handle);

  for (i = 0; i < MAX_TIMER_SUPPORT; i++) {
    TimerCloseMW(gTimerHdl[i]);
  }
}
//*****************************************************************************
//  Function        : WaitKey
//  Description     : Wait a key for specify time.
//  Input           : timeout;   // timeout value in second
//                    aCheckList
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
DWORD WaitKey(DWORD aTimeout, DWORD aCheckList)
{
  DWORD keyin;
  DWORD icc_id;

  icc_id = 0;
  APM_SetKbdTimeout(aTimeout);
  while ((keyin=APM_GetKeyin())==0) {

    if ((aCheckList & WAIT_ICC_INSERT) && (gIccHandle != -1)) {
      if (ICCInsertedMW()&0x01) {
        keyin = WAIT_ICC_INSERT;
        break;
      }
    }

    if ((aCheckList & WAIT_ICC_REMOVE) && (gIccHandle != -1)) {
      if (ICCInsertedMW()&0x01) {
        keyin = WAIT_ICC_REMOVE;
        break;
      }
    }

    if ((aCheckList & WAIT_MSR_READY) && (gMsrHandle != -1)) {
      if (IOCtlMW(gMsrHandle, IO_MSR_RXLEN, NULL) > 0) {
        keyin = WAIT_MSR_READY;
        break;
      }
    }

//    if ((aCheckList & WAIT_AUX1_READY) && (gAux1Handle != -1)) {
//      if (IOCtlMW(gAux1Handle, IO_AUX_RX_RDY, NULL) > 0) {
//        keyin = WAIT_AUX1_READY;
//        break;
//      }
//    }

//    if ((aCheckList & WAIT_AUX2_READY) && (gAux1Handle != -1)) {
//      if (IOCtlMW(gAux2Handle, IO_AUX_RX_RDY, NULL) > 0) {
//        keyin = WAIT_AUX2_READY;
//        break;
//      }
//    }

    SleepMW();
  }
  return(keyin);
}
//*****************************************************************************
//  Function        : MSRSwiped
//  Description     : Check for MSR data.
//  Input           : aTrk1;            // pointer to track1 buffer.
//                    aTrk2;            // pointer to track2 buffer.
//  Return          : TRUE/FALSE;       // TRUE=>Card Swiped
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN MSRSwiped(struct MW_TRK1BUF *aTrk1, struct MW_TRK2BUF *aTrk2)
{
  WORD status;
  BYTE len;
  BYTE c;
  DWORD msr_len, cur_len = 0;
  BYTE msr_buf[1024];

  if (IOCtlMW(gMsrHandle, IO_MSR_RXLEN, NULL) <= 0)
    return FALSE;

  aTrk1->b_len = 0;
  aTrk2->b_len = 0;
  status = IOCtlMW(gMsrHandle, IO_MSR_STATUS, NULL);
  memset(msr_buf, 0, sizeof(msr_buf));
  msr_len = ReadMW(gMsrHandle, &msr_buf, sizeof(msr_buf));
  if (((status & MW_TRACK2) == 0x00) && (msr_len > 0)) {
    IOCtlMW(gMsrHandle, IO_MSR_RESET, NULL);
    return TRUE;
  }

  cur_len++;
  len = 0;
  if (status & MW_TRACK2) {
    while ((c=msr_buf[cur_len++]) != END_SENTINEL) {
      aTrk2->sb_content[len++] = c;
    }
  }
  else {
    cur_len++;
  }
  aTrk2->b_len = len;
  cur_len++;
  len = 0;
  if (status & MW_TRACK1) {
    while ((c=msr_buf[cur_len++]) != END_SENTINEL) {
      aTrk1->sb_content[len++] = c;
    }
  }
  else {
    cur_len++;
  }

  aTrk1->b_len = len;
  return TRUE;
}
//*****************************************************************************
//  Function        : ICCInserted
//  Description     : Check for ICC inserted.
//  Input           : TRUE;
//                    FALSE;
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ICCInserted(void)
{
  DWORD icc_id;

  icc_id = 0;
  if (ICCInsertedMW()&0x01) {
    IOCtlMW(gIccHandle, IO_ICC_SELECT, &icc_id);
    if (bSMCLocked == FALSE) {
      bSMCLocked = TRUE;
      return TRUE;
    }
  }
  else
    if (bSMCLocked) {
      bSMCLocked = FALSE;
    }
  return FALSE;
}
//*****************************************************************************
//  Function        : ICCReaderRdy
//  Description     : Check for ICC reader ready for use.
//  Input           : TRUE/FALSE;// FALSE => ICC not remove since last inserted
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
BOOLEAN ICCReaderRdy(void)
{
  DWORD icc_id;

  icc_id = 0;
  if ((ICCInsertedMW()&0x01)==0) {
    if (bSMCLocked) {
      bSMCLocked = FALSE;
    }
    return TRUE;
  }
  return (bSMCLocked == FALSE);
}
//*****************************************************************************
//  Function        : WaitICCRemove
//  Description     : Wait user to pull out the ICC
//  Input           : N/A
//  Return          : N/A
//  Note            : N/A
//  Globals Changed : N/A
//*****************************************************************************
void WaitICCRemove(void)
{
  DWORD icc_id;

  icc_id = 0;
  TimerSetMW(gTimerHdl[TIMER_GEN], 0);
  while (ICCInsertedMW()&0x01) {
    SleepMW();
    if (TimerGetMW(gTimerHdl[TIMER_GEN]) == 0) {
      LongBeep();
      TimerSetMW(gTimerHdl[TIMER_GEN], TIMER_1SEC);
    }
  }
}
////*****************************************************************************
////  Function        : WaitICCRemove
////  Description     : Wait user to pull out the ICC
////  Input           : N/A
////  Return          : N/A
////  Note            : N/A
////  Globals Changed : N/A
////*****************************************************************************
//void WaitDebugICCRemove(void)
//{
//  DWORD icc_id;
//  if (DebugAutoRun()&&!DebugEnd()) {
//    if (bSMCLocked) {
//      bSMCLocked = FALSE;
//    }
//    return;
//  }
//
//  icc_id = 0;
//  TimerSetMW(gTimerHdl[TIMER_GEN], 0);
//  while ((GetCharMW()!=MWKEY_ENTER)&&(ICCInsertedMW()&0x01)) {
//    SleepMW();
//  }
//  if (bSMCLocked) {
//    bSMCLocked = FALSE;
//  }
//}
//*****************************************************************************
//  Function        : PowerOnICC
//  Description     : Power ON ICC 0.
//  Input           : N/A
//  Return          : IC Card Protocol. 0=>Err
//  Note            : T1000 implement sam ownership.=>EMVDLL have to handle it.
//  Globals Changed : N/A
//*****************************************************************************
DWORD PowerOnICC(void)
{
  BYTE tmp[256];

  #ifdef EMV_ICCOn
  return EMV_ICCOn(tmp);
  #else
  return IOCtlMW(gIccHandle, IO_ICC_ON, tmp);
  #endif
}
//*****************************************************************************
//  Function        : PowerOffICC
//  Description     : Power OFF ICC.
//  Input           : N/A
//  Return          : N/A
//  Note            : T1000 implement sam ownership.=>EMVDLL have to handle it.
//  Globals Changed : N/A
//*****************************************************************************
DWORD PowerOffICC(void)
{
  #ifdef EMV_ICCOff
  return EMV_ICCOff();
  #else
  return IOCtlMW(gIccHandle, IO_ICC_OFF, NULL);
  #endif
}
