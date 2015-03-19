#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "ids_basic_shm.h"

#define BUF_SIZE 1024
void getNameByPid(pid_t pid, char *task_name) {
    char proc_pid_path[BUF_SIZE];
    char buf[BUF_SIZE];

    sprintf(proc_pid_path, "/proc/%d/status", pid);
    FILE* fp = fopen(proc_pid_path, "r");
    if(NULL != fp){
        if( fgets(buf, BUF_SIZE-1, fp)== NULL ){
            fclose(fp);
        }
        fclose(fp);
        sscanf(buf, "%*s %s", task_name);
    }
}


char *
OI_GetShm(int iKey, size_t iSize, int iFlag)
{
    int iShmID;
    char *sShm;
    char sErrMsg[512];

	//printf("Frank Test OI_GetShm ************************** iFlag = %d\n", iFlag);

#if 0
    struct shmid_ds stShmStat;

    if((iShmID = shmget(iKey, 0, iFlag)) < 0)
    {
        snprintf(sErrMsg, sizeof(sErrMsg), "shmget %d %d", iKey, iSize);
        perror(sErrMsg);
        return NULL;
    }

    if(shmctl(iShmID, IPC_STAT, &stShmStat) < 0)
    {
        perror("shmctl");
        return NULL;
    }

    if(iSize != stShmStat.shm_segsz)
    {
        perror("iSize != shm_segsz");
        return (NULL);
    }
#endif
    if( 0 == iKey )
	{
        snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShm shmget %d %u: iKey don't zero(0)", iKey, iSize);
        perror(sErrMsg);
	    return(NULL);
	}

    if((iShmID = shmget(iKey, iSize, iFlag)) < 0)
    {
    	char task_name[512];
		getNameByPid(getpid(), task_name);
     	if((iFlag & IPC_CREAT))
     	{
	        snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShm iFlag & IPC_CREAT But 0x%x %u %d, pid=%u exe_name=%s", iKey, iSize, iFlag, getpid(),task_name);	
	        perror(sErrMsg);
     	}
		else
			{
		       // snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShm  0x%x %u %d, pid=%u exe_name=%s", iKey, iSize, iFlag, getpid(),task_name);		
		       // perror(sErrMsg);
			}
        return NULL;
    }
    if((sShm = (char *) shmat(iShmID, NULL, 0)) == (char *) -1)
    {
        perror("shmat");
        return NULL;
    }

    return sShm;
}

char *
OI_GetShmOnlyMat(int iKey, size_t *piSize, int iFlag)
{
    int iShmID;
    char *sShm;
    char sErrMsg[50];
    struct shmid_ds stShmStat;

    if( 0 == iKey )
	{
        snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShmOnlyMat shmget %d %lu: iKey don't zero(0)", iKey, *piSize);
        perror(sErrMsg);
	    return(NULL);
	}

    if((iShmID = shmget(iKey, 0, iFlag & (~IPC_CREAT))) < 0)
    {
        snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShmOnlyMat shmget 0x%x %u", iKey, *piSize);
        perror(sErrMsg);
        return NULL;
    }

    if(shmctl(iShmID, IPC_STAT, &stShmStat) < 0)
    {
        perror("OI_GetShmOnlyMat shmctl");
        return NULL;
    }

    if(*piSize != 0 && *piSize != stShmStat.shm_segsz)
    {
        perror("OI_GetShmOnlyMat *piSize != shm_segsz");
        return (NULL);
    }

    if(*piSize == 0)
    {
        *piSize = stShmStat.shm_segsz;
    }

    if((sShm = (char *) shmat(iShmID, NULL, 0)) == (char *) -1)
    {
        perror("OI_GetShmOnlyMat shmat");
        return NULL;
    }

    return sShm;
}

int
OI_GetShm2(void **pstShm, int iKey, size_t iSize, int iFlag)
{
	//printf("Frank Test OI_GetShm2 ************************** iFlag = %d\n", iFlag);

    char *sShm;
	if( 0 == iKey )
	{
	    return(-1);
	}

    if(!(sShm = OI_GetShm(iKey, iSize, iFlag & (~IPC_CREAT))))
    {
        if(!(iFlag & IPC_CREAT))
            return -2;
        if(!(sShm = OI_GetShm(iKey, iSize, iFlag)))
            return -3;

        memset(sShm, 0, iSize);
        *pstShm = sShm;
		return(1);
    }

    *pstShm = sShm;
    return 0;
}

int
OI_GetShm3(void **pstShm, int iKey, size_t iSize, int iFlag)
{
    char *sShm;
	if( 0 == iKey )
	     return(-1);

    if((sShm = OI_GetShm(iKey, iSize, iFlag & (~IPC_CREAT))) == NULL)
    {
        if(!(iFlag & IPC_CREAT))
            return -2;
        if(!(sShm = OI_GetShm(iKey, iSize, iFlag)))
            return -3;

        *pstShm = sShm;
        return 1;
    }
    *pstShm = sShm;
    return 0;
}

static int iRWLockNums;
static int iRWLockID;
static int giChldID;

int
OI_RWLock_Init(key_t key, int iLockNums, int iChldID)
{
    int oflag, i;
    union semun arg;
    unsigned short *ptr = NULL;
    struct semid_ds seminfo;

    iRWLockNums = 0;
    giChldID = iChldID;
    oflag = IPC_CREAT | IPC_EXCL | 0666;

    if((iRWLockID = semget(key, iLockNums, oflag)) > -1)
    {
        /* 4success, we're the first so initialize */
        if((ptr = calloc(iLockNums, sizeof(unsigned short))) == NULL)
            return -1;
        arg.array = ptr;
        for(i = 0; i < iLockNums; i++)
            ptr[i] = 0;
        if(semctl(iRWLockID, 0, SETALL, arg) == -1)
        {   /* 初始化sem */
            free(ptr);
            perror("semctl error\n");
            return (-1);
        }
        free(ptr);
    }
    else if(errno == EEXIST)
    {
        /* 4someone else has created; make sure it's initialized */
        if((iRWLockID = semget(key, iLockNums, 0666)) < 0)
            return -1;
        arg.buf = &seminfo;
        for(i = 0; i < 5; i++)
        {
            if(semctl(iRWLockID, 0, IPC_STAT, arg) == -1)
            {
                perror("semctl error\n");
                return (-1);
            }
            if(arg.buf->sem_otime != 0)
            {
                iRWLockNums = iLockNums;
                return 0;
            }
            sleep(1);
        }
        perror("semget OK, but semaphore not initialized");
        return -1;
    }
    else
    {
        perror("semget error");
        return -1;
    }
    iRWLockNums = iLockNums;
    return 0;
}

int
OI_RWLock_GetValue(key_t key)
{
    int semid, nsems, i;
    struct semid_ds seminfo;
    unsigned short *ptr = NULL;
    union semun arg;

    if((semid = semget(key, 0, 0)) < 0)
    {
        perror("RW_Lock doesn't exist!");
        return -1;
    }
    arg.buf = &seminfo;
    if(semctl(semid, 0, IPC_STAT, arg) < 0)
    {
        perror("semctl error!\n");
        return -1;
    }
    nsems = arg.buf->sem_nsems;

    /* 4allocate memory to hold all the values in the set */
    if((ptr = calloc(nsems, sizeof(unsigned short))) == NULL)
    {
        perror("calloc error");
        return -1;
    }

    arg.array = ptr;

    /* 4fetch the values and print */
    if(semctl(semid, 0, GETALL, arg) < 0)
    {
        free(ptr);
        perror("semctl error!\n");
        return -1;
    }
    for(i = 0; i < nsems; i++)
        printf("semval[%d] = %d\n", i, ptr[i]);
    free(ptr);
    return 0;
}

static int
OI_semcall(int wkey, int op)
{
    int nsems, i;
    struct sembuf sbuf[2];
    struct sembuf *ptr = NULL;

    /* 若wkey为-1 则对信号量集合所有元素赋值 ，否则只对相应的元素赋值 */
    if(wkey > -1)
        nsems = 1;
    else
        nsems = iRWLockNums;

    if(wkey < 0)
    {
        if(op > 0)
            nsems = nsems * 2;
        if((ptr = calloc(nsems, sizeof(struct sembuf))) == NULL)
            return -1;

        if(op > 0)
        {
            for(i = 0; i < nsems / 2; i++)
            {
                ptr[(i * 2)].sem_num = i;
                ptr[(i * 2)].sem_op = 0;
                ptr[(i * 2)].sem_flg = 0;
                ptr[(i * 2) + 1].sem_num = i;
                ptr[(i * 2) + 1].sem_op = op;
                ptr[(i * 2) + 1].sem_flg = SEM_UNDO;
            }
        }
        else
        {
            for(i = 0; i < nsems; i++)
            {
                ptr[i].sem_num = i;
                ptr[i].sem_op = op;
                ptr[i].sem_flg = SEM_UNDO;
            }
        }
        if(semop(iRWLockID, ptr, nsems) == -1)
        {
            perror("semop error1");
            free(ptr);
            return -1;
        }
        free(ptr);
    }
    else
    {
        if(op > 0)
        {
            nsems = 2;
            sbuf[0].sem_num = wkey;
            sbuf[0].sem_op = 0;
            sbuf[0].sem_flg = 0;
            sbuf[1].sem_num = wkey;
            sbuf[1].sem_op = op;
            sbuf[1].sem_flg = SEM_UNDO; /* 当进程退出时恢复所有操作 */
        }
        else
        {
            sbuf[0].sem_num = wkey;
            sbuf[0].sem_op = op;
            sbuf[0].sem_flg = SEM_UNDO; /* 当进程退出时恢复所有操作 */
        }
        if(semop(iRWLockID, sbuf, nsems) == -1)
        {
            perror("semop error2");
            return -1;
        }
    }
    return 0;
}
static int iWhichSem;
int
OI_RWLock_UnLock()
{
    return OI_semcall(iWhichSem, -1);
}

int
OI_RWLock_RDLock()
{
    if(OI_semcall(giChldID, 1) == 0)
    {
        iWhichSem = giChldID;
        return 0;
    }
    return -1;
}

int
OI_RWLock_WDLock()
{
    if(OI_semcall(-1, 1) == 0)
    {
        iWhichSem = -1;
        return 0;
    }
    return -1;
}

/////////////////////////////////////////////////////////////////////////////////////
//信号量操作相关函数
//static int  iSemID=-1;    
//#define SEMP_KEY 0x2999

int
OI_Lib_Semp_Init(OI_SEMLOCKINFO * pstLockInfo, int iSemKey)
{
#ifndef SLACKWARE
    union semun
    {
        int val;
        struct semid_ds *buf;
        ushort *array;
    };
#endif
    union semun arg;
    u_short array[2] = { 0, 0 };
    int iFirst = 1;             //是否第一次创建

    //初始化信号灯
    iFirst = 1;
    if((pstLockInfo->iSemID = semget(iSemKey, 1, 0666 | IPC_CREAT | IPC_EXCL)) < 0)
    {
        if((pstLockInfo->iSemID = semget(iSemKey, 1, 0)) == -1)
        {
            snprintf(pstLockInfo->sErrMsg, sizeof(pstLockInfo->sErrMsg), "Fail to semget.");
            return -1;
        }
        iFirst = 0;
    }


    if(iFirst)
    {
        arg.array = &array[0];
        if(semctl(pstLockInfo->iSemID, 0, SETALL, arg) == -1)
        {
            snprintf(pstLockInfo->sErrMsg, sizeof(pstLockInfo->sErrMsg), "Can't initialize the semaphore value.");
            return -1;
        }
    }

    return 0;
}

int
OI_Lib_Semp_Lock(OI_SEMLOCKINFO * pstLockInfo)
{
    struct sembuf sops[2] = { {0, 0, 0}, {0, 1, SEM_UNDO} };
    size_t nsops = 2;

    assert(pstLockInfo->iSemID >= 0);

    if(semop(pstLockInfo->iSemID, &sops[0], nsops) == -1)
        return -1;

    return 0;
}

int
OI_Lib_Semp_Unlock(OI_SEMLOCKINFO * pstLockInfo)
{
    struct sembuf sops[1] = { {0, -1, SEM_UNDO} };
    size_t nsops = 1;

    assert(pstLockInfo->iSemID >= 0);
    if(semop(pstLockInfo->iSemID, &sops[0], nsops) == -1)
        return -1;

    return 0;
}


char *
OI_GetShm4(int iKey, size_t iSize, int iFlag, int *piShmID)
{
    int iShmID;
    char *sShm;
    char sErrMsg[50];

    if( 0 == iKey )
	{
        snprintf(sErrMsg, sizeof(sErrMsg), "OI_GetShm4 shmget %d %lu: iKey don't zero(0)", iKey, iSize);
        perror(sErrMsg);
	    return(NULL);
	}

    if((iShmID = shmget(iKey, iSize, iFlag)) < 0)
    {
        sprintf(sErrMsg, "OI_GetShm4 shmget %d %lu", iKey, iSize);
        perror(sErrMsg);
        return NULL;
    }
    if((sShm = shmat(iShmID, NULL, 0)) == (char *) -1)
    {
        perror("OI_GetShm4 shmat");
        return NULL;
    }
    *piShmID = iShmID;
    return sShm;
}
