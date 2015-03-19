#include "game_base_svr_struct.h"
#include "endecode_base.h"

/*
pkgHead = dwPkgLen+dwHeadLen+cPkgType+cCmdStatus+dwCmdId+dwSeqId+ddwUid+
wZoneId+dwSrcIp+wSrcPort+dwDstIp+wDstPort+dwResult+wTlvNum+stTlv[64]
*/
int DecodePkgHead(pkg_head *pstPkgHead, char *pPkg, int iPkgLen)
{
	if(NULL==pstPkgHead || NULL==pPkg || 0==iPkgLen)
		RET_LINE();
	
	char *pCur = pPkg;
	int iLenLeft = iPkgLen;
	uint16_t ii = 0;

	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwPkgLen)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwHeadLen)));
	RET_TRUE_LINE(GetINT8(&pCur, &iLenLeft, &(pstPkgHead->cPkgType)));
	RET_TRUE_LINE(GetINT8(&pCur, &iLenLeft, &(pstPkgHead->cCmdStatus)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwCmdId)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwSeqId)));
	RET_TRUE_LINE(GetINT64(&pCur, &iLenLeft, &(pstPkgHead->ddwUid)));
	RET_TRUE_LINE(GetINT8(&pCur, &iLenLeft, &(pstPkgHead->cPlatId)));
	RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->wZoneId)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwSrcIp)));
	RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->wSrcPort)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwDstIp)));
	RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->wDstPort)));
	RET_TRUE_LINE(GetINT32(&pCur, &iLenLeft, &(pstPkgHead->dwResult)));
	RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->wTlvNum)));

	for(ii=0; ii<pstPkgHead->wTlvNum; ii++)
	{
		RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->stTlv[ii].wTypeId)));
		RET_TRUE_LINE(GetINT16(&pCur, &iLenLeft, &(pstPkgHead->stTlv[ii].wLen)));

		if(TLV_VALUE_MAX_SIZE<pstPkgHead->stTlv[ii].wLen || iLenLeft<pstPkgHead->stTlv[ii].wLen)
			RET_LINE();
		memcpy(pstPkgHead->stTlv[ii].cValue, pCur, pstPkgHead->stTlv[ii].wLen);
		pCur += pstPkgHead->stTlv[ii].wLen;
		iLenLeft -= pstPkgHead->stTlv[ii].wLen;
	}
	
	return 0;
}

int EncodePkgHead(pkg_head *pstPkgHead, char *pPkg, int *iPkgLen)
{
	if(NULL==pstPkgHead || NULL==pPkg || NULL==iPkgLen || 0==*iPkgLen)
		RET_LINE();

	char *pCur = pPkg;
	int iLenLeft = *iPkgLen;
	uint16_t ii = 0;

	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwPkgLen));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwHeadLen));
	RET_TRUE_LINE(AddINT8(&pCur, &iLenLeft, pstPkgHead->cPkgType));
	RET_TRUE_LINE(AddINT8(&pCur, &iLenLeft, pstPkgHead->cCmdStatus));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwCmdId));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwSeqId));
	RET_TRUE_LINE(AddINT64(&pCur, &iLenLeft, pstPkgHead->ddwUid));
	RET_TRUE_LINE(AddINT8(&pCur, &iLenLeft, pstPkgHead->cPlatId));
	RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->wZoneId));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwSrcIp));
	RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->wSrcPort));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwDstIp));
	RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->wDstPort));
	RET_TRUE_LINE(AddINT32(&pCur, &iLenLeft, pstPkgHead->dwResult));
	RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->wTlvNum));

	for(ii=0; ii<pstPkgHead->wTlvNum; ii++)
	{
		RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->stTlv[ii].wTypeId));
		RET_TRUE_LINE(AddINT16(&pCur, &iLenLeft, pstPkgHead->stTlv[ii].wLen));

		if(TLV_VALUE_MAX_SIZE<pstPkgHead->stTlv[ii].wLen || iLenLeft<pstPkgHead->stTlv[ii].wLen)
			RET_LINE();
		memcpy(pCur, pstPkgHead->stTlv[ii].cValue, pstPkgHead->stTlv[ii].wLen);
		pCur += pstPkgHead->stTlv[ii].wLen;
		iLenLeft -= pstPkgHead->stTlv[ii].wLen;
	}

	*iPkgLen = iLenLeft;
	
	return 0;
}

int GetPkgHeadLen(pkg_head *pstPkgHead)
{
	if(NULL == pstPkgHead)
		RET_LINE();

	uint16_t i = 0;
	uint32_t dwHeadLen = 0;

	dwHeadLen += sizeof(pstPkgHead->dwPkgLen);
	dwHeadLen += sizeof(pstPkgHead->dwHeadLen);
	dwHeadLen += sizeof(pstPkgHead->cPkgType);
	dwHeadLen += sizeof(pstPkgHead->cCmdStatus);
	dwHeadLen += sizeof(pstPkgHead->dwCmdId);
	dwHeadLen += sizeof(pstPkgHead->dwSeqId);
	dwHeadLen += sizeof(pstPkgHead->ddwUid);
	dwHeadLen += sizeof(pstPkgHead->cPlatId);
	dwHeadLen += sizeof(pstPkgHead->wZoneId);
	dwHeadLen += sizeof(pstPkgHead->dwSrcIp);
	dwHeadLen += sizeof(pstPkgHead->wSrcPort);
	dwHeadLen += sizeof(pstPkgHead->dwDstIp);
	dwHeadLen += sizeof(pstPkgHead->wDstPort);
	dwHeadLen += sizeof(pstPkgHead->dwResult);
	dwHeadLen += sizeof(pstPkgHead->wTlvNum);

	for(i=0; i<pstPkgHead->wTlvNum; i++)
	{
		dwHeadLen += sizeof(pstPkgHead->stTlv[i].wTypeId);
		dwHeadLen += sizeof(pstPkgHead->stTlv[i].wLen);
		dwHeadLen += pstPkgHead->stTlv[i].wLen;
	}

	pstPkgHead->dwHeadLen = dwHeadLen;
	
	return 0;
}

int GetLogicSvrMsgBusShmKey(pkg_head *pstPkgHead, uint32_t *pdwMsgBusShmKey)
{
	if(NULL==pstPkgHead || NULL==pdwMsgBusShmKey)
		RET_LINE();

	uint16_t i = 0;
	uint32_t dwKey = 0;
	for(i=0; i<pstPkgHead->wTlvNum; i++)
	{
		if(e_src_logicsvr_shm_key == pstPkgHead->stTlv[i].wTypeId)
		{
			if(sizeof(uint32_t) == pstPkgHead->stTlv[i].wLen)
			{
				memcpy(&dwKey, pstPkgHead->stTlv[i].cValue, sizeof(uint32_t));
				*pdwMsgBusShmKey= ntohl(dwKey);
			}
			
			break;
		}
	}
	
	return 0;
}

