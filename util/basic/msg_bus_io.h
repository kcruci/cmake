#ifndef __MSG_BUS_IO_H
#define __MSG_BUS_IO_H

#include <stdint.h>

#ifndef BUS_MSG_LEN
#define BUS_MSG_LEN (1024*10)
#endif

int ReadMsgBus(int iKey, void* pkg, int *dwLen, int iBufSize);
int ReadMsgBusByLock(int iKey, void* pkg, int *dwLen, int iBufSize);

int WriteMsgBus(int iKey, char* pPkg, uint32_t dwLen, int iBufSize);
int WriteMsgBusByLock(int iKey, char* pPkg, uint32_t dwLen, int iBufSize);

#endif
