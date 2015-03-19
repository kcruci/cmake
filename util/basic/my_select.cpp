
#include "my_select.h"

SELECT_FD_SET * GetFdSet(void)
{//这个是默认使用的set
	static SELECT_FD_SET stFdSet;
	return &stFdSet;
}

int FdZero(SELECT_FD_SET *pstFdSet)
{
	if(!pstFdSet) return -1;
	
	FD_ZERO(&pstFdSet->AllFDs);
	
	FD_ZERO(&pstFdSet->ReadFDs);
	FD_ZERO(&pstFdSet->WriteFDs);
	FD_ZERO(&pstFdSet->ExceptFDs);
	
	pstFdSet->iMaxFDNum = 0;
	return 0;
}

int FdDel(SELECT_FD_SET *pstFdSet,int iFdNum)
{
	int i;
	if((!pstFdSet)||(iFdNum<0) ||(iFdNum>=FD_SETSIZE))
		return -1;
	
	FD_CLR(iFdNum,&pstFdSet->AllFDs);
	
	for(i=0;i<FD_SETSIZE;i++)
	{//这样挺费劲的,但是没有办法
		if( FD_ISSET(i,&pstFdSet->AllFDs) )
		{
			pstFdSet->iMaxFDNum = i+1;
		}
	}
	
	return 0;
}

int FdAdd(SELECT_FD_SET *pstFdSet,int iFdNum)
{
	if((!pstFdSet)||(iFdNum<0) ||(iFdNum>=FD_SETSIZE))
		return -1;
	
	FD_SET(iFdNum,&pstFdSet->AllFDs);
	
	if(iFdNum >= pstFdSet->iMaxFDNum)
	{//the last fd
		pstFdSet->iMaxFDNum = iFdNum + 1;
	}	
	
	return 0;
}

int Is_X_Set(SELECT_FD_SET *pstFdSet,int iFdNum,int iType)
{
	fd_set *p=NULL;
	if( (!pstFdSet)||(iFdNum<0) ||(iFdNum>=FD_SETSIZE) )
		return 0;

	switch(iType)
	{
		case READ:
			p=&pstFdSet->ReadFDs;break;
		case WRITE:
			p=&pstFdSet->WriteFDs;break;
		case EXCEPT:
			p=&pstFdSet->ExceptFDs;break;
		default:  //not define type
			return 0;
	}
	
	return FD_ISSET(iFdNum,p);
}

int Fd_X_Select(SELECT_FD_SET *pstFdSet,int iType)
{
	struct timeval stTimeVal;
	int iRet;
	fd_set EmptyFdSet;

	if(!pstFdSet) return -1;//error
	if(!pstFdSet->iMaxFDNum) return 1; //none
	
	memset(&stTimeVal,0,sizeof(struct timeval));
	stTimeVal.tv_sec  = 0;
	stTimeVal.tv_usec = 500; 
	
	FD_ZERO(&EmptyFdSet);
	
	memcpy(&pstFdSet->ReadFDs,(iType&READ)?(&pstFdSet->AllFDs):(&EmptyFdSet),sizeof(fd_set));
	memcpy(&pstFdSet->WriteFDs,(iType&WRITE)?(&pstFdSet->AllFDs):(&EmptyFdSet),sizeof(fd_set));
	memcpy(&pstFdSet->ExceptFDs,(iType&EXCEPT)?(&pstFdSet->AllFDs):(&EmptyFdSet),sizeof(fd_set));
	
	iRet = select(pstFdSet->iMaxFDNum, 
			(iType&READ)?(&pstFdSet->ReadFDs):NULL,
			(iType&WRITE)?(&pstFdSet->WriteFDs):NULL,
			(iType&EXCEPT)?(&pstFdSet->ExceptFDs):NULL,
			&stTimeVal);
	
	if( iRet < 0)
	{ 
		if(errno == EINTR)
		{//A non blocked signal was caught.
			return 1;
		}
		//DEBUG_PRINT("");	
		//SYS_ERR_PRINT("select Error[%d]:",errno);
		return -1;//error
	}
	
	else if(iRet==0)
	{//none
		return 1;
	}
	
	else //iRet>0
		return 0;
}

#ifdef _OPEN_EPOLL_
//-1 error,0----ok
int EpollFdAdd(EPOLL_FD_SET* pstFdSet,int fd,int iType)
{
	int iRet;
	struct epoll_event ev;
	
	if(!pstFdSet)return -1;
	if((fd<0)||(fd>=MAX_EPOLL_FD))return -1;

	if(pstFdSet->iInitFlag != 0x77ABCDEF)
	{
		if(EpollZero(pstFdSet) < 0)
			return -1;
	}
	
	iType &= (READ|WRITE|EXCEPT);
	pstFdSet->RES_TYPE[fd] |= (char)iType;

	if(pstFdSet->RES_TYPE[fd]==0)return -1;
	
	memset(&ev,0,sizeof(struct epoll_event));
	ev.data.fd = fd;
	if(pstFdSet->RES_TYPE[fd]&READ)		ev.events |= EPOLLIN;
	if(pstFdSet->RES_TYPE[fd]&WRITE)	ev.events |= EPOLLOUT;
	if(pstFdSet->RES_TYPE[fd]&EXCEPT)	ev.events |= (EPOLLERR|EPOLLHUP);

	iRet=epoll_ctl(pstFdSet->iEpollFd,EPOLL_CTL_ADD,fd,&ev);

	if(iRet < 0)
	{
		if(errno == EEXIST)
		{//已经在里面了
			iRet=epoll_ctl(pstFdSet->iEpollFd,EPOLL_CTL_MOD,fd,&ev);
			if(iRet<0)
			{
				//SYS_ERR_PRINT("epoll_ctl:EPOLL_CTL_MOD\n");
				return -1;//加入失败
			}
			return 0;
		}
		//SYS_ERR_PRINT("epoll_ctl:EPOLL_CTL_ADD\n");
		return -1;//加入失败
	}
	
	return 0;
}

//-1 error,0---ok
int EpollFdDel(EPOLL_FD_SET*pstFdSet,int fd)
{
	struct epoll_event ev;
	int iRet;
	
	if(!pstFdSet)return -1;
	if((fd<0)||(fd>=MAX_EPOLL_FD))return -1;

	if(pstFdSet->iInitFlag != 0x77ABCDEF)
	{
		return -1;
	}
	
	pstFdSet->RES_TYPE[fd] = 0;//clear
	
	memset(&ev,0,sizeof(struct epoll_event));
	iRet=epoll_ctl(pstFdSet->iEpollFd,EPOLL_CTL_DEL,fd,&ev);
	
	if(iRet < 0)
	{
		if(errno == ENOENT)
		{
			return 0;
		}
		//SYS_ERR_PRINT("epoll_ctl:EPOLL_CTL_DEL\n");
		return -1;//删除失败
	}
	
	return 0;
}

//-1 error,0---ok
int EpollZero(EPOLL_FD_SET* pstFdSet)
{
	if(!pstFdSet)return -1;
	if(pstFdSet->iInitFlag == 0x77ABCDEF)
	{
		if(pstFdSet->iEpollFd>=0)
		{
			close(pstFdSet->iEpollFd);
		}
	}
	
	memset(pstFdSet,0,sizeof(EPOLL_FD_SET));
	
	if( (pstFdSet->iEpollFd=epoll_create(MAX_EPOLL_FD))<0 )
	{
		//SYS_ERR_PRINT("epoll_create:");
		return -1;
	}
	
	pstFdSet->iInitFlag = 0x77ABCDEF;
	return 0;
}

//-1 ---error. 1--- timeout,0 ---have event
int EpollWait(EPOLL_FD_SET* pstFdSet)
{
	if(!pstFdSet)return -1;
	
	if(pstFdSet->iInitFlag != 0x77ABCDEF)
	{
		if(EpollZero(pstFdSet) < 0)
			return -1;
		return 1;//刚刚zero过的，没有任何add
	}
	
	pstFdSet->iGetFdCnt=0;//clear
	
	do{
	pstFdSet->iFds
		= epoll_wait(pstFdSet->iEpollFd, pstFdSet->events, MAX_EPOLL_FD,3000);
	}while(pstFdSet->iFds<0 && errno==EINTR);
	
	if(pstFdSet->iFds<0)
	{
		//SYS_ERR_PRINT("epoll_wait:");
		return -1;
	}
	if(pstFdSet->iFds==0)return 1;//timeout
	
	return 0;
}

//-1 error,>=0 fd
int GetEventEpoll(EPOLL_FD_SET*pstFdSet,int *iType)
{
	int fd;
	if(!pstFdSet)return -1;
	
	if(pstFdSet->iInitFlag != 0x77ABCDEF)
	{
		return -1;
	}
	
	if(pstFdSet->iFds<=0)return -1;
	if(pstFdSet->iGetFdCnt>=pstFdSet->iFds)return -1;
	
	fd=pstFdSet->events[pstFdSet->iGetFdCnt].data.fd;
	//EpollFdAdd(pstFdSet,fd,0);
	
	if(!iType)return -1;
	*iType=0;
	if(pstFdSet->events[pstFdSet->iGetFdCnt].events & EPOLLIN)  *iType |= READ;
	if(pstFdSet->events[pstFdSet->iGetFdCnt].events & EPOLLOUT) *iType |= WRITE;
	if(pstFdSet->events[pstFdSet->iGetFdCnt].events & EPOLLERR) *iType |= EXCEPT;
	if(pstFdSet->events[pstFdSet->iGetFdCnt].events & EPOLLHUP) *iType |= EXCEPT;
	pstFdSet->iGetFdCnt++;

	return fd;
}

EPOLL_FD_SET* GetEpollSet(void)
{
	static EPOLL_FD_SET stEpollFdSet;
	return &stEpollFdSet;
}

int TypeIsRead(int iType)
{
	if(iType&READ)return 1;
	return 0;
}
int TypeIsWrite(int iType)
{
	if(iType&WRITE)return 1;
	return 0;
}
int TypeIsExcept(int iType)
{
	if(iType&EXCEPT)return 1;
	return 0;
}
#endif
//获取两个时间差，单位毫秒
static int GetMS(struct timeval t1, struct timeval t2)
{
    return  abs((t2.tv_sec-t1.tv_sec)*1000+(t2.tv_usec-t1.tv_usec)/1000);
}

int CreateSocket(int iSocketType,char *sIP, unsigned short shPort, int *iListenSocket)
{
	struct sockaddr_in address;
	int listening_socket;
	int reuse_addr = 1;

	memset( (void*)&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	inet_aton( (sIP != NULL)? sIP : "0.0.0.0", &address.sin_addr);//转换成网络字节序
	//address.sin_port = htons( (sPort != NULL) ? atoi(sPort) : 0 );
	address.sin_port = htons(shPort);
	
	if ((listening_socket = socket(AF_INET, iSocketType, 0)) < 0)
	{
		perror("socket");
		return -1;
	}

	//设置本地地址可重用
	setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
	//绑定套接口和地址
	if (bind(listening_socket, (struct sockaddr *) &address, sizeof(address)) < 0) 
	{
		perror("bind");
		close(listening_socket);
		return -1;
	}
	//tcp
	if (iSocketType == SOCK_STREAM) 
	{
		if(listen(listening_socket,128)<0)
		{
			perror("listen");
			close(listening_socket);
			return -1;
		}
	}
	
	if (iListenSocket) *iListenSocket = listening_socket;
	return listening_socket;
}

int SetNBlock(int iSock)
{
	int iFlags;

	//O_NONBLOCK和O_NDELAY所产生的结果都是使
	//I/O变成非搁置模式(non-blocking)，在读取不到数据
	//或是写入缓冲区已满会马上return，而不会搁置程序动作
	//，直到有数据或写入完成。
	iFlags = fcntl(iSock, F_GETFL, 0);//获取目前的套接口设置
	
	iFlags |= O_NONBLOCK;//非阻塞
	iFlags |= O_NDELAY;//
	
	fcntl(iSock, F_SETFL, iFlags);
	
	return 0;
}

int Recv_n(char *pRecvBuf,int iRecvBufLen,int ibytes,int sockfd)
{
	if(pRecvBuf==NULL)
		return -__LINE__;
	if(iRecvBufLen<ibytes)
		return -__LINE__;

	int iRet=0,iLeft=0;
	char *pCur=NULL;
	struct timeval stStartTime,stCurTime;
	gettimeofday(&stStartTime, NULL);
	
	pCur=pRecvBuf;
	iLeft=ibytes;

	while(iLeft>0)
	{
		struct timeval stTimeVal;
		fd_set  ReadFDs;
		int iMaxFDNum;

		//初始化
		FD_ZERO(&ReadFDs);
		iMaxFDNum = 0;
		FD_SET(sockfd, &ReadFDs);
		stTimeVal.tv_sec =10;//10秒
		stTimeVal.tv_usec = 0;//1000 微秒

        //Frank.cheng modify at 2014.12.24
		//iRet = select(sockfd, &ReadFDs, NULL, NULL, &stTimeVal);
        iRet = select(sockfd+1, &ReadFDs, NULL, NULL, &stTimeVal);
		if (iRet < 0)
		{
			return -__LINE__;				
		}
		else if(iRet==0)
		{
			gettimeofday(&stCurTime, NULL);
			if(GetMS(stStartTime, stCurTime)>=100000)// 100秒
				return -__LINE__;
		}

		iRet=recv(sockfd,pCur,iLeft,MSG_WAITALL);
		if(iRet<0)
		{
			return -__LINE__;
		}
		else if(iRet==0)//可能套接口已经挂了
		{
			return -__LINE__;
		}
		else 
		{
			iLeft-=iRet;//还剩下多少字节需要发送出去
			pCur+=iRet;
		}
	}

	if(iLeft>0)
		return  -__LINE__;
	
	return ibytes;
}



