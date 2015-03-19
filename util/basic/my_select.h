#ifndef _MY_SELECT_H
#define _MY_SELECT_H

#include <sys/socket.h>
#include <sys/time.h>  
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/select.h>
#include <sys/stat.h> 
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> 
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h> 
#include <unistd.h> 
#include <errno.h>  
#include <assert.h>
#include <stdint.h>


#ifdef _OPEN_EPOLL_
#include <sys/epoll.h>
#endif


#ifdef _OPEN_EPOLL_
#define MAX_EPOLL_FD 12000 
typedef struct
{
	int iInitFlag;
	int iEpollFd;//epoll_create(MAX_EPOLL_FD) 
	char RES_TYPE[MAX_EPOLL_FD];
	struct epoll_event events[MAX_EPOLL_FD];
	int iFds;//epoll_wait(...)
	int iGetFdCnt;
}EPOLL_FD_SET;

EPOLL_FD_SET* GetEpollSet(void);
int EpollFdAdd(EPOLL_FD_SET* pstFdSet,int fd,int iType);
int EpollFdDel(EPOLL_FD_SET*pstFdSet,int fd);
int EpollZero(EPOLL_FD_SET* pstFdSet);
int EpollWait(EPOLL_FD_SET*pstFdSet);
int GetEventEpoll(EPOLL_FD_SET*pstFdSet,int *iType);
int TypeIsRead(int iType);
int TypeIsWrite(int iType);
int TypeIsExcept(int iType);

#define EPOLL_ZERO()        	EpollZero(GetEpollSet())
#define EPOLL_ADD(fd,iType) 	EpollFdAdd(GetEpollSet(),fd,iType)
#define EPOLL_DEL(fd)       	EpollFdDel(GetEpollSet(),fd)
#define EPOLL_WAIT()        	EpollWait(GetEpollSet())
#define EPOLL_GET(iType)    	GetEventEpoll(GetEpollSet(),iType)
#define EPOLL_IS_READ(iType) 	TypeIsRead(iType)
#define EPOLL_IS_WRITE(iType) 	TypeIsWrite(iType)
#define EPOLL_IS_EXCEPT(iType) 	TypeIsExcept(iType)
#endif

typedef struct 
{
	fd_set ReadFDs, WriteFDs,ExceptFDs,AllFDs;
	int iMaxFDNum;
}SELECT_FD_SET;

#define READ   ((int)0x01)
#define WRITE  ((int)0x02)
#define EXCEPT ((int)0x04)


SELECT_FD_SET * GetFdSet(void);
int FdZero(SELECT_FD_SET *pstFdSet);
int FdDel(SELECT_FD_SET *pstFdSet,int iFdNum);
int FdAdd(SELECT_FD_SET *pstFdSet,int iFdNum);

//X 是read write except
int Is_X_Set(SELECT_FD_SET *pstFdSet,int iFdNum,int iType);
int Fd_X_Select(SELECT_FD_SET *pstFdSet,int iType);

#define Is_Read_Set(set,fd) Is_X_Set(set,fd,READ)
#define Is_Write_Set(set,fd) Is_X_Set(set,fd,WRITE)
#define Is_Except_Set(set,fd) Is_X_Set(set,fd,EXCEPT)

#define Fd_Read_Select(set) Fd_X_Select(set,READ)
#define Fd_Write_Select(set) Fd_X_Select(set,WRITE)
#define Fd_Except_Select(set) Fd_X_Select(set,EXCEPT)
#define Fd_RW_Select(set) Fd_X_Select(set,READ|WRITE)

////////////////////使用默认的set////////////////////
#define FDZERO() FdZero(GetFdSet())     // 1st
#define FDDEL(fd) FdDel(GetFdSet(),fd)
#define FDADD(fd) FdAdd(GetFdSet(),fd)  // 2nd

#define READ_IS_SET(fd) Is_X_Set(GetFdSet(),fd,READ)
#define WRITE_IS_SET(fd) Is_X_Set(GetFdSet(),fd,WRITE)
#define EXCEPT_IS_SET(fd) Is_X_Set(GetFdSet(),fd,EXCEPT)

#define READ_SELECT() Fd_X_Select(GetFdSet(),READ)
#define WRITE_SELECT() Fd_X_Select(GetFdSet(),WRITE)
#define EXCEPT_SELECT() Fd_X_Select(GetFdSet(),EXCEPT)
#define RW_SELECT()  Fd_X_Select(GetFdSet(),READ|WRITE) // 3rd
/*read and write*/
////////////////////使用默认的set////////////////////

int CreateSocket(int iSocketType,char *sIP, unsigned short shPort, int *iListenSocket);
int SetNBlock(int iSock);


//从套接口接受iSendBufLen个字符
int Recv_n(char *pRecvBuf,int iRecvBufLen,int ibytes,int sockfd);
#endif

