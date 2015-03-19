#include "oi_net.h"

/* Take a service name, and a service type, and return a port number.  If the
   service name is not found, it tries it as a decimal number.  The number
   returned is byte ordered for the network. */


int
OI_atoport(char *service, char *proto)
{
    int port;
    long int lport;
    struct servent *serv;
    char *errpos;

    /* First try to read it from /etc/services */
    serv = getservbyname(service, proto);
    if(serv != NULL)
        port = serv->s_port;
    else
    {   /* Not in services, maybe a number? */
        lport = strtol(service, &errpos, 0);
        if((errpos[0] != 0) || (lport < 1) || (lport > 65535))
            return -1;  /* Invalid port address */
        port = htons(lport);
    }
    return port;
}

/* Converts ascii text to in_addr struct.  NULL is returned if the address
   can not be found. */
struct in_addr *
OI_atoaddr(char *address)
{
    struct hostent *host;
    in_addr_t iaddr;
    static struct in_addr saddr;

    /* First try it as aaa.bbb.ccc.ddd. */
    //saddr.s_addr = inet_addr(address);
    iaddr = inet_addr(address); //modified by diegojiang at 2011-10-10
    //if(saddr.s_addr != (size_t) - 1) 
    if(iaddr != INADDR_NONE) 
    {
        saddr.s_addr = iaddr;
        return &saddr;
    }
    host = gethostbyname(address);
    if(host != NULL)
    {
        return (struct in_addr *) *host->h_addr_list;
    }
    return NULL;
}

/* This function listens on a port, and returns connections.  It forks
   returns off internally, so your main function doesn't have to worry
   about that.  This can be confusing if you don't know what is going on.
   The function will create a new process for every incoming connection,
   so in the listening process, it will never return.  Only when a connection
   comes in, and we create a new process for it will the function return.
   This means that your code that calls it should _not_ loop.

   The parameters are as follows:
     socket_type: SOCK_STREAM or SOCK_DGRAM (TCP or UDP sockets)
     port: The port to listen on.  Remember that ports < 1024 are
       reserved for the root user.  Must be passed in network byte
       order (see "man htons").
     listener: This is a pointer to a variable for holding the file
       descriptor of the socket which is being used to listen.  It
       is provided so that you can write a signal handler to close
       it in the event of program termination.  If you aren't interested,
       just pass NULL.  Note that all modern unixes will close file
       descriptors for you on exit, so this is not required. */
int
OI_get_connection(int socket_type, u_short port, int *listener)
{
    struct sockaddr_in address;
    int listening_socket;
    int connected_socket = -1;
    int new_process;
    int reuse_addr = 1;

    /* Setup internet address information.  
       This is used with the bind() call */
    memset((char *) &address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    listening_socket = socket(AF_INET, socket_type, 0);
    if(listening_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(listener != NULL)
        *listener = listening_socket;

    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    if(bind(listening_socket, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind");
        close(listening_socket);
        exit(EXIT_FAILURE);
    }

    if(socket_type == SOCK_STREAM)
    {
        listen(listening_socket, 5);    /* Queue up to five connections before
                                           having them automatically rejected. */

        while(connected_socket < 0)
        {
            connected_socket = accept(listening_socket, NULL, NULL);
            if(connected_socket < 0)
            {
                /* Either a real error occured, or blocking was interrupted for
                   some reason.  Only abort execution if a real error occured. */
                if(errno != EINTR)
                {
                    perror("accept");
                    close(listening_socket);
                    exit(EXIT_FAILURE);
                }
                else
                {
                    continue;   /* don't fork - do the accept again */
                }
            }

            new_process = fork();
            if(new_process < 0)
            {
                perror("fork");
                close(connected_socket);
                connected_socket = -1;
            }
            else
            {   /* We have a new process... */
                if(new_process == 0)
                {
                    /* This is the new process. */
                    close(listening_socket);    /* Close our copy of this socket */
                    if(listener != NULL)
                        *listener = -1; /* Closed in this process.  We are not 
                                           responsible for it. */
                }
                else
                {
                    /* This is the main loop.  Close copy of connected socket, and
                       continue loop. */
                    close(connected_socket);
                    connected_socket = -1;
                }
            }
        }
        return connected_socket;
    }
    else
        return listening_socket;
}

int
OI_SetNBlock(int iSock)
{
    int iFlags;

    iFlags = fcntl(iSock, F_GETFL, 0);
    iFlags |= O_NONBLOCK;
    iFlags |= O_NDELAY;
    fcntl(iSock, F_SETFL, iFlags);
    return 0;
}

int
OI_CreateSockAddr(int socket_type, char *sIP, char *sPort, int *iListener)
{
    struct sockaddr_in stAddr;
    int iListenSocket;
    int iReuseAddr = 1;

    /* Setup internet address information.  
       This is used with the bind() call */
    memset((char *) &stAddr, 0, sizeof(stAddr));

    stAddr.sin_family = AF_INET;
    if(socket_type == SOCK_STREAM)
        stAddr.sin_port = htons(atoi(sPort));
    else
        stAddr.sin_port = htons(atoi(sPort));
    stAddr.sin_addr = *OI_atoaddr(sIP);

    iListenSocket = socket(AF_INET, socket_type, 0);
    if(iListenSocket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(iListener != NULL)
        *iListener = iListenSocket;

    setsockopt(iListenSocket, SOL_SOCKET, SO_REUSEADDR, &iReuseAddr, sizeof(iReuseAddr));

    if(bind(iListenSocket, (struct sockaddr *) &stAddr, sizeof(stAddr)) < 0)
    {
        perror("bind");
        close(iListenSocket);
        exit(EXIT_FAILURE);
    }

    if(socket_type == SOCK_STREAM)
    {
        listen(iListenSocket, 5);   /* Queue up to five connections before
                                       having them automatically rejected. */
    }
    return iListenSocket;
}

int
OI_Create_sock(int socket_type, u_short port, int *listener)
{
    struct sockaddr_in address;
    int listening_socket;
    int reuse_addr = 1;

    /* Setup internet address information.  
       This is used with the bind() call */
    memset((char *) &address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    listening_socket = socket(AF_INET, socket_type, 0);
    if(listening_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(listener != NULL)
        *listener = listening_socket;

    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    if(bind(listening_socket, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind");
        close(listening_socket);
        exit(EXIT_FAILURE);
    }

    if(socket_type == SOCK_STREAM)
    {
        listen(listening_socket, 5);    /* Queue up to five connections before
                                           having them automatically rejected. */
    }
    return listening_socket;
}

int
OI_CreateUnixSock(int socket_type, char *sSockPath, int *listener)
{
    struct sockaddr_un name;
    int listening_socket;
    int reuse_addr = 1;

    /* Remove previous socket */
    unlink(sSockPath);

    /* Setup Unix Domain name information.  
       This is used with the bind() call */
    memset((char *) &name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, sSockPath, sizeof(name.sun_path));

    listening_socket = socket(AF_UNIX, socket_type, 0);
    if(listening_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(listener != NULL)
        *listener = listening_socket;

    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));

    if(bind(listening_socket, (struct sockaddr *) &name, sizeof(name)) < 0)
    {
        perror("bind");
        close(listening_socket);
        exit(EXIT_FAILURE);
    }

    if(socket_type == SOCK_STREAM)
    {
        listen(listening_socket, 5);    /* Queue up to five connections before
                                           having them automatically rejected. */
    }
    return listening_socket;
}

int
OI_ConnectUnixSock(int socket_type, char *sSockPath)
{
    struct sockaddr_un name;
    int sock;

    sock = socket(AF_UNIX, socket_type, 0);
    if(sock < 0)
    {
        perror("socket");
        return -1;
    }
    memset((char *) &name, 0, sizeof(name));
    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, sSockPath);

    if(connect(sock, (struct sockaddr *) &name, sizeof(name)) < 0)
    {
        perror("connect");
        close(sock);
        return -1;
    }
    return sock;
}

/* This is a generic function to make a connection to a given server/port.
   service is the port name/number,
   type is either SOCK_STREAM or SOCK_DGRAM, and
   netaddress is the host name to connect to.
   The function returns the socket, ready for action.*/
int
OI_make_connection(char *service, int type, char *netaddress)
{
    /* First convert service from a string, to a number... */
    int port = -1;
    struct in_addr *addr;
    int sock, connected;
    struct sockaddr_in address;

    if(type == SOCK_STREAM)
        port = OI_atoport(service, "tcp");
    if(type == SOCK_DGRAM)
        port = OI_atoport(service, "udp");
    if(port == -1)
    {
        fprintf(stderr, "make_connection:  Invalid socket type.\n");
        return -1;
    }
    addr = OI_atoaddr(netaddress);
    if(addr == NULL)
    {
        fprintf(stderr, "make_connection:	Invalid network address.\n");
        return -1;
    }

    memset((char *) &address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = (port);
    address.sin_addr.s_addr = addr->s_addr;

    sock = socket(AF_INET, type, 0);

    // printf("Connecting to %s on port %d.\n",inet_ntoa(*addr),htons(port));

    if(type == SOCK_STREAM)
    {
        connected = connect(sock, (struct sockaddr *) &address, sizeof(address));
        if(connected < 0)
        {
            perror("connect");
            close(sock);
            return -1;
        }
        return sock;
    }
    /* Otherwise, must be for udp, so bind to address. */
    if(bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind");
        close(sock);
        return -1;
    }
    return sock;
}

int
OI_make_connectionPort(int type, char *netaddress, int port)
{
    /* First convert service from a string, to a number... */
    struct in_addr *addr;
    int sock, connected;
    struct sockaddr_in address;

    addr = OI_atoaddr(netaddress);
    if(addr == NULL)
    {
        fprintf(stderr, "make_connection:	Invalid network address.\n");
        return -1;
    }

    memset((char *) &address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = port;
    address.sin_addr.s_addr = addr->s_addr;

    sock = socket(AF_INET, type, 0);

    if(type == SOCK_STREAM)
    {
        connected = connect(sock, (struct sockaddr *) &address, sizeof(address));
        if(connected < 0)
        {
            perror("connect");
            close(sock);
            return -1;
        }
        return sock;
    }

    /* Otherwise, must be for udp, so bind to address. */
    if(bind(sock, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("bind");
        close(sock);
        return -1;
    }
    return sock;
}


/* This is just like the read() system call, accept that it will make
   sure that all your data goes through the socket. */
int
OI_sock_read(int sockfd, char *buf, size_t count)
{
    size_t bytes_read = 0;
    int this_read;

    while(bytes_read < count)
    {
        do
            this_read = read(sockfd, buf, count - bytes_read);
        while((this_read < 0) && (errno == EINTR));
        if(this_read < 0)
            return this_read;
        else if(this_read == 0)
            return bytes_read;
        bytes_read += this_read;
        buf += this_read;
    }
    return count;
}

/* This is just like the write() system call, accept that it will
   make sure that all data is transmitted. */
int
OI_sock_write(int sockfd, const char *buf, size_t count)
{
    size_t bytes_sent = 0;
    int this_write;

    while(bytes_sent < count)
    {
        do
            this_write = write(sockfd, buf, count - bytes_sent);
        while((this_write < 0) && (errno == EINTR));
        if(this_write <= 0)
            return this_write;
        bytes_sent += this_write;
        buf += this_write;
    }
    return count;
}

/* This function reads from a socket, until it recieves a linefeed
   character.  It fills the buffer "str" up to the maximum size "count".

   This function will return -1 if the socket is closed during the read
   operation.

   Note that if a single line exceeds the length of count, the extra data
   will be read and discarded!  You have been warned. */
int
OI_sock_gets(int sockfd, char *str, size_t count)
{
    int bytes_read;
    size_t total_count = 0;
    char *current_position;
    char last_read = 0;

    current_position = str;
    while(last_read != 10)
    {
        bytes_read = read(sockfd, &last_read, 1);
        if(bytes_read <= 0)
        {
            /* The other side may have closed unexpectedly */
            return -1;  /* Is this effective on other platforms than linux? */
        }
        if((total_count < count) && (last_read != 10) && (last_read != 13))
        {
            current_position[0] = last_read;
            current_position++;
            total_count++;
        }
    }
    if(count > 0)
        current_position[0] = 0;
    return total_count;
}

/* This function writes a character string out to a socket.  It will 
   return -1 if the connection is closed while it is trying to write. */
int
OI_sock_puts(int sockfd, const char *str)
{
    return OI_sock_write(sockfd, str, strlen(str));
}

/* This ignores the SIGPIPE signal.  This is usually a good idea, since
   the default behaviour is to terminate the application.  SIGPIPE is
   sent when you try to write to an unconnected socket.  You should
   check your return codes to make sure you catch this error! */
void
OI_ignore_pipe(void)
{
    struct sigaction sig;

    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGPIPE, &sig, NULL);
}

int
OI_SingleSend(int iSock, char *sBuf, int iLen)
{
    fd_set WriteFds;
    int iNfds;
    struct timeval stZerotv;

    if(iSock < 0)
        return -1;
    FD_ZERO(&WriteFds);
    FD_SET(iSock, &WriteFds);
    stZerotv.tv_sec = stZerotv.tv_usec = 0;
    iNfds = iSock + 1;

    if(select(iNfds, NULL, &WriteFds, NULL, &stZerotv) > 0)
    {
        if(FD_ISSET(iSock, &WriteFds))
            return send(iSock, sBuf, iLen, 0);
    }
    return 0;
}

void
OI_MultiSend(fd_set * WriteFds, int iNfds, char *sBuf, int iLen)
{
    register int iFd;
    struct timeval stZerotv;

    stZerotv.tv_sec = stZerotv.tv_usec = 0;
    iLen = strlen(sBuf);

    if(select(iNfds, NULL, WriteFds, NULL, &stZerotv) > 0)
    {
        for(iFd = 3; iFd < iNfds; iFd++)
            if(FD_ISSET(iFd, WriteFds))
                send(iFd, sBuf, iLen, 0);
    }
}

// Not block read line, return 1 when match '\n' or read iBytes.
int
OI_SockReadln(int iSock, char *sStr, int iBytes)
{
    int iLen;
    char *sLn;

    if((iLen = recv(iSock, sStr, iBytes, MSG_PEEK)) <= 0)
        return -1;
    sStr[iLen - 1] = 0;
    if((sLn = strchr(sStr, '\n')) != NULL)
    {
        iLen = sLn - sStr + 1;
    }
    else if(iLen != iBytes)
        return 0;
    if((iLen = recv(iSock, sStr, iLen, MSG_PEEK)) <= 0)
        return -1;
    sStr[iLen - 1] = 0;
    return 1;
}

int
OI_SetSockRecvBuf(int iSock, long lBufLen)
{
    setsockopt(iSock, SOL_SOCKET, SO_RCVBUF, (void *) &lBufLen, sizeof(lBufLen));
    return 0;
}

int
OI_StrToAddr(char *sAddr, char *sService, char *sProto, struct sockaddr_in *pstIAddr)
{
    struct in_addr *pstAddr;

    if((short) (pstIAddr->sin_port = OI_atoport(sService, sProto)) == -1)
        return -1;
    if(!(pstAddr = OI_atoaddr(sAddr)))
        return -1;
    pstIAddr->sin_family = AF_INET;
    pstIAddr->sin_addr.s_addr = pstAddr->s_addr;
    return 0;
}

int
OI_MakeSockAddrIn(struct sockaddr_in *pstAddr, char *sIP, char *sPort)
{
    int iPort;
    struct in_addr *pInAddr;

    memset(pstAddr, 0, sizeof(struct sockaddr_in));
    if((pInAddr = OI_atoaddr(sIP)) != NULL && (iPort = htons(atoi(sPort))) > 0)
    {
        pstAddr->sin_family = AF_INET;
        pstAddr->sin_addr.s_addr = pInAddr->s_addr;
        pstAddr->sin_port = iPort;
        return 0;
    }

    return -1;
}

//返回值 0 非内部IP， 1 内部IP，< 0  出错
int OI_CheckInnerIp(in_addr_t dwIp) 
{
    static unsigned char cFlag = 0 ;
	static unsigned int dwInnerIP1 = 0;
	static unsigned int dwInnerIP2 = 0;
	static unsigned int dwInnerIP3 = 0;

	if( cFlag == 0 )
	{
	    cFlag = 1 ;
		dwInnerIP1 = ntohl(inet_addr("10.0.0.0"));
		dwInnerIP2 = ntohl(inet_addr("172.16.0.0"));
		dwInnerIP3 = ntohl(inet_addr("192.168.0.0"));
	}

	dwIp = ntohl(dwIp) ; // 转换成主机字节序比较
    // 内部IP
	if(((dwIp & 0xFF000000) == dwInnerIP1) || ((dwIp & 0xFFF00000) == dwInnerIP2) || ((dwIp & 0xFFFF0000) == dwInnerIP3) )
	{   
	    return(1);
	} 

	return(0);
}

int OI_CheckInnerIpS(char *sIP) 
{
	in_addr_t dwIP = inet_addr(sIP);
	return(OI_CheckInnerIp(dwIP));
}
