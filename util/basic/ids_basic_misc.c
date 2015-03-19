#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>
#include <sys/param.h>
#include <sys/stat.h>


#include "ids_basic_misc.h"
#include "ids_basic_str.h"

#define PROC_DIR	"/proc"

int
OI_WritePid(char *sPidFilePath)
{
    FILE *pPidFile;

    if((pPidFile = fopen(sPidFilePath, "w")) == NULL)
        return -1;
    fprintf(pPidFile, "%d", getpid());
    fclose(pPidFile);
    return 0;
}

extern int g_iStopFlag;
void OI_sig_handle(int signo);

void
OI_DaemonInit()
{
    int fd;
	int iRet ;

    // shield some signals
    signal(SIGALRM, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    //signal(SIGTERM, SIG_IGN);
    signal(SIGTERM, OI_sig_handle);

    // fork child process
    if(fork())
        exit(0);

    // creates  a new session
    if(setsid() == -1)
        exit(1);

    // If you needn't STDIN,STDOUT,STDERR, close fd from 0;
    for(fd = 3; fd < NOFILE; fd++)
        close(fd);
    iRet = chdir("/");
    umask(0);

    g_iStopFlag = 0;
    return;
}

char *
OI_MarkTime(int iFlag, char *sFormat, ...)
{
    static struct timeval stStartTv;
    char sTmp[30];
    static char sStr[sizeof(sTmp) + 30];
    struct timeval stCurrTv, *pstAddTv;
    va_list ap;
    int sec, usec;

    if(iFlag)
    {
        gettimeofday(&stCurrTv, NULL);
        sec = stCurrTv.tv_sec - stStartTv.tv_sec;
        if((usec = stCurrTv.tv_usec - stStartTv.tv_usec) < 0)
        {
            sec--;
            usec += 1000000;
        }
        va_start(ap, sFormat);
        if(iFlag == 3 || iFlag == 4)
        {
            pstAddTv = va_arg(ap, struct timeval *);

            pstAddTv->tv_sec += sec;
            if((pstAddTv->tv_usec += usec) > 1000000)
            {
                pstAddTv->tv_sec++;
                pstAddTv->tv_usec -= 1000000;
            }
        }
        else if(iFlag == 5)
        {
            pstAddTv = va_arg(ap, struct timeval *);

            pstAddTv->tv_sec = sec;
            pstAddTv->tv_usec = usec;
            va_end(ap);
            return NULL;
        }
        vsnprintf(sTmp, sizeof(sTmp), sFormat, ap);
        va_end(ap);
        sprintf(sStr, "%s: %dsec %dms %dus", sTmp, sec, usec / 1000, usec % 1000);
        if(iFlag == 1 || iFlag == 3)
            printf("%s\n", sStr);
    }
    else
    {
        gettimeofday(&stStartTv, NULL);
    }
    return sStr;
}


int
OI_GetPrpsinfo(pid_t pr_pid, prpsinfo_t * prpsinfo)
{
    int fd;
    char sFilePath[100];

#ifdef __linux__
    char line[255], item[100];
    FILE *status_file;
#endif

    sprintf(sFilePath, "%s/%d", PROC_DIR, pr_pid);
    if((fd = open(sFilePath, O_RDONLY)) < 0)
    {
        perror(sFilePath);
        return -1;
    }
#ifdef __linux__
    strcat(sFilePath, "/status");
    if((status_file = fopen(sFilePath, "r")) == NULL)
    {
        // printf("Can not open stauts file:%s\nIgnore........\n",proc_status_file);;
        close(fd);
        return -1;
    }
    strcpy(prpsinfo->pr_fname, "");
    while(!feof(status_file))
    {
        memset(line, 0, sizeof(line));
        if(fgets(line, sizeof(line), status_file) == NULL)
            break;
        OI_get_val(item, line);
        if(!strcmp(item, "Name:"))
        {
            OI_get_val(item, line);
            strcpy(prpsinfo->pr_fname, item);
            break;
        }
    }


    fclose(status_file);
    close(fd);
    if(!strcmp(prpsinfo->pr_fname, ""))
        return -1;
    prpsinfo->pr_pid = pr_pid;
#else
    if(ioctl(fd, PIOCPSINFO, prpsinfo) < 0)
    {
        close(fd);
        return -1;
    }
#endif
    close(fd);
    return 0;
}

int
OI_KillPrevious(char *sPidFilePath)
{
    FILE *pPidFile;
    char sPid[10], sMyName[50], sKillName[50];
    int iPid;
    prpsinfo_t prpsinfo;

    if((pPidFile = fopen(sPidFilePath, "r")) == NULL)
    {
        perror(sPidFilePath);
        return -1;
    }
    memset(sPid, 0, sizeof(sPid));
    if(!fgets(sPid, sizeof(sPid) - 1, pPidFile))
        return -1;
    iPid = atoi(sPid);

    if(OI_GetPrpsinfo(getpid(), &prpsinfo) < 0)
        return -1;
    memset(sMyName, 0, sizeof(sMyName));
    strncpy(sMyName, prpsinfo.pr_fname, sizeof(sMyName) - 1);

    if(OI_GetPrpsinfo(iPid, &prpsinfo) < 0)
        return -1;
    memset(sKillName, 0, sizeof(sKillName));
    strncpy(sKillName, prpsinfo.pr_fname, sizeof(sKillName) - 1);

    if(strcmp(sMyName, sKillName))
        return -1;
    if(kill(iPid, SIGKILL))
        return -1;
    return 0;
}

#if 1

int
OI_UinToTable(unsigned int lUin)
{
    return lUin % 10;
}

char *
OI_HashToIndexTable(char *sBaseTableName, unsigned char c)
{
    static char sTableName[80];

    snprintf(sTableName, sizeof(sTableName) - 1, "%s%d", sBaseTableName, c);
    return sTableName;
}

#endif
int
OI_MsSleep(unsigned int lMs)
{
    struct timeval stTv;

    stTv.tv_sec = lMs / 1000;
    stTv.tv_usec = (lMs % 1000) * 1000;
    select(0, NULL, NULL, NULL, &stTv);
    return 0;
}

int
OI_FileCopy(char *sFile1, char *sFile2)
{
    FILE *pf1, *pf2;
    char sBuf[1024 * 4];
    int iBytesRead;

    if((pf1 = fopen(sFile1, "rb")) != NULL)
    {
        if((pf2 = fopen(sFile2, "wb")) != NULL)
        {
            while((iBytesRead = fread(sBuf, 1, sizeof(sBuf), pf1)) > 0)
                fwrite(sBuf, 1, iBytesRead, pf2);
            fclose(pf1);
            fclose(pf2);
            return 0;
        }
        fclose(pf1);
    }
    return -1;
}


unsigned int
OI_GetPrimeNumber(unsigned int iNum)
{
    unsigned int i, j;

    for(i = iNum; i <= (unsigned int) 0xffffffff; ++i)
    {
        for(j = 2; j < iNum; ++j)
            if(i % j == 0)
                break;
        if(j == iNum)
            return i;
    }
    return 0;
}
