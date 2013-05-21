/*-----------------------------------------------------------------------------
 *  File          : keytrans.h
 *  Module        : LCPOS
 *  Description   : Declaration & Definition for keytrans.c
 *  Author        : John
 *  Notes         :
 *
 *  Date          Who         Action
 *  ------------  ----------- -------------------------------------------------
 *  19 Mar  2012  John        Port from CREON/PR608.
 *-----------------------------------------------------------------------------
 */
#ifndef _KEYTRANS_H_
#define _KEYTRANS_H_

//----------------------------------------------------------------------
//    Key Bank Index
//----------------------------------------------------------------------
/*#define MKEY_IDX            0
#define EKEY_IDX            1
#define DKEY_IDX            2*/

/*#define MKEY_IDX            0
#define EKEY_IDX            1
#define CARDKEY_IDX         2
#define DKEY_IDX            3*/

#define MKEY_IDX            1
#define EKEY_IDX            2
#define CARDKEY_IDX         3
#define DKEY_IDX            4


//----------------------------------------------------------------------
//    Key Inject Mode
//----------------------------------------------------------------------
#define KEYMODE_KVC         0x01      // with KVC
#define KEYMODE_ENC         0x00      // encryption key
#define KEYMODE_DEC         0x02      // decryption key
#define KEYMODE_MKEY        0x04      // master key
#define KEYMODE_SINGLE      0x08      // single key length
#define KEYMODE_DOUBLE		  0x10      // double key length
#define KEYMODE_TRIPLE		  0x18      // triple key length

extern void ResetKey(BYTE idx);
extern BOOLEAN LoadTmk(void);
extern void LoadKeys(void);
extern void EncData(void);
extern void DecData(void);
extern void SetMode(void);
extern BOOLEAN GetPIN(void);
extern BOOLEAN SetDhostIdx(void);
extern BOOLEAN ResetDukptIkey(void);
extern BOOLEAN LoadDukptIkey(void);
extern BOOLEAN GetDukptPin(void);
extern void IccTmkInject(void);
extern void SetMasterKey( void );
extern BOOLEAN key_ready(BYTE idx);

extern BOOLEAN LoadEWKPin( BYTE *aEWK);
extern BOOLEAN LoadEWKCard( BYTE *aEWK );
#endif //_KEYTRANS_H_

