#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#ifdef __cplusplus
extern "C"
{
#endif

#include "ids_basic_log.h"
#include "ids_basic_misc.h"

#include "ids_network_frame_io_multi_process.h"
#ifdef __cplusplus
}
#endif


#include "ids_network_frame.h"

#define TIMEOUT_CONNECT 2
#define TIME_RECONNECT 2
#define TCP_SHORTLINK_TIMEOUT 1
#define MAX_SERVER_PORT 30

#define LOG(fmt, args...) do {if (g_stIDSNetworkFrameConfig.pstLog) Log(g_stIDSNetworkFrameConfig.pstLog, 2, "%s:%d(%s): " fmt, __FILE__, __LINE__, __FUNCTION__ , ## args);else printf("%s:%d(%s): " fmt, __FILE__, __LINE__, __FUNCTION__ , ## args);} while(0)
#define LOG_STAT(fmt, args...) do {if (g_stIDSNetworkFrameConfig.pstLog) Log(g_stIDSNetworkFrameConfig.pstLog, 2, "%s: " fmt, "FRAME:STAT" , ## args);} while (0)
#define LOG_BUG_RET(retval) do { LOG("BUG!"); return (retval); } while (0)
#define ERRSTR strerror(errno)
#define OTHER_FD_NUM 100

typedef struct tag_ids_socket_context_inner
{
    int iSocket;
    uint64_t ddwUID;
    struct sockaddr_in stClientAddr;
    time_t tCreateTime;
    time_t tLastAccessTime;
    ids_socket_context_stat Stat;//SCTX的定义到此
    int iBytesRecved;
    int iBytesToSend;
    int iPkgLen;
    char RecvBuf[RECVBUF_LEN];
    char SendBuf[SENDBUF_LEN];
	int iAddrName; // 本地监听的Name；如果是本身作为客户端时，在客户端列表的索引
    int iPkgHeadLen;
    tag_ids_socket_context_inner()
	{
		ddwUID = 0;
	}
}ids_socket_context;                  // 该定义必须与SCTX的定义吻合

typedef struct 
{
    int iCltDefNum;
    IDSNetworkFrameAddrDefine stCltDefArr[MAX_CLTDEF];
    int iSocket[MAX_CLTDEF];
    time_t tLastConnectTime[MAX_CLTDEF];
}ids_inner_contrl_def_node;

typedef struct
{
    LogFile *pstLog;
    int iLogLevel;

	int iTimeoutMSec; // 等待fd的时候的超时值，单位为毫秒
	int iShortLinkTimeOutSec; //TCP短连接时的阻塞超时值，单位是秒
    int iUserInfoLen;
    void *aUserInfo;

    IDSNetworkFrameAddrDefine stTCPSvrList[MAX_SERVER_PORT];

    IDSNetworkFrameAddrDefine stUDPSvrList[MAX_SERVER_PORT];

    int iUDPSocketList[MAX_SERVER_PORT];

	ids_inner_contrl_def_node stIDSInnerCtlDefNode;

    ids_io_multi_process_mng *pstIOMPMng;
    int iMaxAcceptSocketNum;
    int iCurAcceptCltNum;

    int iTotalSocketNum;           

    ids_socket_context *astSocketContext;
    ids_network_frame_callback stCallback;
}ids_network_frame_config;

static ids_network_frame_config g_stIDSNetworkFrameConfig;

static int ids_network_frame_create_listen_socket(void);
static int ids_network_frame_connect_to_tcp_all_server(void);
static int AddParentTcpAcceptSocket(int iParentTcpListenNum, IDSNetworkFrameAddrDefineEx *pstParentListenDefEx);
static int AddOneTcpClientDef(IDSNetworkFrameAddrDefine *pstTCPCltDef);

char *DateTimeStr1(const time_t *mytime, char *strtime)
{
    struct tm curr;
    static char sinner[50];
    char * s = strtime;
    if (!s)
    {
        s = sinner;
    }
    
    curr = *localtime(mytime);

    if (curr.tm_year > 50)
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", 
                    curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
                    curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    else
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", 
                    curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday,
                    curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
                
    return s;
}

static int IDSNetworkFrameHandleLoopDefault(void)
{
	LOG("********* %s **********", __FUNCTION__);

    return 0;
}

static int IDSNetworkFrameHandleAcceptDefault(_ids_socket_context * pstSctx, void *pUserInfo, int iListenAddrName)
{
	LOG("********* %s **********", __FUNCTION__);

    return 0;
}

static int IDSNetworkFrameHandleConnectDefault(_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName)
{
	LOG("********* %s **********", __FUNCTION__);

    return 0;
}

static int IDSNetworkFrameHandlePkgHeadDefault(_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName, void *pPkg, int iBytesRecved, int *piPkgLen)
{
	LOG("********* %s **********", __FUNCTION__);

    LOG("BUG!");
    return -1;
}

static int IDSNetworkFrameHandlePkgDefault(_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName, void *pPkg, int iPkgLen)
{
	LOG("********* %s **********", __FUNCTION__);

    LOG("BUG!");
    return -1;
}

static int IDSNetworkFrameHandleUdpPkgDefault(_ids_socket_context * pstSctx, void *pUserInfo, int iUdpName, void *pPkg, int iPkgLen)
{
    LOG("BUG!");
    return -1;
}

static int IDSNetworkFrameHandleCloseConnDefault(_ids_socket_context *pstSctx, void *pUserInfo, int iAddrName, char *sErrInfo) 
{ 
	LOG("********* %s **********", __FUNCTION__);

	return 0; 
}

static int SetIDSNetworkFrameCallback(ids_network_frame_callback * pstDst, ids_network_frame_callback * pstSrc)
{
	LOG("********* %s **********", __FUNCTION__);
/*
    static ids_network_frame_callback stDefaultCallback = {
        IDSNetworkFrameHandleAccept:IDSNetworkFrameHandleAcceptDefault,
        IDSNetworkFrameHandleConnect:IDSNetworkFrameHandleConnectDefault,
        IDSNetworkFrameHandleLoop:IDSNetworkFrameHandleLoopDefault,
        IDSNetworkFrameHandlePkgHead:IDSNetworkFrameHandlePkgHeadDefault,
        IDSNetworkFrameHandlePkg:IDSNetworkFrameHandlePkgDefault,
        IDSNetworkFrameHandleUdpPkg:IDSNetworkFrameHandleUdpPkgDefault, 
        IDSNetworkFrameHandleCloseConn: IDSNetworkFrameHandleCloseConnDefault
    };
*/
    static ids_network_frame_callback stDefaultCallback;
       

    stDefaultCallback.IDSNetworkFrameHandleAccept=IDSNetworkFrameHandleAcceptDefault;
    stDefaultCallback.IDSNetworkFrameHandleConnect=IDSNetworkFrameHandleConnectDefault;
    stDefaultCallback.IDSNetworkFrameHandleLoop=IDSNetworkFrameHandleLoopDefault;
    stDefaultCallback.IDSNetworkFrameHandlePkgHead=IDSNetworkFrameHandlePkgHeadDefault;
    stDefaultCallback.IDSNetworkFrameHandlePkg=IDSNetworkFrameHandlePkgDefault;
    stDefaultCallback.IDSNetworkFrameHandleUdpPkg=IDSNetworkFrameHandleUdpPkgDefault; 
    stDefaultCallback.IDSNetworkFrameHandleCloseConn= IDSNetworkFrameHandleCloseConnDefault;
        
    int i;
    void **p;

#define HANDLE_MUST_FUNC(x) \
do { \
	if (pstSrc->x) { \
		pstDst->x = pstSrc->x; \
	} else { \
		LOG("FATAL: Parameter error!"); return -2; \
	} \
} while (0)

#define HANDLE_OPT_FUNC(x) \
do { \
	if (pstSrc->x) { \
		pstDst->x = pstSrc->x; \
	} else { \
		pstDst->x = stDefaultCallback.x; \
	} \
} while (0)

    memset(pstDst, 0, sizeof(*pstDst));
    HANDLE_OPT_FUNC(IDSNetworkFrameHandlePkgHead);
    HANDLE_OPT_FUNC(IDSNetworkFrameHandlePkg);
    HANDLE_OPT_FUNC(IDSNetworkFrameHandleLoop);
    HANDLE_OPT_FUNC(IDSNetworkFrameHandleAccept);
    HANDLE_OPT_FUNC(IDSNetworkFrameHandleConnect);
    HANDLE_OPT_FUNC(IDSNetworkFrameHandleUdpPkg);
	HANDLE_OPT_FUNC(IDSNetworkFrameHandleCloseConn);

    for(i = 0, p = (void **) pstDst; i < sizeof(*pstDst) / sizeof(*p); i++, p++)
    {
        if(!(*p))
        {
            LOG("FATAL: Callback func No.%d not init!", i);
            return -1;
        }
    }
    return 0;
}

int IDSNetworkFrameInit(void *pUserInfoBuf, int iUserInfoBufLen,
                     int iUserInfoLen,
                     LogFile * pstLog, int iLogLevel,
                     int iTCPServerNum, IDSNetworkFrameAddrDefine * pstTCPSvrDef, int iMaxAcceptCltNum, 
                     int iUDPServerNum, IDSNetworkFrameAddrDefine * pstUDPSvrDef, 
                     int iTCPAsCltNum, IDSNetworkFrameAddrDefine * pstTCPCltDef,
                     int iParentTcpListenNum, IDSNetworkFrameAddrDefineEx *pstParentListenDefEx,
                     ids_network_frame_callback * pstCallback)
{
	LOG("********* %s **********", __FUNCTION__); //NOTE 下面才赋值 pstConfig->pstLog = pstLog，这个时候会打印在屏幕

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_network_frame_callback *pstDstCallback = &(pstConfig->stCallback);
    int i, iRet=0;
    ids_socket_context *astSocketContext;

	memset(pstConfig, 0, sizeof(*pstConfig));

    pstConfig->pstLog = pstLog;
    pstConfig->iLogLevel = iLogLevel;

    pstConfig->iTotalSocketNum = iTCPServerNum + iMaxAcceptCltNum + iUDPServerNum + iTCPAsCltNum + iParentTcpListenNum + OTHER_FD_NUM;  //包括std的，外部的文件fd，数据库连接fd等，最多10个

    if(!(pstConfig->iMaxAcceptSocketNum = iMaxAcceptCltNum) && iTCPServerNum)
    {
        LOG("FATAL: zero MaxAcceptSocketNum!");
        return -1;
    }

    if(!(pstConfig->iUserInfoLen=iUserInfoLen) && (iTCPServerNum || iTCPAsCltNum))
    {
        LOG("FATAL: zero UserInfoLen len!");
        return -1;
    }
    if(!pUserInfoBuf)
    {
        if(!(pstConfig->aUserInfo = malloc(iUserInfoLen * pstConfig->iTotalSocketNum)))
        {
            LOG("FATAL: OUT OF MEMORY!");
            return -1;
        }
    }
    else
    {
        if(iUserInfoBufLen < iUserInfoLen * (pstConfig->iTotalSocketNum))
        {
            LOG("FATAL: UserInfoBufLen mismatch!");
            return -1;
        }
        pstConfig->aUserInfo = pUserInfoBuf;
    }

    if(MAX_SERVER_PORT < (iTCPServerNum+iParentTcpListenNum))
    {
        LOG("FATAL: Too many TCP ports!");
        return -1;
    }
    memset(pstConfig->stTCPSvrList, 0, sizeof(pstConfig->stTCPSvrList));
    for(i = 0; i < iTCPServerNum; i++)
    {
        memcpy(&(pstConfig->stTCPSvrList[i]), &(pstTCPSvrDef[i]), sizeof(IDSNetworkFrameAddrDefine));
    }
    if(MAX_SERVER_PORT < iUDPServerNum)
    {
        LOG("FATAL: Too many UDP ports!");
        return -1;
    }
    memset(pstConfig->stUDPSvrList, 0, sizeof(pstConfig->stUDPSvrList));
    for(i = 0; i < iUDPServerNum; i++)
    {
        memcpy(&(pstConfig->stUDPSvrList[i]), &(pstUDPSvrDef[i]), sizeof(IDSNetworkFrameAddrDefine));
    }
    if(MAX_CLTDEF < iTCPAsCltNum)
    {
        LOG("FATAL: Too many TCP Clients!");
        return -1;
    }
    memset(&(pstConfig->stIDSInnerCtlDefNode), 0, sizeof(ids_inner_contrl_def_node));
    pstConfig->stIDSInnerCtlDefNode.iCltDefNum = iTCPAsCltNum;
    for(i = 0; i < iTCPAsCltNum; i++)
    {
        if(MAX_CLTDEF<=pstTCPCltDef[i].iAddrName|| 0>pstTCPCltDef[i].iAddrName)
        {
            LOG("FATAL: TCPClient[%u] Name (%d) is out of [%d, %d)!", i, pstTCPCltDef[i].iAddrName, 0, MAX_CLTDEF);
            return -1;
        }
        memcpy(&((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[pstTCPCltDef[i].iAddrName]), &(pstTCPCltDef[i]), sizeof(IDSNetworkFrameAddrDefine));

		/*Begin Added by Frank.cheng at 2015.01.04*/
		//默认设置为TRUE ，不需要重连的，需调用IDSNetworkFrameSetTCPClientReconnectOnError来设置
		pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pstTCPCltDef[i].iAddrName].bReconnectOnError = TRUE;
		
		/*End Added by Frank.cheng at 2015.01.04*/

		if (pstConfig->iLogLevel >= 4)
		{
			LOG("DEBUG:need to Connect to %s:%d, iTCPCltName = %d", 
				pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pstTCPCltDef[i].iAddrName].sServerIP,
				pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pstTCPCltDef[i].iAddrName].nServerPort, 
				pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pstTCPCltDef[i].iAddrName].iAddrName);
		}
    }

	memset(pstDstCallback, 0, sizeof(ids_network_frame_callback));
    if(SetIDSNetworkFrameCallback(pstDstCallback, pstCallback))
    {
        LOG("FATAL: SetCallback failed!");
        return -1;
    }
	pstConfig->iTimeoutMSec = 100; // 超时值默认设置为100毫秒
	pstConfig->iShortLinkTimeOutSec = TCP_SHORTLINK_TIMEOUT; //默认一秒

    {
        struct rlimit rlim;

        getrlimit(RLIMIT_NOFILE, &rlim);
        rlim.rlim_max = rlim.rlim_cur = pstConfig->iTotalSocketNum;
        if(setrlimit(RLIMIT_NOFILE, &rlim))
        {
            LOG("setrlimit failed!\n");
        }
    }

    if(!(astSocketContext = malloc(sizeof(*astSocketContext) * pstConfig->iTotalSocketNum)))
    {
        LOG("FATAL: Out of memory!");
        return -1;
    }
    pstConfig->astSocketContext = astSocketContext;
    for(i = 0; i < pstConfig->iTotalSocketNum; i++)
    {
        astSocketContext[i].Stat = SOCKET_UNUSED;
    }

    if(!(pstConfig->pstIOMPMng = ids_iomp_mng_init(pstConfig->pstLog, pstConfig->iLogLevel, pstConfig->iTotalSocketNum)))
    {
        LOG("FATAL: ids_iomp_mng_init failed!");
        return -1;
    }

    if(ids_network_frame_create_listen_socket())
    {
        LOG("ids_network_frame_create_listen_socket failed!");
        return -1;
    }

    if(ids_network_frame_connect_to_tcp_all_server())
    {
        LOG("CreateClientSock failed!");
        return -1;
    }

	iRet = AddParentTcpAcceptSocket(iParentTcpListenNum, pstParentListenDefEx);
	if(iRet)
	{
		LOG("AddParentTcpAcceptSocket Error, iRet = %d", iRet);
		return -1;
	}
	
    return 0;
}

static int ids_network_frame_create_tcp_listen_socket(struct in_addr *pstListenAddr, unsigned short ushPort)
{
	LOG("********* %s **********", __FUNCTION__);

    struct sockaddr_in stAddr;
    int s = 0;
    int flags = 1;              //nonblock reusaddr

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1)
    {
        LOG("Error opening socket - %s\n", strerror(errno));
        return -1;
    }
    if(ioctl(s, FIONBIO, &flags) && ((flags = fcntl(s, F_GETFL, 0)) < 0 || fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0))
    {
        close(s);
        return -1;
    }

    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags)) == -1)
    {
        LOG("setsockopt failed SO_REUSEADDR (%s)!", ERRSTR);
    }

    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr = *pstListenAddr;
    stAddr.sin_port = htons(ushPort);
    if(bind(s, (struct sockaddr *) &stAddr, sizeof(stAddr)) == -1)
    {
        LOG("bind %s:%d failed (%s)!", inet_ntoa(stAddr.sin_addr), ntohs(stAddr.sin_port), ERRSTR);
        return -2;
    }
    if(listen(s, 1024) == -1)
    {
        LOG("listen failed (%s)!", ERRSTR);
        return -3;
    }
    return s;
}

static int ids_network_frame_create_tcp_client_socket(IDSNetworkFrameAddrDefine* pstClient)
{
	LOG("********* %s **********", __FUNCTION__);

    struct sockaddr_in stAddr;
    int s = 0;
    int iFlags;                 //nonblock reusaddr

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1)
    {
        LOG("Error opening socket - %s\n", strerror(errno));
        return -1;
    }

    iFlags = fcntl(s, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    fcntl(s, F_SETFL, iFlags);

    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(pstClient->nServerPort);
    if (inet_pton(AF_INET, pstClient->sServerIP, &(stAddr.sin_addr)) <= 0)
    {
        LOG("ip [%s] is invalid.", pstClient->sServerIP);
        return -1;
    }

    if(connect(s, (struct sockaddr *) &stAddr, sizeof(stAddr)) < 0)
    {
        if(errno != EINPROGRESS)
        {
            LOG("%s: connect error, errno=%d", __FUNCTION__, errno);
            close(s);
            return -1;
        }
    }
    return s;
}

int ids_network_frame_create_udp_socket(struct in_addr *pstAddr, unsigned short ushPort)
{
	LOG("********* %s **********", __FUNCTION__);

    struct sockaddr_in stAddr;
    int s = 0;
    int flags = 1;              //nonblock reusaddr
    int iBufLen = 1000 * 1024;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(s == -1)
        return -1;
    if(ioctl(s, FIONBIO, &flags) && ((flags = fcntl(s, F_GETFL, 0)) < 0 || fcntl(s, F_SETFL, flags | O_NONBLOCK) < 0))
    {
        close(s);
        return -1;
    }

    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof(flags)) == -1)
    {
        LOG("setsockopt failed SO_REUSEADDR (%s)!", ERRSTR);
    }

    if(setsockopt(s, SOL_SOCKET, SO_RCVBUF, &iBufLen, sizeof(iBufLen)) == -1)
    {
        LOG("setsockopt failed SO_RCVBUF (%s)!", ERRSTR);
    }
    if(setsockopt(s, SOL_SOCKET, SO_SNDBUF, &iBufLen, sizeof(iBufLen)) == -1)
    {
        LOG("setsockopt failed SO_RCVBUF (%s)!", ERRSTR);
    }

/*Begin added by Frank.cheng at 2015.03.05*/
//允许传端口为0，用于客户端侧，如果传进来的端口号为0 ，则不绑定
    if(0 == ushPort)
    {
        return s;
    }
/*End added by Frank.cheng at 2015.03.05*/

    memset(&stAddr, 0, sizeof(stAddr));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr = *pstAddr;
    stAddr.sin_port = htons(ushPort);
    if(bind(s, (struct sockaddr *) &stAddr, sizeof(stAddr)) == -1)
    {
        LOG("bind %s:%d failed (%s)!", inet_ntoa(stAddr.sin_addr), ntohs(stAddr.sin_port), ERRSTR);
        return -2;
    }
    //setsockopt设置缓冲区大小最大只能256k，通过改系统设置可以达到4m，减少丢包
   
    return s;
}

static int ids_network_frame_create_listen_socket(void)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    ids_socket_context *pContext = NULL;
    int iSocket = 0;
    int i;
    struct in_addr stListenAddr;

    for(i = 0; i < MAX_SERVER_PORT; i++)
    {
        if(pstConfig->stTCPSvrList[i].nServerPort == 0)
        {
            continue;
        }
        if (inet_aton(pstConfig->stTCPSvrList[i].sServerIP, &(stListenAddr)) == 0)
        {
            LOG("FATAL: Invalid IP %s!", pstConfig->stTCPSvrList[i].sServerIP);
            return -1;
        }

        iSocket = ids_network_frame_create_tcp_listen_socket(&(stListenAddr), pstConfig->stTCPSvrList[i].nServerPort);
        if(iSocket < 0)
        {
            LOG("ids_network_frame_create_tcp_listen_socket failed!");
            return -3;
        }

        pContext = &(astSocketContext[iSocket]);
        pContext->Stat = SOCKET_TCP_LISTEN;
        pContext->iSocket = iSocket;
        pContext->iAddrName = pstConfig->stTCPSvrList[i].iAddrName;
        pContext->iPkgHeadLen = pstConfig->stTCPSvrList[i].iPkgHeadLen;

        if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
        {
            LOG("FATAL: ids_iomp_mng_add_fd failed!");
            return -1;
        }
    }

    for(i = 0; i < MAX_SERVER_PORT; i++)
    {

    /*Begin modified by Frank.cheng at 2015.03.05*/
        //允许传端口为0，用于客户端侧
        //if(pstConfig->stUDPSvrList[i].nServerPort == 0)
        //{
        //    continue;
        //}

        if(pstConfig->stUDPSvrList[i].sServerIP[0] == '\0')
        {
            continue;
        }
    /*End modified by Frank.cheng at 2015.03.05*/

        if (inet_aton(pstConfig->stUDPSvrList[i].sServerIP, &(stListenAddr)) == 0)
        {
            LOG("FATAL: Invalid IP %s!", pstConfig->stUDPSvrList[i].sServerIP);
            return -1;
        }

        iSocket = ids_network_frame_create_udp_socket(&(stListenAddr), pstConfig->stUDPSvrList[i].nServerPort);
        if(iSocket < 0)
        {
            LOG("ids_network_frame_create_udp_socket failed!");
            return -3;
        }

        pstConfig->iUDPSocketList[i] = iSocket;
        pContext = &(astSocketContext[iSocket]);
        pContext->Stat = SOCKET_UDP;
        pContext->iSocket = iSocket;
        pContext->iAddrName = pstConfig->stUDPSvrList[i].iAddrName;
        pContext->iPkgHeadLen = pstConfig->stUDPSvrList[i].iPkgHeadLen;

        if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
        {
            LOG("FATAL: ids_iomp_mng_add_fd failed!");
            return -1;
        }
    }


    return 0;
}

static int ids_network_frame_connect_to_one_tcp_server(IDSNetworkFrameAddrDefine* pstClient, ids_socket_context ** ppContext)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    int iSocket = 0;
    iSocket = ids_network_frame_create_tcp_client_socket(pstClient);
    if(iSocket < 0)
    {
        LOG("ids_network_frame_create_tcp_client_socket failed!");
        return -1;
    }
    if(iSocket >= pstConfig->iTotalSocketNum)
    {
        close(iSocket);
        return -1;
    }

    *ppContext = &(astSocketContext[iSocket]);
    (*ppContext)->Stat = SOCKET_TCP_CONNECTING;
    (*ppContext)->iSocket = iSocket;
    (*ppContext)->tLastAccessTime = time(NULL);
    (*ppContext)->tCreateTime = time(NULL);
    (*ppContext)->iBytesRecved = 0;
    (*ppContext)->iBytesToSend = 0;
    (*ppContext)->iPkgLen = 0;
    (*ppContext)->iAddrName = pstClient->iAddrName;
    (*ppContext)->stClientAddr.sin_family = AF_INET;
    (*ppContext)->stClientAddr.sin_port = htons(pstClient->nServerPort);
    (*ppContext)->iPkgHeadLen = pstClient->iPkgHeadLen;
    if (inet_pton(AF_INET, pstClient->sServerIP, &((*ppContext)->stClientAddr.sin_addr)) <= 0)
    {
        LOG("BUG: double check: ip [%s] is invalid.", pstClient->sServerIP);
        return -1;
    }

    pstConfig->stIDSInnerCtlDefNode.iSocket[pstClient->iAddrName] = iSocket;
    pstConfig->stIDSInnerCtlDefNode.tLastConnectTime[pstClient->iAddrName] = time(NULL);

    if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, iSocket, IDS_IOMP_FLAG_WRITE | IDS_IOMP_FLAG_ERROR) < 0)
    {
        LOG("FATAL: ids_iomp_mng_add_fd failed!");
        close(iSocket);
        return -1;
    }
    return 0;
}

int IDSNetworkFrameSetTcpLinkToOneSvr(IDSNetworkFrameAddrDefine* pstClient)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
	ids_socket_context *ppContext = NULL;
    int iSocket = 0;

	if(0 > AddOneTcpClientDef(pstClient))
	{
		LOG("FATAL: AddOneTcpClientDef Error");
		return -1;
	}
	
    iSocket = ids_network_frame_create_tcp_client_socket(pstClient);
    if(iSocket < 0)
    {
        LOG("ids_network_frame_create_tcp_client_socket failed!");
        return -1;
    }
    if(iSocket >= pstConfig->iTotalSocketNum)
    {
        close(iSocket);
        return -1;
    }

    ppContext = &(astSocketContext[iSocket]);
    ppContext->Stat = SOCKET_TCP_CONNECTING;
    ppContext->iSocket = iSocket;
    ppContext->tLastAccessTime = time(NULL);
    ppContext->tCreateTime = time(NULL);
    ppContext->iBytesRecved = 0;
    ppContext->iBytesToSend = 0;
    ppContext->iPkgLen = 0;
    ppContext->iAddrName = pstClient->iAddrName;
    ppContext->stClientAddr.sin_family = AF_INET;
    ppContext->stClientAddr.sin_port = htons(pstClient->nServerPort);
    ppContext->iPkgHeadLen = pstClient->iPkgHeadLen;
    if (inet_pton(AF_INET, pstClient->sServerIP, &(ppContext->stClientAddr.sin_addr)) <= 0)
    {
        LOG("BUG: double check: ip [%s] is invalid.", pstClient->sServerIP);
        return -1;
    }

    pstConfig->stIDSInnerCtlDefNode.iSocket[pstClient->iAddrName] = iSocket;
    pstConfig->stIDSInnerCtlDefNode.tLastConnectTime[pstClient->iAddrName] = time(NULL);

    if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, iSocket, IDS_IOMP_FLAG_WRITE | IDS_IOMP_FLAG_ERROR) < 0)
    {
        LOG("FATAL: ids_iomp_mng_add_fd failed!");
        close(iSocket);
        return -1;
    }
    return 0;
}

static int ids_network_frame_connect_to_tcp_all_server(void)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *pContext = NULL;
    int iSocket = 0;
    int i, iTmpCltNum;

    for(i = 0, iTmpCltNum=0; MAX_CLTDEF>i && iTmpCltNum<pstConfig->stIDSInnerCtlDefNode.iCltDefNum; i++)
    {
        if(0==(pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].nServerPort
           || 7>strlen((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].sServerIP))
        {
            continue;
        }
        iTmpCltNum++;
        if (ids_network_frame_connect_to_one_tcp_server(&((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i]), &pContext) < 0)
        {
            LOG("ids_network_frame_connect_to_one_tcp_server error.");
            continue;
        }
		if (4 <= pstConfig->iLogLevel)
		{
			LOG("DEBUG:try Connect to %s:%d, waiting to ok, socket = %d, i = %d, lastconnecttime=%s.",
					pstConfig->stIDSInnerCtlDefNode.stCltDefArr[i].sServerIP,
					pstConfig->stIDSInnerCtlDefNode.stCltDefArr[i].nServerPort, iSocket, i, 
					DateTimeStr1(& pstConfig->stIDSInnerCtlDefNode.tLastConnectTime[i], NULL));
		}
    }
    return 0;
}

static int ids_network_frame_close_connection(ids_socket_context * pContext, void *pUserInfo, char *sErr, ids_close_conn_flag eFlag)
{
	LOG("********* %s **********", __FUNCTION__);

	if(NULL != sErr)
	{
		LOG("********* %s ********** sErr=%s", __FUNCTION__, sErr);
	}


    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);

    if(!pContext)
    {
        LOG("ids_socket_context is NULL!!!!!!!!!!!!!!!");
        return -1;
    }
	pstCallback->IDSNetworkFrameHandleCloseConn((_ids_socket_context *)pContext, pUserInfo, pContext->iAddrName, sErr);

    if(SOCKET_TCP_ACCEPT!=pContext->Stat && SOCKET_TCP_CONNECTED!=pContext->Stat && SOCKET_TCP_CONNECTING!=pContext->Stat)
    {
        if(SOCKET_UNUSED==pContext->Stat)
        {
            LOG("BUG: unused socket! sErr:%s", sErr);
        }
        else
        {
            LOG("maybe error: not tcp accept! sErr:%s", sErr);
        }
    }

    if(SOCKET_TCP_CONNECTED==pContext->Stat || SOCKET_TCP_CONNECTING==pContext->Stat)
    {
        if(pstConfig->stIDSInnerCtlDefNode.iSocket[pContext->iAddrName] != pContext->iSocket)
        {
            LOG("DEBUG: context stat is %d, but cannot find its define.", pContext->Stat);
        }
		if(ids_clear_config == eFlag)
		{
			pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pContext->iAddrName].nServerPort = 0;
			snprintf(pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pContext->iAddrName].sServerIP, 
				sizeof(pstConfig->stIDSInnerCtlDefNode.stCltDefArr[pContext->iAddrName].sServerIP), 
				"%s", "1.1.1.1");

			if(pContext->iAddrName >= pstConfig->stIDSInnerCtlDefNode.iCltDefNum)
			{
				pstConfig->stIDSInnerCtlDefNode.iCltDefNum --;
			}
		}
    }
    else if(SOCKET_TCP_ACCEPT == pContext->Stat)
    {
        pstConfig->iCurAcceptCltNum--;
    }
    pContext->Stat = SOCKET_UNUSED;

    if(0 > ids_iomp_mng_del_fd(pstConfig->pstIOMPMng, pContext->iSocket))
    {
        LOG("ids_iomp_mng_del_fd failed! sErr:%s", sErr);
    }
    if(close(pContext->iSocket))
    {
        LOG("close socket error (%s)! sErr:%s", ERRSTR, sErr);
    }

    pContext->iSocket = 0;
    if(sErr)
    {
        LOG("close socket. sErr:%s", sErr);
    }
    return 0;
}

int IDSNetworkFrameCloseTcpConnection(_ids_socket_context * pstSctx, ids_close_conn_flag eFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    _ids_socket_context *pstSctxCheck;
    void *pUserInfo;

    if(!pstSctx)
    {
        LOG_BUG_RET(-1);
    }
    if(IDSNetworkFrameGetContext(pstSctx->iSocket, &pstSctxCheck, &pUserInfo) < 0)
    {
        LOG_BUG_RET(-1);
    }
    if(pstSctxCheck != pstSctx)
    {
        LOG_BUG_RET(-1);
    }
    return (ids_network_frame_close_connection((ids_socket_context *) pstSctx, pUserInfo, __FUNCTION__, eFlag));
}

static int IDSNetworkFrameProcessAccept(ids_socket_context * pListenContext, void *pListenUserInfo)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);
    int s = 0;
    static struct sockaddr_in stAddr;
    int iLen = sizeof(stAddr);
    ids_socket_context *pNewContext = NULL;
    void *pNewUserInfo = NULL;

    if(!pListenContext)
    {
        LOG("BUG: Parameter error!");
        return -1;
    }
    if(pListenContext->Stat != SOCKET_TCP_LISTEN)
    {
        LOG("socket stat error!");
        return -1;
    }

    if(pstConfig->iCurAcceptCltNum >= pstConfig->iMaxAcceptSocketNum)
    {
        return -1;
    }

    s = accept(pListenContext->iSocket, (struct sockaddr *) &stAddr, &iLen);
	LOG("accept return %d!", s);
    if(s == -1)
    {
        if(errno == EAGAIN || errno == EINTR)
        {
            return 0;
        }
        LOG("Something wrong in accept (%s). Check it!", ERRSTR);
        return -1;
    }
    if(s >= pstConfig->iTotalSocketNum)
    {
        close(s);
        return -1;
    }

    pNewContext = &(astSocketContext[s]);
    pNewUserInfo = pstConfig->aUserInfo + s * pstConfig->iUserInfoLen;

    pNewContext->Stat = SOCKET_TCP_ACCEPT;
    pNewContext->iSocket = s;
    memcpy(&(pNewContext->stClientAddr), &stAddr, sizeof(stAddr));
    pNewContext->iBytesRecved = 0;
    pNewContext->iBytesToSend = 0;
    pNewContext->iPkgLen = 0;
    pNewContext->tLastAccessTime = pNewContext->tCreateTime = time(NULL);
    pNewContext->iAddrName = pListenContext->iAddrName;
    pNewContext->iPkgHeadLen = pListenContext->iPkgHeadLen;

    if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, pNewContext->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
    {
        LOG("ids_iomp_mng_add_fd failed!");
        ids_network_frame_close_connection(pNewContext, pNewUserInfo, "ids_iomp_mng_add_fd failed!", ids_save_config);
    }

    if(pstCallback->IDSNetworkFrameHandleAccept((_ids_socket_context *) pNewContext, pNewUserInfo, pListenContext->iAddrName) < 0)
    {
        ids_network_frame_close_connection(pNewContext, pNewUserInfo, "IDSNetworkFrameHandleAccept failed!", ids_save_config);
    }

    pstConfig->iCurAcceptCltNum++;

    return 0;
}

static int IDSNetworkFrameProcessTCPRead(ids_socket_context * pContext, void *pUserInfo)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);
    int iRet = 0;

    if(!pContext)
    {
        LOG("BUG: Parameter error!");
        return -1;
    }
    if(pContext->Stat != SOCKET_TCP_ACCEPT && pContext->Stat != SOCKET_TCP_CONNECTED)
    {
        LOG("socket stat error!");
        return -1;
    }
    iRet = recv(pContext->iSocket, pContext->RecvBuf + pContext->iBytesRecved,
                sizeof(pContext->RecvBuf) - pContext->iBytesRecved, 0);
    if(iRet < 0)
    {
        ids_network_frame_close_connection(pContext, pUserInfo, "IDSNetworkFrameProcessTCPRead recv() failed", ids_save_config);
        return -1;
    }
    if (iRet == 0)
    {
		if (pstConfig->iLogLevel >= 4)
		{
			ids_network_frame_close_connection(pContext, pUserInfo, "read 0 bytes from socket, may be the link is down", ids_save_config);
		} else {
			ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
		}
        return 0;
    }

    pContext->iBytesRecved += iRet;

    for(;;)
    {
        int iPkgLen;

        if(pContext->iPkgLen == 0)
        {   // 未获取包长度
            if (pContext->iBytesRecved < pContext->iPkgHeadLen)
            {
                break;// 未收全包头
            }

            // 获取包长度
            if(pstCallback->IDSNetworkFrameHandlePkgHead((_ids_socket_context *) pContext, pUserInfo, pContext->iAddrName, pContext->RecvBuf, pContext->iBytesRecved,
                             &pContext->iPkgLen) < 0)
            {
                ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
                return -1;
            }
            if (pContext->iPkgLen < pContext->iPkgHeadLen)
            {
                ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
                return -1;
            }
        }

        iPkgLen = pContext->iPkgLen;
        if(iPkgLen > sizeof(pContext->RecvBuf))
        {
            ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
            return -1;
        }

        if(pContext->iBytesRecved < iPkgLen)
        {
            break;
        }   //未收全整个包

        // 已收全整个包
        if(pstCallback->IDSNetworkFrameHandlePkg((_ids_socket_context *) pContext, pUserInfo, pContext->iAddrName, pContext->RecvBuf, iPkgLen) < 0)
        {
            ids_network_frame_close_connection(pContext, pUserInfo, "HandlePkg failed!", ids_save_config);
            return -1;
        }

        pContext->iPkgLen = 0;
        pContext->iBytesRecved -= iPkgLen;
        if(pContext->iBytesRecved == 0)
        {
            break;
        }
        if(pContext->iBytesToSend < 0)
        {
            LOG("BUG");
            return -1;
        }

        memmove(pContext->RecvBuf, pContext->RecvBuf + iPkgLen, pContext->iBytesRecved);
    }

    return 0;
}

static int IDSNetworkFrameProcessUDPRead(ids_socket_context * pContext, void *pUserInfo, int iUDPName)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);
    int iRet = 0, iAddrLen = sizeof(pContext->stClientAddr);

    if(!pContext)
    {
        LOG("BUG: Parameter error!");
        return -1;
    }
    if(pContext->Stat != SOCKET_UDP)
    {
        LOG("socket stat error!");
        return -1;
    }

    iRet = recvfrom(pContext->iSocket, pContext->RecvBuf, sizeof(pContext->RecvBuf), 0,
                    (struct sockaddr *) &(pContext->stClientAddr), &iAddrLen);
    if(iRet <= 0)
    {
        return -1;
    }

    if(pstCallback->IDSNetworkFrameHandleUdpPkg((_ids_socket_context *) pContext, pUserInfo, iUDPName, pContext->RecvBuf, iRet) < 0)
    {
        LOG("IDSNetworkFrameHandleUdpPkg failed!");
        return -1;
    }

    return 0;
}

static int ids_network_frame_process_tcp_connect(ids_socket_context * pContext, void *pUserInfo)
{
	LOG("********* %s **********", __FUNCTION__);

    int iSockErr, iSockErrLen = sizeof(iSockErr);
    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);
    
    if(pstConfig->stIDSInnerCtlDefNode.iSocket[pContext->iAddrName] != pContext->iSocket)
    {
        ids_network_frame_close_connection(pContext, pUserInfo, "context and add define is not fix", ids_save_config);
        return -1;
    }

    if(getsockopt(pContext->iSocket, SOL_SOCKET, SO_ERROR, &iSockErr, &iSockErrLen))
    {
        ids_network_frame_close_connection(pContext, pUserInfo, "getsockopt failed", ids_save_config);
        return -1;
    }
    if(iSockErr)
    {
    	LOG("SOL_SOCKET, SO_ERROR output iSockErr=%d", iSockErr);
        ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
        return -2;
    }
    pContext->Stat = SOCKET_TCP_CONNECTED;
    pContext->iBytesRecved = 0;
    pContext->iBytesToSend = 0;
    pContext->iPkgLen = 0;
    pContext->tLastAccessTime = pContext->tCreateTime = time(NULL);
    if(ids_iomp_mng_mod_fd(pstConfig->pstIOMPMng, pContext->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
    {
        LOG("ids_iomp_mng_mod_fd failed!");
    }
    if(pstCallback->IDSNetworkFrameHandleConnect((_ids_socket_context *) pContext, pUserInfo, pContext->iAddrName) < 0)
    {
        ids_network_frame_close_connection(pContext, pUserInfo, "IDSNetworkFrameHandleConnect failed!", ids_save_config);
        return -1;
    }
    return 0;
}

static int ids_network_frame_check_tcp_connection()
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    ids_socket_context *pContext = NULL;
    void *pUserInfo = NULL;
    int i = 0, iTmpCltNum = 0;

    for(i = 0, iTmpCltNum = 0; i < MAX_CLTDEF && iTmpCltNum < pstConfig->stIDSInnerCtlDefNode.iCltDefNum; i++)
    {
        if((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].nServerPort <= 0
           || strlen((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].sServerIP) < 7 /*1.1.1.1 */ )
        {
            continue;
        }
        iTmpCltNum++;
        //根据保存的isocket找到context，并验证
        if (pstConfig->stIDSInnerCtlDefNode.iSocket[i] > 0)
        {
            pContext = &(astSocketContext[pstConfig->stIDSInnerCtlDefNode.iSocket[i]]);
            pUserInfo = pstConfig->aUserInfo + pContext->iSocket * pstConfig->iUserInfoLen;
        }
        else
        {
            pContext = NULL;
            pUserInfo = NULL;
        }

        if(!pContext || !pUserInfo || pContext->iAddrName != i || pContext->iSocket != pstConfig->stIDSInnerCtlDefNode.iSocket[i])
        {
            //需要重连
            if(time(NULL) - pstConfig->stIDSInnerCtlDefNode.tLastConnectTime[i] > TIME_RECONNECT)
            {

			    /*Begin added by Frank.cheng at 2014.12.16*/
				 if((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].bReconnectOnError != TRUE
		            )
		        {
		        	LOG("pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i].bReconnectOnError != TRUE do nothing");
		            continue;
		        }
				
				/*End added by Frank.cheng at 2014.12.16*/
		
                if (ids_network_frame_connect_to_one_tcp_server(&((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[i]), &pContext) < 0)
                {
                    LOG("reconnect to tcp server fail.");
                    continue;
                }
            }
        }
        else
        {
            //处理链接超时的
            if(pContext->Stat != SOCKET_TCP_CONNECTED)
            {
                if(time(NULL) - pContext->tLastAccessTime > TIMEOUT_CONNECT)
                {
                    ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
                }
            }
        }
    }
    return 0;
}

static int ids_network_frame_tcp_send_pkg(ids_socket_context * pContext, void *pUserInfo)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    int iRet = 0;

    if(!pContext)
    {
        LOG("BUG: Parameter error!");
        return -1;
    }
    if(pContext->Stat != SOCKET_TCP_ACCEPT && pContext->Stat != SOCKET_TCP_CONNECTED)
    {
        if(pContext->Stat != SOCKET_UNUSED)
        {
            LOG("Error stat (socket %d, Stat %d)!", pContext->iSocket, pContext->Stat);
            return -1;
        }
        return 0;
    }

    pContext->tLastAccessTime = time(NULL);
    iRet = send(pContext->iSocket, pContext->SendBuf, pContext->iBytesToSend, 0);
    if(iRet == 0)
    {
        return 0;
    }
    if(iRet < 0)
    {
        if(errno == EAGAIN || errno == EINTR)
        {
            return 0;
        }
        ids_network_frame_close_connection(pContext, pUserInfo, "send failed!", ids_save_config);
        return -1;
    }

    if(iRet >= pContext->iBytesToSend)
    {
        pContext->iBytesToSend = 0;
        if(ids_iomp_mng_mod_fd(pstConfig->pstIOMPMng, pContext->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
        {
            LOG("ids_iomp_mng_mod_fd failed!");
        }
        return 0;
    }

    pContext->iBytesToSend -= iRet;
    memmove(pContext->SendBuf, pContext->SendBuf + iRet, pContext->iBytesToSend);
    return 0;
}

int IDSNetworkFrameLoop(void)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_network_frame_callback *pstCallback = &(pstConfig->stCallback);
    int i, iTriggered;
    ids_socket_context *astSocketContext=pstConfig->astSocketContext;

    pstConfig->iCurAcceptCltNum = 0;
    for(;;)
    {
        int iPos, iFd, iFlag;
        pstCallback->IDSNetworkFrameHandleLoop();
        ids_network_frame_check_tcp_connection();

        if ((iTriggered = ids_iomp_mng_wait_for_fd(pstConfig->pstIOMPMng, pstConfig->iTimeoutMSec)) < 0)
        {
            continue;
        }

        iPos = 0;
        while((iFd = ids_iomp_mng_get_ready_fd(pstConfig->pstIOMPMng, &iPos, &iFlag)) >= 0)
        {
            ids_socket_context *pContext;
            void *pUserInfo;

            pContext = &(astSocketContext[iFd]);
            pUserInfo = pstConfig->aUserInfo + iFd * pstConfig->iUserInfoLen;

            if(pContext->iSocket != iFd)
            {
                LOG("BUG: iFd is %d, iSocket is %d!", iFd, pContext->iSocket);
                continue;
            }
            if ((iFlag & IDS_IOMP_FLAG_ERROR) && errno != EINPROGRESS)
            {
                LOG("error:%s\n", strerror(errno));
                ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
                continue;
            }

            if(iFlag & IDS_IOMP_FLAG_READ)
            {
                if(pContext->Stat == SOCKET_TCP_LISTEN)
                {
                    IDSNetworkFrameProcessAccept(pContext, pUserInfo);
                    continue;
                }
                if(pContext->Stat == SOCKET_TCP_ACCEPT || pContext->Stat == SOCKET_TCP_CONNECTED)
                {
                    pContext->tLastAccessTime = time(NULL);
                    IDSNetworkFrameProcessTCPRead(pContext, pUserInfo);
                }
                if(pContext->Stat == SOCKET_UDP)
                {
                    int j;

                    for(j = 0; j < MAX_SERVER_PORT; j++)
                    {
                        if(pstConfig->iUDPSocketList[j] == iFd)
                        {
                            break;
                        }
                    }
                    if(j >= MAX_SERVER_PORT)
                    {
                        LOG("Bug: Can't find UDP socket %d!", iFd);
                        continue;
                    }
                    IDSNetworkFrameProcessUDPRead(pContext, pUserInfo, pstConfig->stUDPSvrList[j].iAddrName);
                }
            }

            if(iFlag & IDS_IOMP_FLAG_WRITE)
            {
                if(pContext->Stat == SOCKET_TCP_CONNECTING)
                {
                    ids_network_frame_process_tcp_connect(pContext, pUserInfo);
                }
                else
                {
                    ids_network_frame_tcp_send_pkg(pContext, pUserInfo);
                }
            }
        }
    }

    return 0;
}

/*Begin added by Frank.cheng at 2014.12.30*/
int IDSNetworkFrameSendToConnectedTCPClient(int iAcceptAddrName,enum ids_enum_send_to_tcp_client_num eNum
																			, void *pUserInfo, void *pPkg, int iPkgLen)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	ids_socket_context *astSocketContext = pstConfig->astSocketContext;
	int iSendClientCount = 0;
	int i = 0;
	//for(i=0; i < pstConfig->iCurAcceptCltNum; i++)
    for(i = 0; i < pstConfig->iTotalSocketNum; i++) //Fixme please 效率太低
	{
		ids_socket_context * pstSctx = (&astSocketContext[i]);
		//LOG("astSocketContext[%d]->Stat=[%d]", i, pstSctx->Stat);
		if (pstSctx->iAddrName != iAcceptAddrName)
		{
			continue;
		}
		if (pstSctx->Stat != SOCKET_TCP_ACCEPT)
		{
			continue;
		}
		iSendClientCount ++;
		IDSNetworkFrameSend((_ids_socket_context *) pstSctx, pUserInfo, pPkg, iPkgLen);
		if (eNum == E_SEND_TO_TCP_CLIENT_ONLY_ONE)
		{
			break;
		}
	}
	LOG("iSendClientCount=%d", iSendClientCount);
	return 0;
}

int IDSNetworkFrameSetTCPClientReconnectOnError(int iClientAddrName,BOOL bValue)
{
	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	if(MAX_CLTDEF <= iClientAddrName || 0 >  iClientAddrName)
	{
		LOG("FATAL: iClientAddrName %d is out of [%d, %d)!", iClientAddrName, 0, MAX_CLTDEF);
		return -1;
	}

	pstConfig->stIDSInnerCtlDefNode.stCltDefArr[iClientAddrName].bReconnectOnError = bValue;

	return 0;
}

/*End added by Frank.cheng at 2014.12.30*/

int IDSNetworkFrameSend(_ids_socket_context * pstSctx, void *pUserInfo, void *pPkg, int iPkgLen)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    int iRet = 0;
    ids_socket_context *pContext = (ids_socket_context *) pstSctx;

    if(!pContext)
    {
        LOG("BUG: Parameter error!");
        return -1;
    }

	//LOG("pContext->Stat=%d", pContext->Stat);
	
    //如果是tcp客户端发送消息，但是断连了，等待重联的过程中，发现需要发送，则马上重联
    //但是这次的发送，则返回失败。仅仅触发重联而已
    if (SOCKET_TCP_RECONNECT_WAIT == pContext->Stat)
    {
        int iAddrName = pContext->iAddrName;
        //先关闭原来的，再重连
        ids_network_frame_close_connection(pContext, pUserInfo, __FUNCTION__, ids_save_config);
        ids_network_frame_connect_to_one_tcp_server(&((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[iAddrName]), &pContext);
		LOG("SOCKET_TCP_RECONNECT_WAIT == pContext->Stat!");

        return -1;
    }
    
    if(pContext->Stat != SOCKET_TCP_ACCEPT && pContext->Stat != SOCKET_TCP_CONNECTED)
    {
        if(pContext->Stat != SOCKET_UNUSED)
        {
            LOG("Error stat (socket %d, Stat %d)!", pContext->iSocket, pContext->Stat);
            return -1;
        }

		
		LOG("pContext->Stat != SOCKET_TCP_ACCEPT && pContext->Stat != SOCKET_TCP_CONNECTED");
        return 0;
    }
    if(pPkg == NULL || iPkgLen <= 0 || iPkgLen > sizeof(pContext->SendBuf))
    {
        LOG("Parameter error!");
        return -1;
    }

    pContext->tLastAccessTime = time(NULL);
    if(pContext->iBytesToSend > 0)
    {
        if(pContext->iBytesToSend + iPkgLen > sizeof(pContext->SendBuf))
        {
            ids_network_frame_close_connection(pContext, pUserInfo, "Send buffer overflow!", ids_save_config);

			LOG("pContext->iBytesToSend + iPkgLen > sizeof(pContext->SendBuf)");
            return -2;
        }
        memcpy(pContext->SendBuf + pContext->iBytesToSend, pPkg, iPkgLen);
		pContext->iBytesToSend += iPkgLen;

		LOG("pContext->iBytesToSend > 0");
        return 0;
    }

    iRet = send(pContext->iSocket, pPkg, iPkgLen, 0);
    //LOG("SEND:\n%s", DumpPackage(pPkg, iRet));
    if(iRet > 0)
    {
        if(iRet == iPkgLen)
        {
            return 0;
        }
        memcpy(pContext->SendBuf, pPkg + iRet, iPkgLen - iRet);
        pContext->iBytesToSend = iPkgLen - iRet;
        if(ids_iomp_mng_mod_fd(pstConfig->pstIOMPMng, pContext->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_WRITE | IDS_IOMP_FLAG_ERROR) < 0)
        {
            ids_network_frame_close_connection(pContext, pUserInfo, "ids_iomp_mng_mod_fd failed", ids_save_config);
            return -3;
        }
        return 0;
    }

    if(iRet < 0 && (errno == EAGAIN || errno == EINTR))
    {
        memcpy(pContext->SendBuf, pPkg, iPkgLen);
        pContext->iBytesToSend = iPkgLen;
        if(ids_iomp_mng_mod_fd(pstConfig->pstIOMPMng, pContext->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_WRITE | IDS_IOMP_FLAG_ERROR) < 0)
        {
            ids_network_frame_close_connection(pContext, pUserInfo, "ids_iomp_mng_mod_fd failed", ids_save_config);
            return -3;
        }
        return 0;
    }

    //出错
    ids_network_frame_close_connection(pContext, pUserInfo, "send failed!", ids_save_config);
    return -1;
}

int IDSNetworkFrameGetUdpSocketByName(int iUdpName)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    int iRet = 0, j, iSocket;
    ids_socket_context *pContext;


    for(j = 0; j < MAX_SERVER_PORT; j++)
    {
        if(pstConfig->stUDPSvrList[j].iAddrName == iUdpName)
        {
            break;
        }
    }
    if(j >= MAX_SERVER_PORT)
    {
        LOG("Can't find UDP name %d!", iUdpName);
        return -1;
    }
    iSocket = pstConfig->iUDPSocketList[j];

    pContext = &(astSocketContext[iSocket]);

    if(pContext->Stat != SOCKET_UDP)
    {
        LOG("Error stat! UDP name %d, socket %d.", iUdpName, iSocket);
        return -1;
    }
    if(pContext->iSocket != iSocket)
    {
        LOG("Socket mismatch!");
        return -1;
    }
    return iSocket;
}

int IDSNetworkFrameSendUdp(int iUdpName, const struct sockaddr_in *pstAddr, void *pPkg, int iPkgLen)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    int iRet = 0, j, iSocket, iAddrLen = sizeof(*pstAddr);
    ids_socket_context *pContext;

    if(pPkg == NULL || iPkgLen <= 0 || iPkgLen > sizeof(pContext->SendBuf))
    {
        LOG("Parameter error!");
        return -1;
    }

    for(j = 0; j < MAX_SERVER_PORT; j++)
    {
        if(pstConfig->stUDPSvrList[j].iAddrName == iUdpName)
        {
            break;
        }
    }
    if(j >= MAX_SERVER_PORT)
    {
        LOG("Can't find UDP name %d!", iUdpName);
        return -1;
    }
    iSocket = pstConfig->iUDPSocketList[j];

    pContext = &(astSocketContext[iSocket]);

    if(pContext->Stat != SOCKET_UDP)
    {
        LOG("Error stat! UDP name %d, socket %d.", iUdpName, iSocket);
        return -1;
    }
    if(pContext->iSocket != iSocket)
    {
        LOG("Socket mismatch!");
        return -1;
    }

    iRet = sendto(iSocket, pPkg, iPkgLen, 0, (const struct sockaddr *) pstAddr, iAddrLen);
    if(iRet != iPkgLen)
    {
        LOG("sendto(%d bytes) failed (%s)!", iPkgLen, ERRSTR);
        return -1;
    }

    return 0;
}

int IDSNetworkFrameGetContext(int iSocket, _ids_socket_context ** ppstSctx, void **ppUserInfo)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;

    if(iSocket < 0 || iSocket >= pstConfig->iTotalSocketNum)
    {
        LOG("Socket %d is invalid!", iSocket);
        return -1;
    }

    if(ppstSctx)
    {
        *ppstSctx = (_ids_socket_context *) & (astSocketContext[iSocket]);
    }
    if(ppUserInfo)
    {
        *ppUserInfo = pstConfig->aUserInfo + iSocket * pstConfig->iUserInfoLen;
    }

    return 0;
}

void IDSNetworkFrameSetWaitTimeout(int iTimeoutMSec)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;

	pstConfig->iTimeoutMSec = iTimeoutMSec;
}

int IDSNetworkFrameGetTcpClientContextbyName(int iTCPCltName, _ids_socket_context ** ppstSctx, void **ppUserInfo)
{
	//LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext, *pstSocketContext;
	int iSocket;
    if (iTCPCltName > 500 || iTCPCltName < 0)
    {
        LOG("TCP name error.must between [0, 500), now is %d", iTCPCltName);
        return -1;
    }
    //双向验证stCltDefine.iSocket == ids_socket_context.iSocket，ids_socket_context.iAddrName
    if((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[iTCPCltName].iAddrName != iTCPCltName)
	{
		LOG("iAddrName in stCltDefine.stCltDefArr[Name %d] is Name %d!",
				iTCPCltName, (pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[iTCPCltName].iAddrName);
		return -1;
	}

	iSocket = pstConfig->stIDSInnerCtlDefNode.iSocket[iTCPCltName];
	if (iSocket <= 0)
	{
		LOG("iSocket in stIDSInnerCtlDefNode.iSocket[Name %d] <= 0!", iTCPCltName);
		return -1;
	}

	pstSocketContext = & (astSocketContext[iSocket]);

	//进一步验证
	if(pstSocketContext->iSocket != iSocket)
	{
		LOG("iSocket in TcpAddrName=%u astSocketContext[%d] is %d!", pstSocketContext->iAddrName, iSocket, pstSocketContext->iSocket);
		return -1;
	}

	if (pstSocketContext->iAddrName != iTCPCltName)
	{
		LOG("iAddrName in astSocketContext[AddrName %d (socket %d)] is Name %d!",
				iTCPCltName, iSocket, pstSocketContext->iAddrName);
		return -1;
	}
	if(ppstSctx)
	{
		*ppstSctx = (_ids_socket_context *) pstSocketContext;
	}
	if(ppUserInfo)
	{
		*ppUserInfo = pstConfig->aUserInfo + (*ppstSctx)->iSocket * pstConfig->iUserInfoLen;
	}
	return 0;
}

void ids_network_frame_signal_handle(int i)
{
	LOG("********* %s **********", __FUNCTION__);

}

int ids_network_frame_tcp_time_connect(const char *pszHost, u_int16_t nPort, int TimeOut)
{
	LOG("********* %s **********", __FUNCTION__);

    int sock;
    struct sockaddr_in rsock;
    struct hostent *hostinfo;
    struct in_addr *addp;

    memset((char *) &rsock, 0, sizeof(rsock));

    if((hostinfo = gethostbyname(pszHost)) == NULL)
    {
        //err_msg("Cannot find %s - %s\n",pszHost,strerror(errno));
        return -2;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        //err_msg("Can't create socket - %s\n",strerror(errno));
        return -3;
    }

    addp = (struct in_addr *) *(hostinfo->h_addr_list);
    rsock.sin_addr = *addp;
    rsock.sin_family = AF_INET;
    rsock.sin_port = htons(nPort);

    signal(SIGALRM, ids_network_frame_signal_handle);
    alarm(TimeOut);
    if(connect(sock, (struct sockaddr *) (&rsock), sizeof(rsock)) == -1)
    {
        close(sock);
        //err_msg("Can't connect %s on port %i - %s\n",pszHost,nPort,strerror(errno));
        if(errno == EINTR)
        {
            alarm(0);
            signal(SIGALRM, SIG_IGN);
            return -1;
        }
        return -4;
    }
    alarm(0);
    signal(SIGALRM, SIG_IGN);
    return sock;
}

ssize_t ids_network_frame_tcp_read(int iSockfd, char pszBuf[], size_t iLength)
{
	LOG("********* %s **********", __FUNCTION__);

    return recv(iSockfd, pszBuf, iLength, 0);
}

ssize_t ids_network_frame_tcp_time_read(int iSockfd, char pszBuf[], size_t iLength, time_t lSec)
{
	LOG("********* %s **********", __FUNCTION__);

    fd_set readfds;
    struct timeval tv;

    tv.tv_sec = lSec;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(iSockfd, &readfds);
    if(select(iSockfd + 1, &readfds, NULL, NULL, &tv) > 0)
    {
        if(FD_ISSET(iSockfd, &readfds))
        {
            return ids_network_frame_tcp_read(iSockfd, pszBuf, iLength);
        }
    }
    return -2;
}

ssize_t ids_network_frame_tcp_write(int iSockfd, const void *pszBuf, ssize_t iLength)
{
	LOG("********* %s **********", __FUNCTION__);

    int n;

    n = write(iSockfd, pszBuf, iLength);
    return (n);
}

ssize_t ids_network_frame_tcp_time_write(int iSockfd, const void *pszBuf, ssize_t iLength, time_t lSec)
{
	LOG("********* %s **********", __FUNCTION__);

    fd_set writefds;
    struct timeval tv;

    tv.tv_sec = lSec;
    tv.tv_usec = 0;

    FD_ZERO(&writefds);
    FD_SET(iSockfd, &writefds);
    if(select(iSockfd + 1, NULL, &writefds, NULL, &tv) > 0)
    {
        if(FD_ISSET(iSockfd, &writefds))
        {
            return ids_network_frame_tcp_write(iSockfd, pszBuf, iLength);
        }
    }
    return -2;  /* Time Out */
}

int IDSNetworkFrameSendTcpShortLink(char *sIp, unsigned short nPort, void *pReqPkg, int iReqLen, void *pAckPkg, int *piAckLen)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    int iSocket = -1;

    if((iSocket = ids_network_frame_tcp_time_connect(sIp, nPort, pstConfig->iShortLinkTimeOutSec)) <= -1)
    {
        return -1;
    }
    if(ids_network_frame_tcp_time_write(iSocket, pReqPkg, iReqLen, pstConfig->iShortLinkTimeOutSec) <= 0)
    {
        close(iSocket);
        return -2;
    }
    if(pAckPkg != NULL && *piAckLen > 0)
    {
        if((*piAckLen = ids_network_frame_tcp_time_read(iSocket, (char *) pAckPkg, *piAckLen, pstConfig->iShortLinkTimeOutSec)) <= 0)
        {
            close(iSocket);
            return -3;
        }
    }
    close(iSocket);
    return 0;
}

void IDSNetworkFrameSetShortLinkWaitTimeout(int iTimeoutSec)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    pstConfig->iShortLinkTimeOutSec = iTimeoutSec;
}

int IDSNetworkFrameCloseUdpSocket(int iSocket)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	ids_socket_context *astSocketContext = pstConfig->astSocketContext;
	ids_socket_context *pContext;

	pContext = &(astSocketContext[iSocket]);
	if(pContext->iSocket != iSocket)
		return -1;

	ids_iomp_mng_del_fd(pstConfig->pstIOMPMng, iSocket);
	close(iSocket);
	pContext->iSocket = 0;
	
	return 0;
}

static int AddParentTcpAcceptSocket(int iParentTcpListenNum, IDSNetworkFrameAddrDefineEx *pstParentListenDefEx)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
    ids_socket_context *astSocketContext = pstConfig->astSocketContext;
    ids_socket_context *pContext = NULL;
    int i;
	IDSNetworkFrameAddrDefineEx *pstDef = pstParentListenDefEx;

    for(i = 0; i < iParentTcpListenNum; i++)
    {
        pContext = &(astSocketContext[pstDef->iSocket]);
        pContext->Stat = SOCKET_TCP_LISTEN;
        pContext->iSocket = pstDef->iSocket;
        pContext->iAddrName = pstDef->iAddrName;
        pContext->iPkgHeadLen = pstDef->iPkgHeadLen;

        if(ids_iomp_mng_add_fd(pstConfig->pstIOMPMng, pstDef->iSocket, IDS_IOMP_FLAG_READ | IDS_IOMP_FLAG_ERROR) < 0)
        {
            LOG("FATAL: ids_iomp_mng_add_fd failed!");
            return -1;
        }

		pstDef += 1;
    }
	return 0;
}

static int AddOneTcpClientDef(IDSNetworkFrameAddrDefine *pstTCPCltDef)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	
	if(MAX_CLTDEF<=pstTCPCltDef->iAddrName|| 0>pstTCPCltDef->iAddrName)
	{
		LOG("FATAL: TCPClient Name (%d) is out of [%d, %d)!", pstTCPCltDef->iAddrName, 0, MAX_CLTDEF);
		return -1;
	}
	memcpy(&((pstConfig->stIDSInnerCtlDefNode.stCltDefArr)[pstTCPCltDef->iAddrName]), pstTCPCltDef, sizeof(IDSNetworkFrameAddrDefine));

	if(pstTCPCltDef->iAddrName > pstConfig->stIDSInnerCtlDefNode.iCltDefNum)
		pstConfig->stIDSInnerCtlDefNode.iCltDefNum ++;

	return 0;
}

int IDSNetworkFramdClearOneCliDefNode(int iTCPCltName)
{
	LOG("********* %s **********", __FUNCTION__);

	ids_network_frame_config *pstConfig = &g_stIDSNetworkFrameConfig;
	pstConfig->stIDSInnerCtlDefNode.stCltDefArr[iTCPCltName].nServerPort = 0;
		snprintf(pstConfig->stIDSInnerCtlDefNode.stCltDefArr[iTCPCltName].sServerIP, 
		sizeof(pstConfig->stIDSInnerCtlDefNode.stCltDefArr[iTCPCltName].sServerIP), 
		"%s", "1.1.1.1");

	if(iTCPCltName >= pstConfig->stIDSInnerCtlDefNode.iCltDefNum)
	{
		pstConfig->stIDSInnerCtlDefNode.iCltDefNum --;
	}
	
	return 0;
}
