#include "cfg_api.h"
#include "endecode_base.h"

static int InitGameBaseSvrShmPointer(char *pTable, game_base_svr_route_cfg *pstCfgShm)
{
	if(NULL==pTable || NULL==pstCfgShm)
		RET_LINE();

	char *pCur = pTable;

	pstCfgShm->pRWFlag = (uint32_t *)pCur;
	
	pCur += sizeof(uint32_t);
	pstCfgShm->pdwGameBaseSvrProxyNum = (uint32_t *)pCur;
	
	pCur += sizeof(uint32_t);
	pstCfgShm->pstProxyCfgList = (game_base_svr_proxy_cfg *)pCur;
	
	pCur += sizeof(game_base_svr_proxy_cfg)*MAX_GAME_BASE_SVR_PROXY_CFG_NUM;
	pstCfgShm->pdwGameBaseSvrProxyCmdCfgNum = (uint32_t *)pCur;

	pCur += sizeof(uint32_t);
	pstCfgShm->pstProxyCmdCfgList = (game_base_svr_proxy_cmd_cfg *)pCur;

	pCur += sizeof(game_base_svr_proxy_cmd_cfg)*MAX_GAME_BASE_SVR_PROXY_CMD_CFG_NUM;
	pstCfgShm->pdwGameBaseSvrProxyCmdRouteCfgNum = (uint32_t *)pCur;

	pCur += sizeof(uint32_t);
	pstCfgShm->pstProxyCmdRouteCfgList = (game_base_svr_proxy_cmd_route_cfg *)pCur;

	pCur += sizeof(game_base_svr_proxy_cmd_route_cfg)*MAX_GAME_BASE_SVR_PROXY_CMD_ROUTE_CFG_NUM;
	pstCfgShm->pdwGameBaseSvrLogicSvrCfgNum = (uint32_t *)pCur;

	pCur += sizeof(uint32_t);
	pstCfgShm->pstLogicSvrCfgList = (game_base_svr_logic_svr_cfg *)pCur;
	
	pCur += sizeof(game_base_svr_logic_svr_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_NUM;
	pstCfgShm->pdwGameBaseSvrLogicSvrCmdCfgNum = (uint32_t *)pCur;

	pCur += sizeof(uint32_t);
	pstCfgShm->pstLogicSvrCmdCfgList = (game_base_svr_logic_svr_cmd_cfg *)pCur;

	pCur += sizeof(game_base_svr_logic_svr_cmd_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_CFG_NUM;
	pstCfgShm->pdwGameBaseSvrLogicSvrCmdRouteCfgNum = (uint32_t *)pCur;

	pCur += sizeof(uint32_t);
	pstCfgShm->pstLogicSvrCmdRouteCfgList = (game_base_svr_logic_svr_cmd_route_cfg *)pCur;

	pCur += sizeof(game_base_svr_logic_svr_cmd_route_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_ROUTE_CFG_NUM;
	pstCfgShm->pstChangeInfo = (game_base_svr_change_info *)pCur;
	
	return 0;
}

int CFG_GetShmSize()
{
	int iShmSize = 0;

	iShmSize += sizeof(uint32_t);

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_proxy_cfg)*MAX_GAME_BASE_SVR_PROXY_CFG_NUM;

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_proxy_cmd_cfg)*MAX_GAME_BASE_SVR_PROXY_CMD_CFG_NUM;

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_proxy_cmd_route_cfg)*MAX_GAME_BASE_SVR_PROXY_CMD_ROUTE_CFG_NUM;

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_logic_svr_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_NUM;

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_logic_svr_cmd_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_CFG_NUM;

	iShmSize += sizeof(uint32_t);
	iShmSize += sizeof(game_base_svr_logic_svr_cmd_route_cfg)*MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_ROUTE_CFG_NUM;

	iShmSize += sizeof(game_base_svr_change_info)*CHANGE_INFO_SIZE;
	
	return iShmSize;
}

int CFG_InitGameBaseSvrShm(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm)
{
	if(NULL == pstCfgShm)
		return -1;

	char *pTable = NULL;
	int iRet=0, iShmSize=0;

	iShmSize = CFG_GetShmSize();
	if(0>=iShmSize || MAX_GAME_BASE_SVR_SHM_SIZE<iShmSize)
		return -2;

	iRet = GetShm2(&pTable, dwShmKey, iShmSize, 0666);
	if(0>iRet || NULL==pTable)
		return -3;

	iRet = InitGameBaseSvrShmPointer(pTable, pstCfgShm);
	if(iRet)
		return -4;
	
	return 0;
}

int CFG_InitGameBaseSvrShmEx(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm)
{
	if(NULL == pstCfgShm)
		return -1;

	char *pTable = NULL;
	int iRet=0, iShmSize=0, iNewCreate=0;
	game_base_svr_change_info *pstChangeInfo = NULL;

	iShmSize = CFG_GetShmSize();
	if(0>=iShmSize || MAX_GAME_BASE_SVR_SHM_SIZE<iShmSize)
		return -2;

	iNewCreate = GetShm2(&pTable, dwShmKey, iShmSize, 0666|IPC_CREAT);
	if(0>iNewCreate|| NULL==pTable)
		return -3;

	iRet = InitGameBaseSvrShmPointer(pTable, pstCfgShm);
	if(iRet)
		return -4;

	if(1 == iNewCreate)
	{
		pstChangeInfo = pstCfgShm->pstChangeInfo;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_PROXY_CFG);

		pstChangeInfo += 1;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_PROXY_CMD_CFG);

		pstChangeInfo += 1;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_PROXY_CMD_ROUTE_CFG);

		pstChangeInfo += 1;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_LOGIC_SVR_CFG);

		pstChangeInfo += 1;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_CFG);

		pstChangeInfo += 1;
		snprintf(pstChangeInfo->cTableName, sizeof(pstChangeInfo->cTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_ROUTE_CFG);
	}
	
	return 0;
}


int CFG_GetLocalChangeInfoByTableName(game_base_svr_route_cfg *pstCfgShm, game_base_svr_change_info **pstChangeInfo, char *cTableName)
{
	if(NULL==pstCfgShm || NULL==pstChangeInfo)
		RET_LINE();
	
	uint32_t ii = 0;

	game_base_svr_change_info *pstTmpChageInfo = pstCfgShm->pstChangeInfo;
	for(ii=0; ii<CHANGE_INFO_SIZE&&pstTmpChageInfo; ii++)
	{
		if(0 == strcmp(pstTmpChageInfo->cTableName, cTableName))
		{
			*pstChangeInfo = pstTmpChageInfo;
			return 1;
		}

		pstTmpChageInfo += 1;
	}
	
	return 0;
}


int CFG_GetLocalChangeInfoByTableId(game_base_svr_route_cfg *pstCfgShm, int iTableType, game_base_svr_change_info **ppChangeInfo)
{
	if(NULL==pstCfgShm || NULL==ppChangeInfo)
		return -1;

	int i = 0;
	uint8_t stFTableName[32] = {0};
	game_base_svr_change_info *pChangeInfo = pstCfgShm->pstChangeInfo;

	if(e_t_proxy_cfg == iTableType)
	{
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_PROXY_CFG);	
	}
	else if(e_t_proxy_cmd_cfg == iTableType)
	{
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_PROXY_CMD_CFG);	
	}
	else if(e_t_proxy_cmd_route_cfg == iTableType)
	{	
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_PROXY_CMD_ROUTE_CFG);	
	}
	else if(e_t_logic_svr_cfg == iTableType)
	{	
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_LOGIC_SVR_CFG);	
	}
	else if(e_t_logic_svr_cmd_cfg == iTableType)
	{
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_CFG);
	}
	else if(e_t_logic_svr_cmd_route_cfg == iTableType)
	{
		snprintf((char *)stFTableName, sizeof(stFTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_ROUTE_CFG);
	}
	else 
	{
		return -2;
	}
	if(NULL == pChangeInfo)
		return -3;
	for(i=0; i<CHANGE_INFO_SIZE; i++)
	{
		int iRet = strncmp((const char *)stFTableName, (char*)pChangeInfo->cTableName,sizeof(stFTableName));
		if(iRet == 0)
		{
			*ppChangeInfo = pChangeInfo;
			return 1;
		}
		pChangeInfo+=1;
	}
	
	return 0;
}

int CFG_GetChangeTime(game_base_svr_route_cfg *pstCfgShm, int iTableId, uint64_t *pddwChangeTime)
{
	if(NULL == pstCfgShm)
		return -1;

	int iRet = 0;
	game_base_svr_change_info *pstChangeInfo = NULL;
	iRet = CFG_GetLocalChangeInfoByTableId(pstCfgShm, iTableId, &pstChangeInfo);
	if(iRet || NULL==pstChangeInfo)
	{
		return -2;
	}

	*pddwChangeTime = pstChangeInfo->ddwChangeTime;

	return 0;
}

int CFG_CheckUpdate(game_base_svr_route_cfg *pstCfgShm, game_base_svr_table_id TableId, uint64_t *pddwTime)
{
	if(NULL==pstCfgShm || (e_t_max_id-1)<=TableId || NULL==pddwTime)
		return -1;

	int iRet = 0;
	char cTableName[MAX_TABLE_NAME_LEN] = {0};
	game_base_svr_change_info *pstChangeInfo = NULL;
	
	switch(TableId)
	{
		case e_t_proxy_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_PROXY_CFG);
			break;
		case e_t_proxy_cmd_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_PROXY_CMD_CFG);
			break;
		case e_t_proxy_cmd_route_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_PROXY_CMD_ROUTE_CFG);
			break;
		case e_t_logic_svr_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_LOGIC_SVR_CFG);
			break;
		case e_t_logic_svr_cmd_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_CFG);
			break;
		case e_t_logic_svr_cmd_route_cfg:
			snprintf(cTableName, sizeof(cTableName), "%s", TABLE_NAME_LOGIC_SVR_CMD_ROUTE_CFG);
			break;
		default:
			return -2;
	}

	iRet = CFG_GetLocalChangeInfoByTableName(pstCfgShm, &pstChangeInfo, cTableName);
	if(0 >= iRet)
		return -3;

	if(*pddwTime < pstChangeInfo->ddwChangeTime)
	{
		*pddwTime = pstChangeInfo->ddwChangeTime;
		return 1;
	}
	
	return 0;
}

int CFG_GetLogicSvrCfg(game_base_svr_route_cfg *pstCfgShm, uint32_t dwInnerGWId, game_base_svr_logic_svr_cfg *pstLogicSvrNode)
{
	if(NULL==pstCfgShm || NULL==pstLogicSvrNode)
		return -1;
	
	uint32_t ii=0, dwInnerGateWayNum=0;
	game_base_svr_logic_svr_cfg *pstTmpLogicSvrCfg = NULL;

	dwInnerGateWayNum = *(pstCfgShm->pdwGameBaseSvrLogicSvrCfgNum);
	pstTmpLogicSvrCfg = pstCfgShm->pstLogicSvrCfgList;

	for(ii=0; ii<dwInnerGateWayNum && pstTmpLogicSvrCfg; ii++)
	{
		if(dwInnerGWId == pstTmpLogicSvrCfg->dwSvrId)
		{
			pstLogicSvrNode->wZoneId = pstTmpLogicSvrCfg->wZoneId;
			pstLogicSvrNode->dwSvrId = pstTmpLogicSvrCfg->dwSvrId;
			pstLogicSvrNode->dwIp = pstTmpLogicSvrCfg->dwIp;
			pstLogicSvrNode->wPort = pstTmpLogicSvrCfg->wPort;
			pstLogicSvrNode->cWorkingState = pstTmpLogicSvrCfg->cWorkingState;
			
			return 1;
		}

		pstTmpLogicSvrCfg += 1;
	}
	
	return 0;
}

int CFG_GetOneLogicSvrNode(game_base_svr_route_cfg *pstCfgShm, 
	uint16_t wZoneId, uint32_t dwCmdId, 
	uint32_t dwLocalIp, game_base_svr_logic_svr_cfg *pstLogicSvrNode)
{
	if(NULL==pstCfgShm || NULL==pstLogicSvrNode)
		return -1;

	int iRet=0, iRandIndex=0;
	time_t tCur;
	uint32_t ii=0, dwFound=0, dwProxyCmdRouteCfgNum=0, dwLogicSvrNum=0, dwRealLogicSvrNum=0;
	game_base_svr_proxy_cmd_route_cfg *pstProxyCmdRouteCfg = NULL;
	game_base_svr_logic_svr_cfg *pstTmpLogicSvrNode=NULL, stTmpLogicSvrCfgList[MAX_LOGIC_SVR_NUM_PER_CMD];

	dwProxyCmdRouteCfgNum = *(pstCfgShm->pdwGameBaseSvrProxyCmdRouteCfgNum);
	pstProxyCmdRouteCfg = pstCfgShm->pstProxyCmdRouteCfgList;

	for(ii=0; ii<dwProxyCmdRouteCfgNum && pstProxyCmdRouteCfg; ii++)
	{
		if(wZoneId==pstProxyCmdRouteCfg->wZoneId && dwCmdId==pstProxyCmdRouteCfg->dwCmdId)
		{
			dwFound = 1;
			break;
		}
		
		pstProxyCmdRouteCfg += 1;
	}

	if(0==dwFound || 0>=pstProxyCmdRouteCfg->dwLogicSvrNum)
		return -2;

	dwLogicSvrNum = pstProxyCmdRouteCfg->dwLogicSvrNum;
	memset(stTmpLogicSvrCfgList, 0, sizeof(stTmpLogicSvrCfgList));
	for(ii=0; ii<dwLogicSvrNum; ii++)
	{
		iRet = CFG_GetLogicSvrCfg(pstCfgShm, pstProxyCmdRouteCfg->dwLogicSvrIdList[ii], &stTmpLogicSvrCfgList[dwRealLogicSvrNum]);
		if(0 > iRet)
			return -3;
		else if(1 == iRet)
		{
			if(e_working == stTmpLogicSvrCfgList[dwRealLogicSvrNum].cWorkingState)
				dwRealLogicSvrNum ++;
		}
	}

	if(0>=dwRealLogicSvrNum || MAX_LOGIC_SVR_NUM_PER_CMD<dwRealLogicSvrNum)
		return -4;

	time(&tCur);
	srand(tCur+dwLocalIp);
	iRandIndex = (int)(dwRealLogicSvrNum*(rand()/(RAND_MAX+1.0)));
	pstTmpLogicSvrNode = &stTmpLogicSvrCfgList[iRandIndex];

	pstLogicSvrNode->wZoneId = pstTmpLogicSvrNode->wZoneId;
	pstLogicSvrNode->dwSvrId = pstTmpLogicSvrNode->dwSvrId;
	pstLogicSvrNode->dwIp = pstTmpLogicSvrNode->dwIp;
	pstLogicSvrNode->wPort = pstTmpLogicSvrNode->wPort;
	pstLogicSvrNode->cWorkingState = pstTmpLogicSvrNode->cWorkingState;
	
	return 0;
}



