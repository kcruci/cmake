#ifndef _IDS_PVP_SVR_COMMON_H
#define  _IDS_PVP_SVR_COMMON_H
#include <arpa/inet.h>

#include "basic/data_type/Vector4.h"
#include "ids_cmd.h"
#include "ids_pvp_msg.h"

struct PushPath
{
	uint32_t dwGatewayFd;
	uint32_t dwProxyFd;
	uint32_t dwLogicSvrShmKey;

	struct sockaddr_in stClientUdpAddr;
	PushPath(uint32_t dwGatewayFd, uint32_t dwProxyFd, uint32_t dwLogicSvrShmKey)
	{
		this->dwGatewayFd = dwGatewayFd;
        this->dwProxyFd = dwProxyFd;
		this->dwLogicSvrShmKey = dwLogicSvrShmKey;
		memset(&stClientUdpAddr, 0x00, sizeof(stClientUdpAddr));
	}

	PushPath(const struct sockaddr_in &stClientUdpAddr)
	{
		dwGatewayFd = 0;
		dwProxyFd = 0;
		dwLogicSvrShmKey = 0;
		this->stClientUdpAddr = stClientUdpAddr;
	}

	PushPath()
	{
		dwGatewayFd = 0;
		dwProxyFd = 0;
		dwLogicSvrShmKey = 0;
		memset(&stClientUdpAddr, 0x00, sizeof(stClientUdpAddr));
	}
};

struct IPvpMsgHandlerBase
{
public:
	IPvpMsgHandlerBase()
	{

	}
	virtual ~IPvpMsgHandlerBase()
	{
		
	}
	virtual int HandleMsg(const pkg_head &head, const PushPath &pushPath, const PvpBussinessMsgHead* pPvpMsgHead, unsigned uBussinessMsgLen) = 0;

};

struct IUseItemHandler
{
public:
	IUseItemHandler()
	{
	}
	
	virtual ~IUseItemHandler()
	{	
	}
	virtual void OnUseItem(uint64_t u64SenderUid, const PushPath &pushPath, const PvpGameMsgUseItem* pMsg) = 0;
};


#endif //_IDS_PVP_SVR_COMMON_H

