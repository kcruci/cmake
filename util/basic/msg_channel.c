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
	
	//�쳣���
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
		if (pstChannel->dwBufSize - lTailPos == iPkgLen && lHeadPos == 0) return -2;//channel�ռ䲻�㡣
		if (pstChannel->dwBufSize - lTailPos < iPkgLen)//ȷ������������ŵģ�����һ����channelͷһ����channelβ����Ȼ�������˷�һ��ռ䡣
		{
			if (lHeadPos <= iPkgLen) return -3;
			
			*(pBuffer + lTailPos) = ETX;
			lTailPos = 0;
		}
	}
	else
	{
		if (lHeadPos - lTailPos <= iPkgLen) return -4;//channel�ռ䲻��
	}

	//��������
	//address to add package
	
	//��λpCur��channelβ��
	pCur = pBuffer + lTailPos;
	
	//add package
	//���channel��ͷ��
	stChannelPkgHead.cFlag = STX;
	stChannelPkgHead.shLength = iBodyLen;

	//��channel������channel�С�
	memcpy(pCur, &stChannelPkgHead, sizeof(stChannelPkgHead));
	memcpy(pCur+sizeof(stChannelPkgHead), pBody, iBodyLen);
	
	//change tail pointer
	//����channel βָ�롣
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

	//�쳣���
	//check pointer
	if (lHeadPos >= pstChannel->dwBufSize || lTailPos >= pstChannel->dwBufSize) 
	{
		//fatal error
		*plHeadPos = *plTailPos = 0;
		return -1;
	}
	
	if (lHeadPos == lTailPos) return -2;//channelΪ�ա�
	
	if (lHeadPos > lTailPos && *(pBuffer + lHeadPos) == ETX) //ͷָ��ȡ��ʣ���Ƭ��������0λ�ã���Ϊchannel���������ŵģ�����һ����ͷһ����β��
	{
		*plHeadPos = lHeadPos = 0;
		if (lHeadPos == lTailPos) return -3;//channel��ȡ��
	}
	
	//get buffer length
	//iBufLen��ָ ���������������ռ�ÿռ�
	if (lTailPos > lHeadPos) iBufLen = lTailPos - lHeadPos;//���������iBufLen�Ĵ�С����ʵ����ռ�ÿռ䡣
	else iBufLen = pstChannel->dwBufSize - lHeadPos;//���������iBufLen�Ĵ�СС��ʵ����ռ�ÿռ䡣
	
	if (iBufLen < sizeof(MsgChannelPkgHead) )//ռ�õĿռ䲻���Զ�ȡһ��channel��ͷ����������ǳ���AddPackagetochannel����ſ��ܳ���
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -4;
	}
	
	pstChannelPkgHead = (MsgChannelPkgHead *)(pBuffer + lHeadPos);
	if (pstChannelPkgHead->cFlag != STX)//����channel��ͷ����
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -5;
	}
	
	iPkgLen = sizeof(MsgChannelPkgHead) + pstChannelPkgHead->shLength;//��ȡchannel���峤���ֶΣ������������ĳ���
	
	if (iBufLen < iPkgLen || *piBodyLen < pstChannelPkgHead->shLength)//��ռ�ÿռ䲻���ȡ�����������������������ⲿ�����ṩ��buffer���Ȳ��������channel��
	{
		//fatal error
		*plHeadPos = lTailPos;
		return -6;
	}

	//�������
	//get package
	pCur = pBuffer + lHeadPos + sizeof(MsgChannelPkgHead);
	*piBodyLen = pstChannelPkgHead->shLength;
	memcpy(pBody, pCur, *piBodyLen);
	
	//change head pointer
	*plHeadPos = (lHeadPos + iPkgLen) % pstChannel->dwBufSize;
	
	return 0;
}

