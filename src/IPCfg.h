/*
 * IPCfg.h
 *
 *  Created on: 13/02/2012
 *      Author: Jorge Numa
 */

#ifndef IPCFG_H_
#define IPCFG_H_

#include "common.h"
#include "midware.h"

//-----------------------------------------------------------------------------
//  Common Defines
//-----------------------------------------------------------------------------
extern struct MW_TCP_PORT gTcpPort;
//-----------------------------------------------------------------------------
//  Functions
//-----------------------------------------------------------------------------
extern void DispTermIP(BYTE *aIP);
extern void ConfigTermIP(void);
extern void ToggleDHCP(void);
extern void TcpCfg(void);
extern WORD ConnectTcpip(BOOLEAN aWaitReady, BOOLEAN aNoCancel);
extern void Server1Cfg(void);
extern void Server2Cfg(void);
extern void GprsCfg( BYTE aPriOrSec );
#endif /* IPCFG_H_ */
