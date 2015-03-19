#include <stdio.h>
#include "common.h"
#include "ids_basic_shm.h"
#include "msg_channel.h"

////////////////////////////////////////////////////////////////////////////////////
// get channel share memory
////////////////////////////////////////////////////////////////////////////////////
static inline int GetMsgChannelTotalSize(int iBufSize)
{
	return sizeof(Msg_Channel_Queue)+iBufSize;
}

static int BZeroMsgBus(Msg_Channel_Queue *pstChannel)
{
	if(NULL == pstChannel)
		return -1;

	pstChannel->lTotalRequestPkgs = 0;
	pstChannel->lRequestHeadPos = 0;
	pstChannel->lRequestTailPos = 0;
	pstChannel->lock = 0;
	memset(pstChannel->caRequestBuffer, 0, sizeof(pstChannel->dwBufSize));

	return 0;
}

int GetMsgChannelQueueShm(int iShmKey, Msg_Channel_Queue **pstIdx, int iFlag, int iBufSize)
{
	char* sShm;
	int iShmSize = 0;
	uint32_t *pdwBufSize = 0;
	
	iShmSize = GetMsgChannelTotalSize(iBufSize);
	if(0 > iShmSize)
		return -1;

	if (!(sShm = GetShm(iShmKey, iShmSize, iFlag & (~IPC_CREAT)))) 
	{
		if (!(iFlag & IPC_CREAT)) return -2;
		if (!(sShm = GetShm(iShmKey, iShmSize, iFlag))) 
			return -3;
		
		memset(sShm, 0, iShmSize);
		pdwBufSize = &(((Msg_Channel_Queue *)sShm)->dwBufSize);
		*pdwBufSize = iBufSize;

		*pstIdx = (Msg_Channel_Queue *)sShm;
	
		return 0;
	}
	
	*pstIdx = (Msg_Channel_Queue *)sShm;
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// add  package to channel
////////////////////////////////////////////////////////////////////////////////////

int AddPackageToMsgChannel(Msg_Channel_Queue *pstChannel, char *pBody, int iBodyLen)
{
	MsgChannelPkgHead stChannelPkgHead;
	char *pBuffer, *pCur;
	volatile uint32_t *plHeadPos, *plTailPos, lHeadPos, lTailPos;
	int iPkgLen;
	
	pBuffer = pstChannel->caRequestBuffer;
	plHeadPos = &pstChannel->lRequestHeadPos;
	plTailPos = &pstChannel->lRequestTailPos;
	
	lHeadPos = *plHeadPos;
	lTailPos = *plTailPos;
	
	//异常情况
	//check pointer
	if (lHeadPos >= pstChannel->dwBufSize || lTailPos >= pstChannel->dwBufSize) 
	{
		//fatal error
		*plHeadPos = *plTailPos = 0;
		return -1;
	}
	
	//package length
	iPkgLen = sizeof(stChannelPkgHead) + iBodyLen;
	
	//tail pointer is behind head pointer
	
	if (lTailPos >= lHeadPos)
	{		
		if (pstChannel->dwBufSize - lTailPos == iPkgLen && lHeadPos == 0) return -2;//channel空间不足。
		if (pstChannel->dwBufSize - lTailPos < iPkgLen)//确保包是连续存放的，不会一半在channel头一半在channel尾，虽然这样会浪费一点空间。
		{
			if (lHeadPos <= iPkgLen) return -3;
			
			*(pBuffer + lTailPos) = ETX;
			lTailPos = 0;
		}
	}
	else
	{
		if (lHeadPos - lTailPos <= iPkgLen) return -4;//channel空间不足
	}

	//正常流程
	//address to add package
	
	//定位pCur到channel尾端
	pCur = pBuffer + lTailPos;
	
	//add package
	//填充channel包头部
	stChannelPkgHead.cFlag = STX;
	stChannelPkgHead.shLength = iBodyLen;

	//将channel包加入channel中。
	memcpy(pCur, &stChannelPkgHead, sizeof(stChannelPkgHead));
	memcpy(pCur+sizeof(stChannelPkgHead), pBody, iBodyLen);
	
	//change tail pointer
	//更改channel 尾指针。
	*plTailPos = (lTailPos + iPkgLen) % pstChannel->dwBufSize;
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////
// get channel usagerates
////////////////////////////////////////////////////////////////////////////////////


int GetMsgChannelUsageRates( Msg_Channel_Queue *pstChannel,int *piUsageRates)
{
	volatile unsigned long *plHeadPos, *plTailPos, lHeadPos, lTailPos;

	int iBufLen;

	if ( piUsageRates == NULL )
	{
		return -1;
	}
	
	plHeadPos = &pstChannel->lRequestHeadPos;
	plTailPos = &pstChannel->lRequestTailPos;
	
	lHeadPos = *plHeadPos;
	lTailPos = *plTailPos;

	if ( lTailPos >= lHeadPos )
	{
		iBufLen = lTailPos - lHeadPos;
	}
	else
	{
		iBufLen = pstChannel->dwBufSize - lHeadPos + lTailPos;
	}

	*piUsageRates = iBufLen * 100 / pstChannel->dwBufSize;	

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////
// get  package from channel
////////////////////////////////////////////////////////////////////////////////////


int GetPackageFromMsgChannel(Msg_Channel_Queue *pstChannel, char *pBody, int *piBodyLen)
{
	MsgChannelPkgHead *pstChannelPkgHead;
	char *pBuffer, *pCur;
	volatile unsigned long *plHeadPos, *plTailPos, lHeadPos, lTailPos;
	int iPkgLen, iBufLen;
	
	pBuffer = pstChannel->caRequestBuffer;
	plHeadPos = &pstChannel->lRequestHeadPos;
	plTailPos = &pstChannel->lRequestTailPos;
	
	lHeadPos = *plHeadPos;
	lTailPos = *plTailPos;

	//异常情况
	//check pointer
	if (lHeadPos >= pstChannel->dwBufSize || lTailPos >= pstChannel->dwBufSize) 
	{
		//fatal error
		*plHeadPos = *plTailPos = 0;
		return -1;
	}
	
	if (lHeadPos == lTailPos) return -2;//channel为空。
	
	if (lHeadPos > lTailPos && *(pBuffer + lHeadPos) == ETX) //头指针取到剩余残片，则跳到0位置，因为channel包是连续放的，不会一半在头一半在尾。
	{
		*plHeadPos = lHeadPos = 0;
		if (lHeadPos == lTailPos) return -3;//channel被取空
	}
	
	//get buffer length
	//iBufLen是指 现在连续的最大已占用空间
	if (lTailPos > lHeadPos) iBufLen = lTailPos - lHeadPos;//这种情况下iBufLen的大小就是实际已占用空间。
	else iBufLen = pstChannel->dwBufSize - lHeadPos;//这种情况下iBufLen的大小小于实际已占用空间。
	
	if (iBufLen < sizeof(MsgChannelPkgHead) )//占用的空间不足以读取一个channel包头，这种情况是除非AddPackagetochannel出错才可能出现
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -4;
	}
	
	pstChannelPkgHead = (MsgChannelPkgHead *)(pBuffer + lHeadPos);
	if (pstChannelPkgHead->cFlag != STX)//解析channel包头错误
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -5;
	}
	
	iPkgLen = sizeof(MsgChannelPkgHead) + pstChannelPkgHead->shLength;//读取channel包体长度字段，计算整个包的长度
	
	if (iBufLen < iPkgLen || *piBodyLen < pstChannelPkgHead->shLength)//已占用空间不足读取整个包，或调用这个函数的外部程序提供的buffer长度不足放整个channel包
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -6;
	}

	//正常情况
	//get package
	pCur = pBuffer + lHeadPos + sizeof(MsgChannelPkgHead);
	*piBodyLen = pstChannelPkgHead->shLength;
	memcpy(pBody, pCur, *piBodyLen);
	
	//change head pointer
	*plHeadPos = (lHeadPos + iPkgLen) % pstChannel->dwBufSize;
	
	return 0;
}

