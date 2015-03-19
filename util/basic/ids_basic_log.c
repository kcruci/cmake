#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stropts.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>
#include "ids_basic_log.h"
#include "ids_basic_shm.h"
#include "ids_basic_str.h"
#include "oi_net.h"
#include "ids_basic_str2.h"
#include "strtime.h"

static OI_LogShmFile stSFLog;

static int
ShiftFiles(OI_LogFile * pstLogFile)
{
    struct stat stStat;
    char sLogFileName[300];
    char sNewLogFileName[300];
    int i;
    struct tm stLogTm, stShiftTm;

    if (pstLogFile->iShiftType == 6)
        return 0;

    snprintf(sLogFileName,sizeof(sLogFileName),"%s.log", pstLogFile->sBaseFileName);

    if(stat(sLogFileName, &stStat) < 0) 
    {
        if (errno == ENOENT) {
            FILE *pfile;
            if ((pfile = fopen(sLogFileName, "a+")) == NULL) 
                return -1;
            fclose(pfile);
            if (stat(sLogFileName, &stStat) < 0)
                return -1;
        } else {
            return -1;
        }
    }
    switch (pstLogFile->iShiftType)
    {
        case 0:
            if(stStat.st_size < pstLogFile->lMaxSize)
                return 0;
            break;
        case 2:
            if(stStat.st_mtime - pstLogFile->lLastShiftTime < pstLogFile->lMaxCount)
                return 0;
            break;
        case 3:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 86400)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_mday == stShiftTm.tm_mday)
                return 0;
            break;
        case 4:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 3600)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_hour == stShiftTm.tm_hour)
                return 0;
            break;
        case 5:
            if(pstLogFile->lLastShiftTime - stStat.st_mtime > 60)
                break;
            memcpy(&stLogTm, localtime(&stStat.st_mtime), sizeof(stLogTm));
            memcpy(&stShiftTm, localtime(&pstLogFile->lLastShiftTime), sizeof(stShiftTm));
            if(stLogTm.tm_min == stShiftTm.tm_min)
                return 0;
            break;
        default:
            if(pstLogFile->lLogCount < pstLogFile->lMaxCount)
                return 0;
            pstLogFile->lLogCount = 0;
    }

    for(i = pstLogFile->iMaxLogNum - 2; i >= 0; i--)
    {
        if(i == 0)
            snprintf(sLogFileName,sizeof(sLogFileName),"%s.log", pstLogFile->sBaseFileName);
        else
            snprintf(sLogFileName,sizeof(sLogFileName),"%s%d.log", pstLogFile->sBaseFileName, i);

        if(access(sLogFileName, F_OK) == 0)
        {
            snprintf(sNewLogFileName,sizeof(sNewLogFileName),"%s%d.log", pstLogFile->sBaseFileName, i + 1);
            if(rename(sLogFileName, sNewLogFileName) < 0)
            {
                return -1;
            }
        }
    }
    time(&pstLogFile->lLastShiftTime);
    return 0;
}

int
OI_InitLogFile(OI_LogFile * pstLogFile, char *sLogBaseName, int iShiftType, int iMaxLogNum, int iMAX)
{
    memset(pstLogFile, 0, sizeof(OI_LogFile));
    strncat(pstLogFile->sLogFileName, sLogBaseName, sizeof(pstLogFile->sLogFileName) - 10);
    strcat(pstLogFile->sLogFileName, ".log");

    strncpy(pstLogFile->sBaseFileName, sLogBaseName, sizeof(pstLogFile->sBaseFileName) - 15);
    pstLogFile->iShiftType = iShiftType;
    pstLogFile->iMaxLogNum = iMaxLogNum;
    pstLogFile->lMaxSize = iMAX;
    pstLogFile->lMaxCount = iMAX;
    pstLogFile->lLogCount = iMAX;
    time(&pstLogFile->lLastShiftTime);

    return ShiftFiles(pstLogFile);
}
static void MyFuncForBreakPoint()
{
	int i = 0;
	
}
int
OI_Log(OI_LogFile * pstLogFile, int iLogTime, const char *sFormat, ...)
{
    va_list ap;
    struct timeval stLogTv;

    if (pstLogFile->iShiftType == 6) {
        snprintf(pstLogFile->sLogFileName, sizeof(pstLogFile->sLogFileName), "%s_%s.log", 
            pstLogFile->sBaseFileName, GetCurShortDateStr());
    }

    if((pstLogFile->pLogFile = fopen(pstLogFile->sLogFileName, "a+")) == NULL)
        return -1;
	fseek(pstLogFile->pLogFile,0L,SEEK_SET);
	
   
    if(iLogTime == 1)
    {
        fprintf(pstLogFile->pLogFile, "[%s] ", OI_GetCurDateTimeStr());
    }
    else if(iLogTime == 2)
    {
        gettimeofday(&stLogTv, NULL);
        fprintf(pstLogFile->pLogFile, "[%s.%.6d] ", 
            OI_GetDateTimeStr((const time_t *) &(stLogTv.tv_sec)), (int) stLogTv.tv_usec);
    }
	 va_start(ap, sFormat);
	//Begin modify by Frank.cheng
   
	char szBuf[40960] = {'\0'};
	memset(szBuf, 0x00, sizeof(szBuf));
	sprintf(szBuf, "pid=%d ",getpid());

	if(szBuf[0] == '\0')
	{
		MyFuncForBreakPoint();
	}
	fprintf(pstLogFile->pLogFile, szBuf);

	 vfprintf(pstLogFile->pLogFile, sFormat, ap);
	//end modify by Frank.cheng
	
    fprintf(pstLogFile->pLogFile, "\n");

    va_end(ap);
    pstLogFile->lLogCount++;


    fclose(pstLogFile->pLogFile);

   return ShiftFiles(pstLogFile);
}

int
OI_LogBin(OI_LogFile * pstLogFile, int iLogTime, const char *sBuf, int iBufLen)
{
    int iLen;
    int iType;

    if (iBufLen > LOG_BUFFER_MAX_LEN)
        return -1;

    if (pstLogFile->iShiftType == 6) {
        snprintf(pstLogFile->sLogFileName, sizeof(pstLogFile->sLogFileName), "%s_%s.log", 
            pstLogFile->sBaseFileName, GetCurShortDateStr());
    }

    if((pstLogFile->pLogFile = fopen(pstLogFile->sLogFileName, "a+")) == NULL)
        return -2;

    if(iLogTime == 1)
    {
        time_t tLogTime;
        time(&tLogTime);

        iType = htonl((int)LOGBIN_TIME_SIMPLE);
        iLen = htonl(sizeof(tLogTime));
        fwrite(&iType, 1, sizeof(iType), pstLogFile->pLogFile);
        fwrite(&iLen, 1, sizeof(iLen), pstLogFile->pLogFile);
        fwrite(&tLogTime, 1, sizeof(tLogTime), pstLogFile->pLogFile);
    } else if (iLogTime == 2) {
        struct timeval stLogTime;
        gettimeofday(&stLogTime, NULL);

        iType = htonl((int)LOGBIN_TIME);
        iLen = htonl(sizeof(stLogTime));
        fwrite(&iType, 1, sizeof(iType), pstLogFile->pLogFile);
        fwrite(&iLen, 1, sizeof(iLen), pstLogFile->pLogFile);
        fwrite(&stLogTime, 1, sizeof(stLogTime), pstLogFile->pLogFile);
    } else {
        // no time logged
    }

    iType = htonl((int)LOGBIN_BUFFER);
    iLen = htonl(iBufLen);
    fwrite(&iType, 1, sizeof(iType), pstLogFile->pLogFile);
    fwrite(&iLen, 1, sizeof(iLen), pstLogFile->pLogFile);
    fwrite(sBuf, 1, iBufLen, pstLogFile->pLogFile);

    pstLogFile->lLogCount++;
    fclose(pstLogFile->pLogFile);
    return ShiftFiles(pstLogFile);
}

typedef int (* LogParseCallback)(int iType, int iLen, const char *sBuf, void *pCtx);

static int OI_LogParseDumper(int iType, int iLen, const char *sBuf, void *pCtx) 
{
    FILE *pOutput = (FILE *)pCtx;
    if (pCtx == NULL)
        pOutput = stdout;
    switch(iType)
    {
        case LOGBIN_TIME_SIMPLE:
            if (iLen != 4)
                return -1;
            fprintf(pOutput, "[%s]\n", OI_GetDateTimeStr(((const time_t *)sBuf)));
            break;
        case LOGBIN_TIME: {
            struct timeval *pTime = (struct timeval *)sBuf;
            if (iLen != 8)
                return -2;
            fprintf(pOutput, "[%s.%.6d]\n", OI_GetDateTimeStr((const time_t *) &(pTime->tv_sec)), (int) pTime->tv_usec);
            break;
        }
        case LOGBIN_BUFFER:
            hex_dump_tofile(pOutput, sBuf, iLen);
            break;
        default:
            return -3;
    }

    return 0;
}

static int OI_LogParseIterator(int iType, int iLen, const char *sBuf, void *pCtx) 
{
    static struct timeval logtime = { 0 };
    LogIteratorCallback pUserCallback = (LogIteratorCallback)pCtx;
    int iRet = 0;

    if (pCtx == NULL)
        return -1;

    switch(iType)
    {
        case LOGBIN_TIME_SIMPLE:
            if (iLen != sizeof(time_t))
                return -2;
            logtime.tv_sec = *((time_t *)sBuf);
            break;
        case LOGBIN_TIME: 
            if (iLen != sizeof(struct timeval))
                return -3;
            logtime = *((struct timeval *)sBuf);
            break;
        case LOGBIN_BUFFER:
            if ( (iRet = (pUserCallback)(&logtime, iLen, (char *)sBuf)) < 0) {
                return iRet * 10;
            }
            memset(&logtime, 0, sizeof(struct timeval));
            break;
        default:
            return -4;
    }

    return 0;
}

struct LogParseSenderContext {
    int fd;
    struct sockaddr_in dest;    
};

static int OI_LogParseSender(int iType, int iLen, const char *sBuf, void *pCtx) 
{
    struct LogParseSenderContext *pSockCtx = (struct LogParseSenderContext *)pCtx;
    if (pCtx == NULL)
        return -1;

    if (pSockCtx->fd <= 0)
        return -2;

    switch(iType)
    {
        case LOGBIN_TIME_SIMPLE:
        case LOGBIN_TIME: 
            break;
        case LOGBIN_BUFFER: {
            int retry_count = 0;
retry_sendto:
            if ( sendto(pSockCtx->fd, sBuf, iLen, 0, 
                (struct sockaddr*)&pSockCtx->dest, sizeof(struct sockaddr_in)) < 0) {
                if (retry_count++ < 3) {
                    goto retry_sendto;
                }   
            }
            break;
        }
        default:
            return -3;
    }

    return 0;
}

static int OI_LogParse(const char *pszLogFileName, LogParseCallback pCallback, void *pCtx)
{
    int iLen;
    int iType;
    int iRet = 0;
    FILE *pLogFile;
    char sBuf[LOG_BUFFER_MAX_LEN];

    if((pLogFile = fopen(pszLogFileName, "r")) == NULL)
        return -1; // OPEN FAILED
    
    while (!feof(pLogFile)) {
        // get Type
        if (fread(&iType, 1, sizeof(iType), pLogFile) < sizeof(iType)) {
            if (feof(pLogFile))
                iRet = 0;  // NORMAL CASE
            else
                iRet = -2; // FORMAT ERROR
            break;
        }
        iType = ntohl(iType);
        // get Length
        if (fread(&iLen, 1, sizeof(iLen), pLogFile) < sizeof(iLen)) {
            iRet = -3; // FORMAT ERROR
            break;
        }
        iLen = ntohl(iLen);
        if (iLen > LOG_BUFFER_MAX_LEN) {
            iRet = -4; // OVERFLOW
            break;
        }
        // get buffer
        if (fread(&sBuf, 1, iLen, pLogFile) < iLen) {
            iRet = -5; // FORMAT ERROR
            break;
        }

        if ((iRet = pCallback(iType, iLen, sBuf, pCtx)) < 0) {
            iRet = iRet * 10;
            break;
        }
    }
    fclose(pLogFile);
    return iRet;
}

int OI_LogIterate(const char *pszLogFileName, LogIteratorCallback pUserCallback)
{
    if (pszLogFileName == NULL)
        return -1;

    return OI_LogParse(pszLogFileName, OI_LogParseIterator, pUserCallback) * 10;
}

int OI_LogDump(const char *pszLogFileName)
{
    if (pszLogFileName == NULL)
        return -1;

    return OI_LogParse(pszLogFileName, OI_LogParseDumper, stdout) * 10;
}

int OI_LogSend(const char *pszLogFileName, const char *sDestIP, const char *sDestPort)
{
    struct LogParseSenderContext stCtx;
    int iRet;

    if (pszLogFileName == NULL || sDestIP == NULL || sDestPort == NULL)
        return -1;

    stCtx.fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(stCtx.fd < 0)
        return -2;
    if (MakeSockAddrIn(&(stCtx.dest), (char *)sDestIP, (char *)sDestPort) < 0)
        return -3;
    iRet = OI_LogParse(pszLogFileName, OI_LogParseSender, &stCtx);
    close(stCtx.fd);
    return iRet * 10;
}

/*
 * 二进制Log每条记录的格式为:
 * Record = wRecLen1 + [TLV_Record + TLV_Record + ...]  + wRecLen2
 * wRecLen1 = wRecLen2 = sizeof(Record) - 2
 * TLV_Record = cType + wFiledLen + sField
 */
int
OI_SLogBin(int iLogTime, ...)
{
    struct timeval stLogTv;

    //int iRet;
    int iLogLen = 0, iRecLen = 0;
    int iSegLen;
    va_list ap;
    unsigned char ucType;
    char *pVal;
    char cVal;
    short shVal;
    int iVal;
    long lVal;
    double dVal;
    unsigned short usSize;
    static char sBuf[MAX_SHM_LOG_REC_LEN];
    char *pBuf = sBuf;
    int iLenLeft = sizeof(sBuf);
    char *pShmBuf;
    OI_LogShm *pstLogShm = stSFLog.pstLogShm;

    if(pstLogShm == NULL)
        return 0;


    pBuf += 2;
    iLenLeft -= 2;  //预留用于填长度

    if(iLogTime == 1)
    {
        if(AddChar(&pBuf, &iLenLeft, LOG_TIME1) < 0)
            return -1;
        if(AddWord(&pBuf, &iLenLeft, sizeof(time_t)) < 0)
            return -1;
        if(AddDDWord(&pBuf, &iLenLeft, time(NULL)) < 0)
            return -1;
    }
    else if(iLogTime == 2)
    {
        gettimeofday(&stLogTv, NULL);
        if(AddChar(&pBuf, &iLenLeft, LOG_TIME2) < 0)
            return -1;
        if(AddString(&pBuf, &iLenLeft, (char *) &stLogTv, sizeof(stLogTv)) < 0)
            return -1;
    }
    else
    {
        if(AddChar(&pBuf, &iLenLeft, LOG_TIME0) < 0)
            return -1;
        if(AddWord(&pBuf, &iLenLeft, 0) < 0)
            return -1;
    }

    va_start(ap, iLogTime);
    while(1)
    {
        ucType = (unsigned char) va_arg(ap, int);

        if(ucType == LOG_TAIL)
        {
            break;
        }

        pVal = va_arg(ap, char *);

        if(AddChar(&pBuf, &iLenLeft, LOG_DESC_STRING) < 0)
            return -1;
        if(pVal == NULL)
        {
            if(AddWord(&pBuf, &iLenLeft, 0) < 0)
                return -1;
        }
        else
        {
            if(AddString(&pBuf, &iLenLeft, pVal, strlen(pVal)) < 0)
                return -1;
        }

        if(AddChar(&pBuf, &iLenLeft, ucType) < 0)
            return -1;
        switch (ucType)
        {
            case LOG_CHAR:
                cVal = (char) va_arg(ap, int);
                if(AddWord(&pBuf, &iLenLeft, sizeof(char)) < 0)
                    return -1;
                if(AddChar(&pBuf, &iLenLeft, cVal) < 0)
                    return -1;
                break;
            case LOG_SHORT:
                shVal = (short) va_arg(ap, int);
                if(AddWord(&pBuf, &iLenLeft, sizeof(short)) < 0)
                    return -1;
                if(AddWord(&pBuf, &iLenLeft, shVal) < 0)
                    return -1;
                break;
            case LOG_INT:
                iVal = va_arg(ap, int);
                if(AddWord(&pBuf, &iLenLeft, sizeof(int)) < 0)
                    return -1;
                if(AddDWord(&pBuf, &iLenLeft, iVal) < 0)
                    return -1;
                break;
            case LOG_LONG:
                lVal = va_arg(ap, long);
                if(AddWord(&pBuf, &iLenLeft, sizeof(long)) < 0)
                    return -1;
                if(AddDDWord(&pBuf, &iLenLeft, lVal) < 0)
                    return -1;
                break;
            case LOG_DOUBLE:
                dVal = va_arg(ap, double);
                if(AddWord(&pBuf, &iLenLeft, sizeof(double)) < 0)
                    return -1;
                if(AddDDWord(&pBuf, &iLenLeft, dVal) < 0)
                    return -1;
                break;
            case LOG_STRING:
                pVal = va_arg(ap, char *);

                if(pVal == NULL)
                {
                    if(AddWord(&pBuf, &iLenLeft, 0) < 0)
                        return -1;
                }
                else
                {
                    if(AddString(&pBuf, &iLenLeft, pVal, strlen(pVal)) < 0)
                        return -1;
                }
                break;
            case LOG_NSTRING:
            case LOG_BUF:
            case LOG_BUF2:
                pVal = va_arg(ap, char *);
                usSize = (unsigned short) va_arg(ap, int);

                if(pVal == NULL)
                {
                    if(AddWord(&pBuf, &iLenLeft, 0) < 0)
                        return -1;
                }
                else
                {
                    if(AddString(&pBuf, &iLenLeft, pVal, usSize) < 0)
                        return -1;
                }
                break;
            default:
                return -1;
                break;
        }
    }
    va_end(ap);

    iRecLen = pBuf - sBuf;
    if(AddWord(&pBuf, &iLenLeft, iRecLen) < 0)
        return -1;
    *(unsigned short *) sBuf = iRecLen;
    iLogLen = pBuf - sBuf;


    if(pstLogShm->lTailPos >= SHM_LOG_BUF_LEN)
    {
        pstLogShm->lHeadPos = pstLogShm->lTailPos = 0;
        return -1;
    }
    if(pstLogShm->lTailPos != pstLogShm->lHeadPos && (pstLogShm->lHeadPos + SHM_LOG_BUF_LEN - pstLogShm->lTailPos) % SHM_LOG_BUF_LEN <= iLogLen)
    {
        pstLogShm->lHeadPos = (pstLogShm->lHeadPos + iLogLen) % SHM_LOG_BUF_LEN;
    }

    pShmBuf = pstLogShm->sLogBuf + pstLogShm->lTailPos;
    iSegLen = (pstLogShm->lTailPos + iLogLen) % SHM_LOG_BUF_LEN;
    if(iSegLen >= pstLogShm->lTailPos)
    {
        memcpy(pShmBuf, sBuf, iLogLen);
    }
    else
    {
        memcpy(pShmBuf, sBuf, iLogLen - iSegLen);
        memcpy(pstLogShm->sLogBuf, sBuf + (iLogLen - iSegLen), iSegLen);
    }
    pstLogShm->lTailPos = iSegLen;

    return 0;
}

static int
ParseRecord(OI_LogFile * pstLogFile, char *sBuf, int iRecordLen)
{
    char *pBuf = sBuf;
    int iLenLeft = iRecordLen;

    while(iLenLeft > 0)
    {
        unsigned char ucType;
        int iFieldLen;
        char cVal;
        short shVal;
        int iVal;
        long lVal;
        double dVal;
        struct timeval stLogTv;
        static char sBuf2[MAX_SHM_LOG_REC_LEN];

        if(GetChar(&pBuf, &iLenLeft, (char *)&ucType) < 0)
            return -1;
        //fprintf(pstLogFile->pLogFile, "Type %d:\n", ucType);
        switch (ucType)
        {
            case LOG_CHAR:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                if(GetChar(&pBuf, &iLenLeft, &cVal) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "%c\n", cVal);
                break;
            case LOG_SHORT:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *)&shVal) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "%u\n", shVal);
                break;
            case LOG_INT:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                if(GetDWord(&pBuf, &iLenLeft, (unsigned int *) &iVal) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "%d\n", iVal);
                break;
            case LOG_LONG:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
#if (__WORDSIZE == 32)
                if(GetDWord(&pBuf, &iLenLeft, (unsigned int *)&lVal) < 0)
                    return -1;
#else
                if(GetDDWord(&pBuf, &iLenLeft, (unsigned long *)&lVal) < 0)
                    return -1;
#endif
                fprintf(pstLogFile->pLogFile, "%ld\n", lVal);
                break;
            case LOG_DOUBLE:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                if(GetDDWord(&pBuf, &iLenLeft, (unsigned long *) &dVal) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "%f\n", dVal);
                break;
            case LOG_DESC_STRING:  //无换行
                iFieldLen = sizeof(sBuf2) - 1;  //预留填0
                if(GetBuffer(&pBuf, &iLenLeft, sBuf2, &iFieldLen) < 0)
                    return -1;
                sBuf2[iFieldLen] = 0;
                fprintf(pstLogFile->pLogFile, "%s", sBuf2);
                break;
            case LOG_STRING:
            case LOG_NSTRING:
                iFieldLen = sizeof(sBuf2) - 1;  //预留填0
                if(GetBuffer(&pBuf, &iLenLeft, sBuf2, &iFieldLen) < 0)
                    return -1;
                sBuf2[iFieldLen] = 0;
                fprintf(pstLogFile->pLogFile, "%s\n", sBuf2);
                break;
            case LOG_BUF:
                iFieldLen = sizeof(sBuf2);
                if(GetBuffer(&pBuf, &iLenLeft, sBuf2, &iFieldLen) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "[Buffer len:%d]\n%s\n", iFieldLen, DumpHex(sBuf2, 0, iFieldLen));
                break;
            case LOG_BUF2:
                iFieldLen = sizeof(sBuf2);
                if(GetBuffer(&pBuf, &iLenLeft, sBuf2, &iFieldLen) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "[Buffer len:%d]\n%s\n", iFieldLen, DumpPackage(sBuf2, iFieldLen));
                break;
            case LOG_TIME0:
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                break;
            case LOG_TIME1:
                iFieldLen = sizeof(sBuf2);
                if(GetWord(&pBuf, &iLenLeft, (unsigned short *) &iFieldLen) < 0)
                    return -1;
                if(GetDWord(&pBuf, &iLenLeft, (unsigned int *)&lVal) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "[%s]\n", OI_GetDateTimeStr((const time_t *) &lVal));
                break;
            case LOG_TIME2:
                iFieldLen = sizeof(stLogTv);
                if(GetBuffer(&pBuf, &iLenLeft, (char *) &stLogTv, &iFieldLen) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "[%s.%.6d]\n", OI_GetDateTimeStr((const time_t *) &(stLogTv.tv_sec)), (int) stLogTv.tv_usec);
                break;
            default:
                iFieldLen = sizeof(sBuf2);
                if(GetBuffer(&pBuf, &iLenLeft, sBuf2, &iFieldLen) < 0)
                    return -1;
                fprintf(pstLogFile->pLogFile, "[Type %d Buffer len: %d]\n%s\n", ucType, iFieldLen, DumpHex(sBuf2, 0, iFieldLen));
                break;
        }
    }
    return 0;
}

static int
ParseShm(OI_LogFile * pstLogFile, char *sTmpShm, int iHeadRec, int iTailRec)
{
    char *pCur = sTmpShm + iHeadRec;
    int iLeftLen = iTailRec - iHeadRec;
    static char sBuf[MAX_SHM_LOG_REC_LEN];

    //char *pBuf;
    int iRecLen1, iRecLen2;

    //int iBufLen;

    while(pCur < sTmpShm + iTailRec && iLeftLen > 0)
    {
        iRecLen1 = sizeof(sBuf);
        if(GetBuffer(&pCur, &iLeftLen, sBuf, &iRecLen1) < 0)
            return -1;
        if(iRecLen1 < sizeof(short))
            return -1;  //末尾有两字节表示长度
        iRecLen2 = *(short *) (sBuf + iRecLen1 - 2);
        if(iRecLen1 != iRecLen2)
        {
            return -1;
        }
        fprintf(pstLogFile->pLogFile, "------------------------->\n");
        if(ParseRecord(pstLogFile, sBuf, iRecLen1 - sizeof(short)) < 0)
        {
            fprintf(pstLogFile->pLogFile, "ParseRecord Error!!Dump Hex\n");
            fprintf(pstLogFile->pLogFile, "%s\n", DumpHex(sBuf, 0, iRecLen1));
        }
        fprintf(pstLogFile->pLogFile, "<-------------------------\n");
    }

    return 0;
}

static int
DumpBinLogShmToFile(OI_LogShmFile * pstLogShmFile, int iLogTime, char *sFormat, ...)
{
    va_list ap;
    struct timeval stLogTv;
    int iShmID;
    int iTmpKey = 0x20111010; //add by dieogjiang at 2011-10-10

    //int iBufLen;
    int iRecCount = 0;
    char *sTmpShm;
    OI_LogFile *pstLogFile = &pstLogShmFile->stLogFile;
    OI_LogShm *pstLogShm = pstLogShmFile->pstLogShm;

    if(pstLogFile == NULL || pstLogShm == NULL)
        return 0;

    if((pstLogFile->pLogFile = fopen(pstLogFile->sLogFileName, "a+")) == NULL)
        return -1;

    if(iLogTime == 1)
    {
        fprintf(pstLogFile->pLogFile, "[%s] ", OI_GetCurDateTimeStr());
    }
    else if(iLogTime == 2)
    {
        gettimeofday(&stLogTv, NULL);
        fprintf(pstLogFile->pLogFile, "[%s.%.6d] ", OI_GetDateTimeStr((const time_t *) &(stLogTv.tv_sec)), (int) stLogTv.tv_usec);
    }

    va_start(ap, sFormat);
    vfprintf(pstLogFile->pLogFile, sFormat, ap);
    fprintf(pstLogFile->pLogFile, "\n");
    va_end(ap);

    fprintf(pstLogFile->pLogFile, "Start dump debug records to file , Head:%d Tail:%d BufLen:%d ...\n", pstLogShm->lHeadPos, pstLogShm->lTailPos,
            SHM_LOG_BUF_LEN);


    srand(time(NULL));
    iTmpKey += rand() % 1000; //modified by diegojiang at 2011-10-10
    if(!(sTmpShm = OI_GetShm4(iTmpKey, SHM_LOG_BUF_LEN, 0666 | IPC_CREAT, &iShmID)))
    {
        fprintf(pstLogFile->pLogFile, "Get tmp Shm error\n");
    }
    else
    {
        /*iHeadRec是第一条记录的开始地址, iTailRec是最后一条记录的末尾地址 */
        int iHeadRec, iTailRec;

        //按时间顺序拷贝到新临时内存中,避免后面的逻辑过于复杂*/
        if(pstLogShm->lHeadPos <= pstLogShm->lTailPos)
        {
            memcpy(sTmpShm, pstLogShm->sLogBuf + pstLogShm->lHeadPos, pstLogShm->lTailPos - pstLogShm->lHeadPos);
            iTailRec = pstLogShm->lTailPos - pstLogShm->lHeadPos;
        }
        else
        {
            memcpy(sTmpShm, pstLogShm->sLogBuf + pstLogShm->lHeadPos, SHM_LOG_BUF_LEN - pstLogShm->lHeadPos);
            memcpy(sTmpShm + SHM_LOG_BUF_LEN - pstLogShm->lHeadPos, pstLogShm->sLogBuf, pstLogShm->lTailPos);
            iTailRec = pstLogShm->lTailPos - pstLogShm->lHeadPos + SHM_LOG_BUF_LEN;
        }

        /*从最后一条记录开始找,尽可能找到最前面的完整记录 */
        iHeadRec = iTailRec;
        iRecCount = 0;
        while(iHeadRec - 2 > 0 && (!pstLogShmFile->lMaxLogRec || iRecCount < pstLogShmFile->lMaxLogRec))
        {
            static short shRecLen1, shRecLen2;

            shRecLen2 = *(unsigned short *) (sTmpShm + iHeadRec - 2);
            if(iHeadRec - 2 - shRecLen2 < 0)
                break;
            shRecLen1 = *(unsigned short *) (sTmpShm + iHeadRec - 2 - shRecLen2);
            if(shRecLen1 != shRecLen2)
                break;  //如果不相等,前面的内容可能都有问题
            iHeadRec -= 2 + shRecLen2;
            iRecCount++;
        }
        fprintf(pstLogFile->pLogFile, "HeadRec %u,iTailRec %u, iRecCount %d\n", iHeadRec, iTailRec, iRecCount);
        /*扫描内存中的记录,如果扫描失败,则log所有二进制内容 */
        if(ParseShm(pstLogFile, sTmpShm, iHeadRec, iTailRec) < 0)
        {
            fprintf(pstLogFile->pLogFile, "ParseShm Error!!Dump Hex\n%s\n", DumpHex(sTmpShm, iHeadRec, iTailRec - iHeadRec));
        }
        shmctl(iShmID, IPC_RMID, NULL);
        shmdt(sTmpShm);
    }

    fprintf(pstLogFile->pLogFile, "Done dump debug Records to file\n");

    pstLogShm->lTailPos = pstLogShm->lHeadPos = 0;
    pstLogFile->lLogCount++;
    fclose(pstLogFile->pLogFile);
    return ShiftFiles(pstLogFile);
}

static int
GetBinLogShm(OI_LogShm ** pstLogShm, int iShmKey)
{
    char *sShm;

    if(OI_GetShm2((void **) &sShm, iShmKey, sizeof(OI_LogShm), IPC_CREAT | 0666) < 0)
    {
        return -1;
    }

    *pstLogShm = (OI_LogShm *) sShm;

    return 0;
}

int
OI_InitBinLogShmFile(int iShmKey, char *sLogBaseName, int lShiftType, int lMaxLogNum, int lMAX, int lMaxLogRec)
{
    //char sLogFileName[300] = "";
    OI_LogFile *pstLogFile = &stSFLog.stLogFile;

    memset(&stSFLog, 0, sizeof(OI_LogShmFile));

    strncat(pstLogFile->sLogFileName, sLogBaseName, sizeof(pstLogFile->sLogFileName) - 10);
    strcat(pstLogFile->sLogFileName, ".log");
    // if ((pstLogFile->pLogFile = fopen(sLogFileName, "a+")) == NULL) return -1;
    strncpy(pstLogFile->sBaseFileName, sLogBaseName, sizeof(pstLogFile->sBaseFileName) - 1);
    pstLogFile->iShiftType = lShiftType;
    pstLogFile->iMaxLogNum = lMaxLogNum;
    pstLogFile->lMaxSize = lMAX;
    pstLogFile->lMaxCount = lMAX;
    pstLogFile->lLogCount = lMAX;
    time(&pstLogFile->lLastShiftTime);

    stSFLog.lMaxLogRec = lMaxLogRec;

    GetBinLogShm(&stSFLog.pstLogShm, iShmKey);
    DumpBinLogShmToFile(&stSFLog, 2, "Dump old shm log while InitLogFileShm ...");

    return ShiftFiles(pstLogFile);
}

int OI_PkgFlowLog(char *pFlowLogBuff, int *piFlowLogBuffLen,
        char cStoreVer, unsigned int dwServerType, unsigned char cOpType, unsigned int dwUin, unsigned int dwUpdateTime, 
        unsigned int dwClientIP, unsigned int dwServiceConnIP, unsigned int dwServiceSvrIP, 
        unsigned short wClientType, unsigned int dwClientVersion, const char* sFormat, ...)
{
    int iRet = 0;
    int iLen = 0;
    int iHeadLen = 0;
    char sClientIP[20] = {0};
    char sServiceConnIP[20] = {0};
    char sServiceSvrIP[20] = {0};
    va_list ap;
    int iOutputLen = 0;

    if ( (pFlowLogBuff == NULL) || (piFlowLogBuffLen == NULL) || (*piFlowLogBuffLen <= 0) )
    {
        return -1;
    }

    iLen = *piFlowLogBuffLen;
    strncpy(sClientIP, inet_ntoa(*(struct in_addr*)(&dwClientIP)), sizeof(sClientIP)-1);
    strncpy(sServiceConnIP, inet_ntoa(*(struct in_addr*)(&dwServiceConnIP)), sizeof(sServiceConnIP)-1);
    strncpy(sServiceSvrIP, inet_ntoa(*(struct in_addr*)(&dwServiceSvrIP)), sizeof(sServiceSvrIP)-1);
    iRet = snprintf(pFlowLogBuff, iLen, "%d|%u|%u|%u|%u|%s|%s|%s|%u|%u%s", 
            cStoreVer, dwServerType, cOpType, dwUin, dwUpdateTime, sClientIP, sServiceConnIP, sServiceSvrIP, wClientType, dwClientVersion,
            sFormat ? "|" : "");
    if ( (iRet < 0) || (iRet >= iLen) )
    {
        return -2;
    }
    iOutputLen += iRet;

    if (sFormat)
    {
        iHeadLen = iRet;
        iLen -= iRet;
        va_start(ap, sFormat);
        iRet = vsnprintf(pFlowLogBuff + iHeadLen, iLen, sFormat, ap);
        if ( (iRet < 0) || (iRet >= iLen) )
        {
            va_end(ap);
            return -3;
        }
        va_end(ap);
        iOutputLen += iRet;
    }

    *piFlowLogBuffLen = iOutputLen;
    return 0;
}

#define STX_TRASH 0x07
#ifndef ETX
#define ETX  0x03
#endif


#define ATTR_SEND_DELETEDINFO       90145
#define ATTR_RECV_ACK_RET_OK        90146
#define ATTR_RECV_ACK_RET_ERR       90147
#define ATTR_RECV_ACK_PKT_ERR       90148

static int
__CreateUDPSocket(struct sockaddr_in *pstAddr)
{
    int s = 0;
    int flags = 1;              //nonblock reusaddr

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
        printf("[%s] setsockopt failed SO_REUSEADDR (%s)!", __FUNCTION__, strerror(errno));
    }

    if(bind(s, (struct sockaddr *) pstAddr,  sizeof(struct sockaddr_in)) == -1)
    {
        printf("[%s] bind %s:%d failed (%s)!", 
            __FUNCTION__, inet_ntoa(pstAddr->sin_addr), ntohs(pstAddr->sin_port), strerror(errno));
        return -2;
    }
    return s;
}

int
OI_SendDeletedInfo(unsigned int dwLocalIP, unsigned int dwTrashCenterIP, unsigned short wTrashCenterPort,
    unsigned short wBusinessType, unsigned int dwUin, unsigned int dwFUin,
    unsigned int dwInterfaceIP, unsigned int dwDelUinCenterIP,
    char *pBuf, unsigned short wBufLen)
{
    static int iTrashSocket = -1;
    struct sockaddr_in stTrashCenterIn;
    struct sockaddr_in stAddrFrom;
    int iFromLen = sizeof(struct sockaddr_in);
    char sBuf[65535];
    char *pCur = sBuf;
    int iSendLen = sizeof(sBuf);
    TRASH_HEAD *pHead;
    int iRetryCount = 5;
    int iRet;

    if (pBuf == NULL)
        return -1;

    if (wBufLen > 0xFFFF - sizeof(TRASH_HEAD) - 2)
        return -2;
    
    if (iTrashSocket < 0)
    {
        struct sockaddr_in stAddrIn;
        memset(&stAddrIn, 0, sizeof(struct sockaddr_in));
        stAddrIn.sin_family = AF_INET;
        stAddrIn.sin_addr.s_addr = dwLocalIP;
        stAddrIn.sin_port = 0;
        iTrashSocket = __CreateUDPSocket(&stAddrIn);
        if (iTrashSocket < 0)
            return -10;
        // iTrashSocket is NONBLOCK
    }

    memset(&stTrashCenterIn, 0, sizeof(struct sockaddr_in));
    stTrashCenterIn.sin_family = AF_INET;
    stTrashCenterIn.sin_addr.s_addr = dwTrashCenterIP;
    stTrashCenterIn.sin_port = htons(wTrashCenterPort);

    // Req = STX_THRASH + TRASH_HEAD + BODY + ETX 
    if (AddChar(&pCur, &iSendLen, STX_TRASH) < 0)
        return -3;
   
    pHead = (TRASH_HEAD *)pCur;
    pCur += sizeof(TRASH_HEAD);
    iSendLen -= sizeof(TRASH_HEAD);
    memset(pHead, 0, sizeof(TRASH_HEAD));
    pHead->wBusinessType = htons(wBusinessType);
    pHead->dwUin = htonl(dwUin);
    pHead->dwFUin = htonl(dwFUin);
    pHead->dwDelUinCenterIP = dwDelUinCenterIP;
    pHead->dwInterfaceIP = dwInterfaceIP;
    pHead->dwTimeStamp = htonl(time(NULL));

    memcpy(pCur, pBuf, wBufLen);
    pCur += wBufLen;
    iSendLen -= wBufLen;

    if (AddChar(&pCur, &iSendLen, ETX) < 0)
        return -3;

    pHead->wPkgLen = htons(pCur - sBuf);

retry_send:
    iRet = sendto(iTrashSocket, sBuf, pCur - sBuf, 0, 
        (struct sockaddr*)&stTrashCenterIn, sizeof(struct sockaddr_in));
    if (iRet < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            if (iRetryCount-- > 0)                    
                goto retry_send;
        }
        return -4;
    } else {
//        Attr_API(ATTR_SEND_DELETEDINFO, 1);
    }

retry_recv:
    iRet = recvfrom(iTrashSocket, sBuf, sizeof(sBuf), 0, (struct sockaddr*)&stAddrFrom, (socklen_t *)&iFromLen);
    if (iRet < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // no message recv... just go out.
            return 0;
        }
        return -5;
    } else if (iRet > 0) {
        // Rsp = STX_THRASH + TRASH_HEAD + cResult + ETX 
        if (iRet == sizeof(TRASH_HEAD) + 2 + 1) {
            if (sBuf[0] == STX_TRASH && sBuf[iRet - 1] == ETX) {
                if (sBuf[sizeof(TRASH_HEAD) + 1] == 0) { // cResult 
            //        Attr_API(ATTR_RECV_ACK_RET_OK, 1);
                    goto retry_recv;
                } else {
            //        Attr_API(ATTR_RECV_ACK_RET_ERR, 1);
                    goto retry_recv;
                }
            }
        }
//        Attr_API(ATTR_RECV_ACK_PKT_ERR, 1);
        goto retry_recv;
    }

    return 0;
}

int 
OI_IterateDeletedInfo(const char *pszDelFileName, DeletedInfoCallback pCallback, void *pCtx,
    unsigned short wBusinessType, unsigned int dwUin, unsigned int dwFUin)
{
    int iLen;
    char cSTX;
    TRASH_HEAD stHead;
    char sBuf[65535];
    int iRet = 0;
    FILE *pLogFile;

    if((pLogFile = fopen(pszDelFileName, "r")) == NULL)
        return -1; // OPEN FAILED
    
    while (!feof(pLogFile)) {
        // get STX_TRASH
        if (fread(&cSTX, 1, sizeof(cSTX), pLogFile) < sizeof(cSTX)) {
            if (feof(pLogFile))
                iRet = 0;  // NORMAL CASE
            else
                iRet = -2; // FORMAT ERROR
            break;
        }
        if (cSTX != STX_TRASH) {
            iRet = -3; // FORMAT ERROR
            break;
        }
        // get TRASH_HEAD
        if (fread(&stHead, 1, sizeof(stHead), pLogFile) < sizeof(stHead)) {
            iRet = -4; // FORMAT ERROR
            break;
        }
        iLen = ntohs(stHead.wPkgLen) - 2 - sizeof(TRASH_HEAD);
        if (iLen > sizeof(sBuf)) {
            iRet = -5;
            break;
        }

        if ((ntohs(stHead.wBusinessType) == wBusinessType) &&
            ((dwUin == 0xFFFFFFFF) || (ntohl(stHead.dwUin) == dwUin)) &&
            ((dwFUin == 0xFFFFFFFF) || (ntohl(stHead.dwFUin) == dwFUin)) ) 
        {
            // get buffer
            if (fread(sBuf, 1, iLen, pLogFile) < iLen) {
                iRet = -6; // FORMAT ERROR
                break;
            }

            if ((iRet = pCallback(&stHead, sBuf, iLen, pCtx)) < 0) {
                iRet = iRet * 100;
                break;
            }
        } 
        else 
        {
            // skip buffer.
            if (fseek(pLogFile, iLen, SEEK_CUR) < 0) {
                iRet = -7;
                break;
            }
        }
        // get ETX
        if (fread(&cSTX, 1, sizeof(cSTX), pLogFile) < sizeof(cSTX)) {
            iRet = -8; // FORMAT ERROR
            break;
        }
        if (cSTX != ETX) {
            iRet = -9; // FORMAT ERROR
            break;
        }
    }
    fclose(pLogFile);
    return iRet;
}
