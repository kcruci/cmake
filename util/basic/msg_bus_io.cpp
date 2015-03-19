#include "msg_bus_io.h"
#include "stdall.h"
#include "cpu_timer.h"

#ifdef __cplusplus
extern "C" {
#endif 

#include "cfg_api.h"
#include "game_base_svr_struct.h"
#include "msg_channel.h"
#include "ids_basic_str2.h"
#include "ids_basic_log.h"
#include "ids_basic_misc.h"
#include "ids_basic_macro.h"

#ifdef __cplusplus
}
#endif 

#define MAX_SHM_NUM 5

static inline void 
spin_lock(volatile int *lock, int id)
{
        static unsigned long long ticks_per_second = 0;
        unsigned long long ticks_nop = 0;

        int l;
        int i = 50;
        UNLIKELY_IF (ticks_per_second == 0) {
            ticks_per_second = GetCpuFreq();
        }
        for (l=atomic_comp_swap(lock, id, 0);
                l!=0 && l!=id; 
                l=atomic_comp_swap(lock, id, 0)
        ) {
                if (i --) { 
                        nop(); 
                        ++ticks_nop;
                } 
                else { 
                        if(ticks_nop > ticks_per_second/50)
                            *lock = 0;
                        i = 50; 
                        sched_yield();
                }
        }
}

static inline int
spin_unlock(volatile int *lock, int id)
{
        return id == atomic_comp_swap(lock, 0, id);
}

static Msg_Channel_Queue * g_pstChannel[MAX_SHM_NUM];
static int mapKey2Ch[MAX_SHM_NUM];

static inline int AddKey(int iKey)
{
    int i;
    for(i=0; i<MAX_SHM_NUM; i++)
	{
        if(0 == mapKey2Ch[i])
		{
            mapKey2Ch[i] = iKey;
            return i;
        }
		else if (mapKey2Ch[i] == iKey)
		{
            return i;
        }
    }
    return -1;
}

static Msg_Channel_Queue ** GetChannelByKey(int iKey)
{
    int i = AddKey(iKey);
    if(0 > i)
    {
		return NULL;
    }
    else
    {
		return &(g_pstChannel[i]);
    }
}

static int SetChannelByKey(int iKey, Msg_Channel_Queue *pstChannel)
{
	int i = AddKey(iKey);
	if(0 > i)
	{
		return -1;
	}

	g_pstChannel[i] = pstChannel;

	return 0;
}

int ReadMsgBus(int iKey, void* pkg, int *dwLen, int iBufSize) 
{
    Msg_Channel_Queue ** ppstChannel;

    RET_TRUE_LINE(NULL == pkg);
    RET_TRUE_LINE(NULL == dwLen);
    
    if(BUS_MSG_LEN > *dwLen) {
        return -11;
    }
    *dwLen = BUS_MSG_LEN;

    ppstChannel = GetChannelByKey(iKey);

    if(NULL == *ppstChannel)
	{
		if(0 >= iBufSize)
			iBufSize = DEFAULT_MSG_BUS_SIZE;
		
        if(0 > GetMsgChannelQueueShm(iKey, ppstChannel, 0666|IPC_CREAT, iBufSize))
		{
            return -12;
        }

		SetChannelByKey(iKey, *ppstChannel);
    }

    return GetPackageFromMsgChannel(*ppstChannel, (char*)pkg, dwLen);
}

int ReadMsgBusByLock(int iKey, void* pkg, int *dwLen, int iBufSize)
{
    int iRet = 0;
    Msg_Channel_Queue ** ppstChannel;

	RET_TRUE_LINE(NULL == pkg);
	RET_TRUE_LINE(NULL == dwLen);

    
    if(BUS_MSG_LEN > *dwLen) {
        return -11;
    }
    *dwLen = BUS_MSG_LEN;

    ppstChannel = GetChannelByKey(iKey);

    if(NULL == *ppstChannel)
	{
		if(0 >= iBufSize)
			iBufSize = DEFAULT_MSG_BUS_SIZE;
		
        if(0 > GetMsgChannelQueueShm(iKey, ppstChannel, 0666|IPC_CREAT, iBufSize))
		{
            return -12;
        }

		SetChannelByKey(iKey, *ppstChannel);
    }

    spin_lock(&(*ppstChannel)->lock, getpid());
    iRet = GetPackageFromMsgChannel(*ppstChannel, (char*)pkg, dwLen);
    spin_unlock(&(*ppstChannel)->lock, getpid());

    return iRet;

}

int WriteMsgBus(int iKey, char* pPkg, uint32_t dwLen, int iBufSize)
{
	Msg_Channel_Queue ** ppstChannel;
	
	RET_TRUE_LINE(NULL == pPkg);
	RET_TRUE_LINE(0 == dwLen);

	ppstChannel = GetChannelByKey(iKey);
    if(NULL == *ppstChannel)
	{
		if(0 >= iBufSize)
			iBufSize = DEFAULT_MSG_BUS_SIZE;
		
        if(0 > GetMsgChannelQueueShm(iKey, ppstChannel, 0666|IPC_CREAT, iBufSize))
		{
            return -12;
        }

		SetChannelByKey(iKey, *ppstChannel);
    }
	
    return AddPackageToMsgChannel(*ppstChannel, pPkg, dwLen);
}

int WriteMsgBusByLock(int iKey, char* pPkg, uint32_t dwLen, int iBufSize)
{
	int iRet = 0;
	Msg_Channel_Queue ** ppstChannel;
	
	RET_TRUE_LINE(NULL == pPkg);
	RET_TRUE_LINE(0 == dwLen);

	ppstChannel = GetChannelByKey(iKey);
    if(NULL == *ppstChannel)
	{
		if(0 >= iBufSize)
			iBufSize = DEFAULT_MSG_BUS_SIZE;
		
        if(0 > GetMsgChannelQueueShm(iKey, ppstChannel, 0666|IPC_CREAT, iBufSize))
		{
            return -12;
        }

		SetChannelByKey(iKey, *ppstChannel);
    }

	spin_lock(&(*ppstChannel)->lock, getpid());
    iRet = AddPackageToMsgChannel(*ppstChannel, pPkg, dwLen);
	spin_unlock(&(*ppstChannel)->lock, getpid());
	
	return iRet;
}

