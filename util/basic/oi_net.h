/*
 *  This file is provided for use with the unix-socket-faq.  It is public
 *  domain, and may be copied freely.  There is no copyright on it.  The
 *  original work was by Vic Metcalfe (vic@brutus.tlug.org), and any
 *  modifications made to that work were made with the understanding that
 *  the finished work would be in the public domain.
 *
 *  If you have found a bug, please pass it on to me at the above address
 *  acknowledging that there will be no copyright on your work.
 *
 *  The most recent version of this file, and the unix-socket-faq can be
 *  found at http://www.interlog.com/~vic/sock-faq/.
 */

#ifndef _OI_NET_H_
#define _OI_NET_H_

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

/* On some crazy systems, these aren't defined. */
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define atoport(service,proto) 			OI_atoport(service,proto)
#define atoaddr(address) 				OI_atoaddr(address)
#define get_connection(socket_type,port,listener) OI_get_connection(socket_type,port,listener)
#define make_connection(service,type,netaddress) 	OI_make_connection(service,type,netaddress)
#define sock_read(sockfd,buf,count) 	OI_sock_read(sockfd,buf,count)
#define sock_write(sockfd,buf,count) 	OI_sock_write(sockfd,buf,count)
#define sock_gets(sockfd,str,count) 	OI_sock_gets(sockfd,str,count)
#define sock_puts(sockfd,str) 			OI_sock_puts(sockfd,str)
#define CreateSockAddr(socket_type,sIP,sPort,iListener) OI_CreateSockAddr(socket_type,sIP,sPort,iListener)
#define ignore_pipe() 							OI_ignore_pipe()
#define Create_sock(socket_type,port,listener) OI_Create_sock(socket_type,port,listener)
#define SetNBlock(iSock) 							OI_SetNBlock(iSock)
#define MakeSockAddrIn(pstAddr,sIP,sPort) 			OI_MakeSockAddrIn(pstAddr,sIP,sPort)


    int OI_atoport(char *service, char *proto);
    struct in_addr *OI_atoaddr(char *address);
    int OI_get_connection(int socket_type, u_short port, int *listener);

    int OI_make_connection(char *service, int type, char *netaddress);
    int OI_make_connectionPort(int type, char *netaddress, int port);   // port 网络序

    int OI_sock_read(int sockfd, char *buf, size_t count);
    int OI_sock_write(int sockfd, const char *buf, size_t count);
    int OI_sock_gets(int sockfd, char *str, size_t count);
    int OI_sock_puts(int sockfd, const char *str);
    int OI_CreateSockAddr(int socket_type, char *sIP, char *sPort, int *iListener);

    void OI_ignore_pipe(void);

	// 创建一个socket
    int OI_Create_sock(int socket_type, u_short port, int *listener);   // port 网络序
	// 设置非阻塞
    int OI_SetNBlock(int iSock);

    // 使用 sIP:sPort 创建一个 sockaddr_in
    int OI_MakeSockAddrIn(struct sockaddr_in *pstAddr, char *sIP, char *sPort);

	//返回值 0 非内部IP， 1 内部IP， 《0  出错
	int OI_CheckInnerIp(in_addr_t dwIp);
	int OI_CheckInnerIpS(char *sIP) ;


#ifdef __cplusplus
}
#endif

#endif
