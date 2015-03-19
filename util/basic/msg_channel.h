#ifndef _MSG_CHANNEL_H
#define _MSG_CHANNEL_H
#include <stdint.h>

#define DEFAULT_MSG_BUS_SIZE (10*1000*1000)

typedef struct
{
	char cFlag;
	unsigned short shLength;	//只是后面包体的长度
} MsgChannelPkgHead;

typedef struct
{
	volatile unsigned long lTotalRequestPkgs;
	
	volatile unsigned long lRequestHeadPos;
	volatile unsigned long lRequestTailPos;	

	volatile int lock;
	
	uint32_t dwBufSize;
	char caRequestBuffer[0];
} Msg_Channel_Queue;

int GetMsgChannelQueueShm(int iShmKey, Msg_Channel_Queue **pstIdx, int iFlag, int iBufSize);
int GetMsgChannelUsageRates( Msg_Channel_Queue *pstChannel,int *piUsageRates);
int AddPackageToMsgChannel(Msg_Channel_Queue *pstChannel, char *pBody, int iBodyLen);
int GetPackageFromMsgChannel(Msg_Channel_Queue *pstChannel, char *pBody, int *piBodyLen);
#endif
