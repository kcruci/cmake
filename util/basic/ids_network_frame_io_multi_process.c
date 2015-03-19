#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/epoll.h>
#include "ids_network_frame_io_multi_process.h"
#include "svrlog.h"

#define LOG(fmt, args...)// do { if (pstIOMPMng->pstLog) { Log(pstIOMPMng->pstLog, 2, "%s:%d(%s): " fmt, __FILE__, __LINE__, __FUNCTION__ , ## args); } } while (0)
#define LOG_BUG_RET(retval)// do { LOG("BUG!"); return (retval); } while (0)
#define ERRSTR strerror(errno)

struct _ids_io_multi_process_mng
{
    LogFile *pstLog;
    int iLogLevel;

    int iMaxSocketNum;
    int iTriggered;

	int epollfd;
    struct epoll_event *evs;
};

ids_io_multi_process_mng *ids_iomp_mng_init(LogFile * pstLog, int iLogLevel, int iMaxSocketNum)
{
	LOG("********* %s **********", __FUNCTION__);

    ids_io_multi_process_mng *pstIOMPMng;

    if(!(pstIOMPMng = (ids_io_multi_process_mng *)malloc(sizeof(ids_io_multi_process_mng))))
    {
        return NULL;
    }

    memset(pstIOMPMng, 0, sizeof(ids_io_multi_process_mng));

    pstIOMPMng->pstLog = pstLog;
    pstIOMPMng->iLogLevel = iLogLevel;

    if((pstIOMPMng->iMaxSocketNum = iMaxSocketNum) <= 0)
    {
        LOG_BUG_RET(NULL);
    }

    if((pstIOMPMng->epollfd = epoll_create(iMaxSocketNum)) < 0)
    {
        LOG("epoll_create Error (%s)!", ERRSTR);
        return NULL;
    }
    if(!(pstIOMPMng->evs = malloc(sizeof(struct epoll_event) * iMaxSocketNum)))
    {
        LOG("malloc epoll_event array failed (%s)!", ERRSTR);
        return NULL;
    }


    return pstIOMPMng;
}

static inline void ids_iomp_mng_set_epoll_ev(struct epoll_event *pev, int iFd, int iFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    uint32_t epoll_events = 0;

    if((iFlag & IDS_IOMP_FLAG_READ) != 0)
    {
        epoll_events |= EPOLLIN;
    }
    if((iFlag & IDS_IOMP_FLAG_WRITE) != 0)
    {
        epoll_events |= EPOLLOUT;
    }
    if((iFlag & IDS_IOMP_FLAG_ERROR) != 0)
    {
        epoll_events |= EPOLLHUP | EPOLLERR;
    }

    pev->data.fd = iFd;
    pev->events = epoll_events;
}

static inline void ids_iomp_mng_get_epoll_ev(struct epoll_event *pev, int *piFd, int *piFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    int iFlag = 0;

    if((pev->events & EPOLLIN) != 0)
    {
        iFlag |= IDS_IOMP_FLAG_READ;
    }
    if((pev->events & EPOLLOUT) != 0)
    {
        iFlag |= IDS_IOMP_FLAG_WRITE;
    }
    if((pev->events & (EPOLLHUP | EPOLLERR)) != 0)
    {
        iFlag |= IDS_IOMP_FLAG_ERROR;
    }

    *piFd = pev->data.fd;
    *piFlag = iFlag;
}

int ids_iomp_mng_add_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd, int iFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    struct epoll_event ev;

    if(iFd < 0)
    {
        LOG_BUG_RET(-1);
    }
    ids_iomp_mng_set_epoll_ev(&ev, iFd, iFlag);
    if(epoll_ctl(pstIOMPMng->epollfd, EPOLL_CTL_ADD, iFd, &ev) < 0)
    {
        if(errno != EEXIST || epoll_ctl(pstIOMPMng->epollfd, EPOLL_CTL_MOD, iFd, &ev) < 0)
        {
            LOG("epoll_ctl Error (%s)!", ERRSTR);
            return -1;
        }
        else
        {
            LOG("epoll_ctl add Error!");
        }
    }
    return 0;
}

int ids_iomp_mng_mod_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd, int iFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    struct epoll_event ev;

    if(iFd < 0)
    {
        LOG_BUG_RET(-1);
    }
    ids_iomp_mng_set_epoll_ev(&ev, iFd, iFlag);
    if(epoll_ctl(pstIOMPMng->epollfd, EPOLL_CTL_MOD, iFd, &ev) < 0)
    {
        if(errno != ENOENT || epoll_ctl(pstIOMPMng->epollfd, EPOLL_CTL_ADD, iFd, &ev) < 0)
        {
            LOG("epoll_ctl Error (%s)!", ERRSTR);
            return -1;
        }
        else
        {
            LOG("epoll_ctl mod Error!");
        }
    }
    return 0;
}

int ids_iomp_mng_del_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd)
{
	LOG("********* %s **********", __FUNCTION__);

    struct epoll_event ev;

    if(iFd < 0)
    {
        LOG_BUG_RET(-1);
    }
    if(epoll_ctl(pstIOMPMng->epollfd, EPOLL_CTL_DEL, iFd, &ev) < 0)
    {
        if(errno != ENOENT)
        {
            LOG("epoll_ctl Error (%s)!", ERRSTR);
            return -1;
        }
        else
        {
            LOG("epoll_ctl del Error!");
        }
    }
    return 0;
}

int ids_iomp_mng_wait_for_fd(ids_io_multi_process_mng * pstIOMPMng, int iTimeoutMSec)
{
	LOG("********* %s **********", __FUNCTION__);

    int iTriggered;

    if(iTimeoutMSec < 0)
    {
        iTimeoutMSec = -1;
    }
    iTriggered = epoll_wait(pstIOMPMng->epollfd, pstIOMPMng->evs, pstIOMPMng->iMaxSocketNum, iTimeoutMSec);
    if(iTriggered < 0)
    {
        if(errno != EAGAIN && errno != EINTR)
        {
            LOG("epoll_wait Error!");
        }
        return -1;
    }
    pstIOMPMng->iTriggered = iTriggered;
    
    return iTriggered;
}

int ids_iomp_mng_get_ready_fd(ids_io_multi_process_mng * pstIOMPMng, int *piPos, int *piFlag)
{
	LOG("********* %s **********", __FUNCTION__);

    int iFd;

    if(*piPos < 0)
    {
        LOG_BUG_RET(-1);
    }
    if(*piPos >= pstIOMPMng->iTriggered)
    {
        return -100;
    }
    ids_iomp_mng_get_epoll_ev(pstIOMPMng->evs + *piPos, &iFd, piFlag);
    if(!*piFlag)
    {
        LOG_BUG_RET(-1);
    }
    (*piPos)++;
    return (iFd);
}
