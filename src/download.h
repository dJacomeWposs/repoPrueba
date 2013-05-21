#ifndef _INC_DOWNLOAD_H__
#define _INC_DOWNLOAD_H__

#include "common.h"

extern const BYTE KAgentId[8];
extern const BYTE KDnldKey[16];
extern const BYTE *KTempFile;

extern void DownloadLocal(void);
extern BOOLEAN DownloadFtp(void);
extern int DownloadMain(void);
extern int DownloadAuth(void);
extern int DownloadWrite(BYTE *aFilename);

#endif // _INC_DOWNLOAD_H__

