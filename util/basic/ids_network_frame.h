#ifndef _IDS_NETWORK_FRAMEWORK_H_INCLUDED_
#define _IDS_NETWORK_FRAMEWORK_H_INCLUDED_
#include <linux/stddef.h>

#define RECVBUF_LEN 8192
#define SENDBUF_LEN 8192
#define MAX_CLTDEF 5000
#define LEN_FRAMEWORK_IP 16

typedef enum
{
	ids_clear_config,
	ids_save_config
}ids_close_conn_flag;

typedef enum
{
    SOCKET_UNUSED,
    SOCKET_TCP_ACCEPT,
    SOCKET_TCP_LISTEN,
    SOCKET_TCP_CONNECTING,
    SOCKET_TCP_CONNECTED,
    SOCKET_TCP_RECONNECT_WAIT,
    SOCKET_UDP
}ids_socket_context_stat;

typedef struct tag_ids_socket_context
{
    const int iSocket;
    uint64_t ddwUID;//added by Frank.cheng
    const struct sockaddr_in stClientAddr;
    const time_t tCreateTime;
    const time_t tLastAccessTime;
    const ids_socket_context_stat Stat;
	tag_ids_socket_context()
	{
		ddwUID = 0;
	}
}_ids_socket_context;                           // 该定义必须与ids_socket_context的定义吻合

typedef struct tag_IDSNetworkFrameAddrDefine
{
    char sServerIP[LEN_FRAMEWORK_IP];
    unsigned short nServerPort;
    int iAddrName;
    int iPkgHeadLen;

	/*Begin added by Frank.cheng at 2014.12.16*/
	BOOL bReconnectOnError;

	tag_IDSNetworkFrameAddrDefine()
	{
		bReconnectOnError = TRUE;
	}
	/*End added by Frank.cheng at 2014.12.16*/;
}IDSNetworkFrameAddrDefine;

typedef struct
{
    char sServerIP[LEN_FRAMEWORK_IP];
    unsigned short nServerPort;
    int iAddrName;
    int iPkgHeadLen;
	int iSocket;
}IDSNetworkFrameAddrDefineEx;

typedef int (*IDSNetworkFrameHandlePkgFn) (_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName, void *pPkg, int iPkgLen);

typedef struct
{
    int (*IDSNetworkFrameHandlePkgHead) (_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName, void *pPkg, int iBytesRecved, int *piPkgLen);

    int (*IDSNetworkFrameHandlePkg) (_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName, void *pPkg, int iPkgLen);

    int (*IDSNetworkFrameHandleAccept) (_ids_socket_context * pstSctx, void *pUserInfo, int iListenAddrName);
    
    int (*IDSNetworkFrameHandleConnect) (_ids_socket_context * pstSctx, void *pUserInfo, int iAddrName);

    int (*IDSNetworkFrameHandleLoop) (void);
	
    int (*IDSNetworkFrameHandleUdpPkg) (_ids_socket_context * pstSctx, void *pUserInfo, int iUdpName, void *pPkg, int iPkgLen);

	int (*IDSNetworkFrameHandleCloseConn) (_ids_socket_context *pstSctx, void *pUserInfo, int iAddrName, char *sErrInfo);
}ids_network_frame_callback;

int IDSNetworkFrameInit(void *pUserInfoBuf, int iUserInfoBufLen,
                     int iUserInfoLen,
                     LogFile * pstLog, int iLogLevel,
                     int iTCPServerNum, IDSNetworkFrameAddrDefine * pstTCPSvrDef, int iMaxAcceptCltNum, 
                     int iUDPServerNum, IDSNetworkFrameAddrDefine * pstUDPSvrDef, 
                     int iTCPAsCltNum, IDSNetworkFrameAddrDefine * pstTCPCltDef,
                     int iParentTcpListenNum, IDSNetworkFrameAddrDefineEx *pstParentListenDefEx,
                     ids_network_frame_callback * pstCallback);

int IDSNetworkFrameLoop(void);
/*Begin added by Frank.cheng at 2014.12.30*/
enum ids_enum_send_to_tcp_client_num
{
		E_SEND_TO_TCP_CLIENT_ONLY_ONE,
		E_SEND_TO_TCP_CLIENT_ALL,
};

int IDSNetworkFrameSendToConnectedTCPClient(int iAcceptAddrName,enum ids_enum_send_to_tcp_client_num eNum
																			, void *pUserInfo, void *pPkg, int iPkgLen);

int IDSNetworkFrameSetTCPClientReconnectOnError(int iClientAddrName,BOOL bValue);

/*End added by Frank.cheng at 2014.12.30*/

int IDSNetworkFrameSend(_ids_socket_context * pstSctx, void *pUserInfo, void *pPkg, int iPkgLen);

int IDSNetworkFrameSendTcpShortLink(char *sIp, unsigned short nPort, void *pReqPkg, int iReqLen, void *pAckPkg, int *piAckLen);


int IDSNetworkFrameSendUdp(int iUdpName, const struct sockaddr_in *pstAddr, void *pPkg, int iPkgLen);

int IDSNetworkFrameCloseTcpConnection(_ids_socket_context * pstSctx, ids_close_conn_flag eFlag);

int IDSNetworkFrameGetContext(int iSocket, _ids_socket_context ** ppstSctx, void **ppUserInfo);

int IDSNetworkFrameGetTcpClientContextbyName(int iTCPCltName, _ids_socket_context ** ppstSctx, void **ppUserInfo);

void IDSNetworkFrameSetWaitTimeout(int iTimeoutMSec);

void IDSNetworkFrameSetShortLinkWaitTimeout(int iTimeoutSec);

int IDSNetworkFrameGetUdpSocketByName(int iUdpName);

int IDSNetworkFrameCloseUdpSocket(int iSocket);

int IDSNetworkFrameSetTcpLinkToOneSvr(IDSNetworkFrameAddrDefine* pstClient);

int IDSNetworkFramdClearOneCliDefNode(int iTCPCltName);
#endif
