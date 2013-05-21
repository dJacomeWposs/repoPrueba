/*
 * toggle.h
 *
 *  Created on: 13/02/2012
 *      Author: joannume7
 */

#ifndef TOGGLE_H_
#define TOGGLE_H_

#include "common.h"


//-----------------------------------------------------------------------------
//     Option toggle Function
//-----------------------------------------------------------------------------
#define MAX_OPTION_LIST     16

#define TOGGLE_MODE_4LINE   0x000000
#define TOGGLE_MODE_8LINE   0x010000
#define TOGGLE_MODE_MASK    0xFF0000
#define OPTION_VAL_MASK     0x0000FF

extern const char *KInitMode[];
extern const char *KTrueFalse[];
extern const char *KDialMode[];
extern const char *KOnOff[];
extern const char *KEnable[];
extern const char *KAuxSpeed[];
extern const char *KAuxPort[];
extern const char *KPPPport[];
extern const char *KAuxCommMode[];
extern const char *KAuxDataLen[];

extern const char *KProtoMode[];
extern const char *KCcittMode[];
extern const char *KMdmSpeed[]   ;

extern char ToggleOption(const char *aMsg, const char **aOptTbl, DWORD aCurVal);



#endif /* TOGGLE_H_ */
