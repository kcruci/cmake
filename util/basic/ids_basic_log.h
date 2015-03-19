#ifndef _IDS_BASIC_LOG_H
#define _IDS_BASIC_LOG_H

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE   //for LFS support
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64    //for LFS support
#endif

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "ids_basic_str.h"

#ifdef  __cplusplus
extern "C"
{
#endif


    typedef struct
    {
        FILE *pLogFile;
        char sBaseFileName[256];
        char sLogFileName[256];
        int iShiftType;    // 0 -> ��С,  1 -> �ļ���, 2 -> shift by interval, 3 ->��, 4 -> Сʱ, 5 ->���� 
        int iMaxLogNum;
        int lMaxSize;
        int lMaxCount;
        int lLogCount;
        time_t lLastShiftTime;
    } OI_LogFile, LogFile ;

#define SHM_LOG_BUF_LEN 2000000
#define MAX_SHM_LOG_REC_LEN 65535

    typedef struct
    {
        unsigned int lHeadPos;
        unsigned int lTailPos; //��һ�����õ�λ��
        char sReserved[20];
        char sLogBuf[SHM_LOG_BUF_LEN];
    } OI_LogShm;

    typedef struct
    {
        OI_LogFile stLogFile;
        OI_LogShm *pstLogShm;
        int lMaxLogRec;        //���Dump�������¼��
        char sReserved[20];
    } OI_LogShmFile;

//
#define LOG_TAIL 		(unsigned char)0    //��־��εĽ���
#define LOG_CHAR    	(unsigned char)1
#define LOG_SHORT    	(unsigned char)2
#define LOG_INT     	(unsigned char)3
#define LOG_LONG    	(unsigned char)4
#define LOG_DOUBLE  	(unsigned char)5
#define LOG_STRING  	(unsigned char)6
#define LOG_DESC_STRING (unsigned char)7
#define LOG_NSTRING  	(unsigned char)8
#define LOG_BUF			(unsigned char)9
#define LOG_BUF2		(unsigned char)10
//
#define LOG_TIME0	(unsigned char)65
#define LOG_TIME1	(unsigned char)66
#define LOG_TIME2	(unsigned char)67

#define BASIC_ARGS 	LOG_STRING, "Function: ", __FUNCTION__,LOG_INT,"Line: ", __LINE__ 
#define SLOG0(fmt, args...) OI_SLogBin(0, BASIC_ARGS fmt, ## args, LOG_TAIL)
#define SLOG1(fmt, args...) OI_SLogBin(1, BASIC_ARGS fmt, ## args, LOG_TAIL)
#define SLOG2(fmt, args...) OI_SLogBin(2, BASIC_ARGS fmt, ## args, LOG_TAIL)
#define SLOG SLOG1


#define  InitLogFile(pstLogFile,sLogBaseName,iShiftType,iMaxLogNum,iMAX) OI_InitLogFile(pstLogFile,sLogBaseName,iShiftType,iMaxLogNum,iMAX)
#define  InitBinLogShmFile(iShmKey,sLogBaseName,lShiftType,lMaxLogNum,lMAX,lMaxLogRec)  OI_InitBinLogShmFile(iShmKey,sLogBaseName,lShiftType,lMaxLogNum,lMAX,lMaxLogRec)
#define  Log(pstLog,iLogTime,fmt,args...)  OI_Log(pstLog, iLogTime,fmt,## args)
#define  LogBin(pstLog,iLogTime,sBuf,iBufLen)  OI_LogBin(pstLog, iLogTime, sBuf, iBufLen)
#define  OI_LOGINFO(pstLog,fmt, args...)  OI_Log(pstLog, 2, "%s:%d(%s): " fmt, __FILE__, __LINE__, __FUNCTION__ , ## args)

    enum {
        LOGBIN_TIME_SIMPLE = 1,
        LOGBIN_TIME,
        LOGBIN_BUFFER,
    };

    //iShiftType;
    //0->shift by size, 
    //1 or default->shift by LogCount, 
    //2->shift by interval, seconds > iMAX 
    //3->shift by day, 
    //4->shift by hour 
    //5->shift by minute
    //6->shift by day, log filename append with date.
    //LOG �ȵ� OI_InitLogFile ��ʼ��
    int OI_InitLogFile(OI_LogFile * pstLogFile, char *sLogBaseName, int iShiftType, int iMaxLogNum, int iMAX);
    int OI_Log(OI_LogFile * pstLogFile, int iLogTime, const char *sFormat, ...) __attribute__ ((format (printf, 3, 4))) ;
    // @brief   OI_LogBin: Log bin buffer to logfile.
    //          LogBin file struct = [TLV], 
    //              T(4Bytes) = LOGBIN_TIME_SIMPLE/LOGBIN_TIME/LOGBIN_BUFFER. 
    //              L(4Bytes) = length of context.
    //              VALUE(L Bytes)
    // @param pstLogFile    pointer to log file struct.
    // @param iLogTime      0 = do not log time, 1 = log simple time, 2 = log detail time.
    // @param sBuf          buffer to log.
    // @param iBufLen       buffer length.
    // @return <0 means failure.
    int OI_LogBin(OI_LogFile *pstLogFile, int iLogTime, const char *sBuf, int iBufLen);
	/***
	  SlogBin(1,
	//����, "ע��", log������, [���ݵĳ���,]
	LOG_CHAR, "cmd type:", 5,
	LOG_INT, "BufLen type:", 530,
	LOG_STRING, "Msg:", "Hello world\n",
	LOG_NSTRING, "Msg2:", sMsg, strlen(sMsg),
	LOG_BUF, "Pkg:", &stRpkg, sizeof(stRpkg),
	LOG_TAIL);
	 ***/
    int OI_SLogBin(int iLogTime, ...);

    /*
     *lMaxLogNum ����ļ���Ŀ
     *lMAX  ÿ���ļ���󳤶�
     *lMaxLogRec Dump�ڴ浽�ļ�ʱ���dump�ļ�¼��,0��ʾ������
     */
    int OI_InitBinLogShmFile(int iShmKey, char *sLogBaseName, int lShiftType, int lMaxLogNum, int lMAX, int lMaxLogRec);

    #define LOG_BUFFER_MAX_LEN 65536 

    // @brief   LogDump: Dump Bin LogFile to screen.
    // @param   pszLogFileName Log Filename
    // @return <0 means failure.
    int OI_LogDump(const char *pszLogFileName);

    // @brief   LogSend: Send Bin LogFile's Buffers to network.
    // @param   pszLogFileName Log Filename
    // @param   sDestIP  destination IP address
    // @param   sDestPort destination Port.
    // @return <0 means failure.
    int OI_LogSend(const char *pszLogFileName, const char *sDestIP, const char *sDestPort);

    // @brief   LogIteratorCallback: user process buffer in this function.
    // @param   time: buffer logging time
    // @param   iLen: buffer len
    // @param   sBuf: buffer address.
    // @return  if process successfully, should return 0. 
    //          otherwise, return negtive value, which will abort iterate process.
    typedef int (* LogIteratorCallback)(struct timeval *time, int iLen, char *sBuf);

    // @brief   LogIterate: Iterate all log buffer.
    // @param   pszLogFileName Log Filename
    // @param   pUserCallback callback function, caller process buffer in this function.
    // @return <0 means failure.
    int OI_LogIterate(const char *pszLogFileName, LogIteratorCallback pUserCallback);

#define  LogDump(pszLogFileName)  OI_LogDump(pszLogFileName)
#define  LogSend(pszLogFileName, sDestIP, sDestPort)  OI_LogSend(pszLogFileName, sDestIP, sDestPort)
#define  LogIterate(pszLogFileName, pUserCallback)  OI_LogIterate(pszLogFileName, pUserCallback)


// @brief   PkgFlowLog: �����ĸ�ʽ������ݵ�Buffer
//          �洢��ʽ = head[|...]
//          head = cStoreVer|dwServerType|cOpType|dwUin|dwUpdateTime|dwClientIP|dwServiceConnIP|dwServiceSvrIP|wClientType|dwClientVersion
//          ����ʾ����1|3|1|69916789|1272342893|10.130.8.11|10.131.8.1|10.132.9.1|17|86|50|test by vitolin|80
// @param   pFlowLogBuff: bufferָ��
// @param   piFlowLogBuffLen: ���룺buffer����󳤶ȣ�����ȷ��buffer�㹻�����籣��head����buffer�����Ҫ124byte
//                            �����buffer���ݵ�ʵ�ʳ���
// @param   cStoreVer: ��ʽ�汾��
// @param   dwServerType: ҵ������
// @param   cOpType: ��������.
// @param   dwUin: uin
// @param   dwUpdateTime: ���ݱ��ʱ��.
// @param   dwClientIP: �ͻ���IP
// @param   dwServiceConnIP: ҵ���ķ���IP
// @param   dwServiceSvrIP: ҵ���Ľ���IP
// @param   wClientType: �ͻ�������
// @param   dwClientVersion: �ͻ��˰汾
// @param   sFormat: ��ʽ�ַ���
// @param   ...: ���
// @return  �ɹ�����0��ʧ�� < 0
int OI_PkgFlowLog(char *pFlowLogBuff, int *piFlowLogBuffLen,
        char cStoreVer, unsigned int dwServerType, unsigned char cOpType, unsigned int dwUin, unsigned int dwUpdateTime, 
        unsigned int dwClientIP, unsigned int dwServiceConnIP, unsigned int dwServiceSvrIP, 
        unsigned short wClientType, unsigned int dwClientVersion, const char* sFormat, ...);

#pragma pack(1)
/* ��������API */
typedef struct
{
    unsigned short      wPkgLen;
    char                cPtlVer;    //Э��汾����0
    unsigned short      wBusinessType;  //ҵ������
    unsigned int        dwUin;
    unsigned int        dwFUin;
    unsigned int        dwDelUinCenterIP; //��������IP
    unsigned short      wDelUinCenterRort;
    unsigned int        dwBusinessIP;     //ҵ��IP
    unsigned short      wBusinessPort;
    unsigned int        dwInterfaceIP;        //interface IP
    unsigned short      wInterfacePort;
    unsigned int        dwTimeStamp;
    char sReserved[8];
} TRASH_HEAD;
#pragma pack()

// @brief   SendDeletedInfo: ����ɾ���û����ݰ�����������
// @param   ��������IP��ַΪ������������ֵΪ������
// @param   dwLocalIP: ����IP��ַ
// @param   dwTrashCenterIP: ��������IP��ַ
// @param   dwTrashCenterPort: �������Ķ˿�
// @param   wBusinessType: ҵ������
// @param   dwUin: uin
// @param   dwFUin: friend uin, û�еĻ�����Ϊ0
// @param   dwInterfaceIP: ҵ���Ľӿڻ�IP��ַ
// @param   dwDelUinCenterIP: �������ĵ�IP��ַ
// @param   pBuf: ҵ���������
// @param   wBufLen: ҵ��������ݵĳ���
// @return  �ɹ�����0��ʧ�� < 0
int
OI_SendDeletedInfo(unsigned int dwLocalIP, unsigned int dwTrashCenterIP, unsigned short wTrashCenterPort,
    unsigned short wBusinessType, unsigned int dwUin, unsigned int dwFUin,
    unsigned int dwInterfaceIP, unsigned int dwDelUinCenterIP,
    char *pBuf, unsigned short wBufLen);

#define SendDeletedInfo(arg1, args...) OI_SendDeletedInfo(arg1, ## args)

// @brief   DeletedInfoCallback: ҵ����������ݵĻص��������������ṩ�ĺ�������IterateDeletedInfo���е���
// @param   pHead: �������ݵ�ͷ��ַ
// @param   pBuf: ҵ��������ݵ�����
// @param   wBufLen: ҵ��������ݵĳ���
// @param   pCtx: �ص������ڲ�����������,��������NULL
// @return  �ɹ�����ҵ��������ݷ���0��ʧ�ܷ��ظ���������ֹIterateDeletedInfo�ı���
typedef int (* DeletedInfoCallback)(const TRASH_HEAD *pHead, const char *pBuf, const unsigned short wBufLen, const void *pCtx);

// @brief   IterateDeletedInfo: ����ҵ����������ļ������Է��ϵ���Ŀ���ûص��������д���
// @param   pszDelFileName: ҵ����������ļ���
// @param   pCallback: �ص�����
// @param   pCtx: �ص������ڲ�����������,��������NULL
// @param   wBusinessType: ��Ҫ���˵�ҵ������
// @param   dwUin: ��Ҫ���˵�uin, 0xffffffff��ʾ���е�uin
// @param   dwFUin: ��Ҫ���˵�friend uin, û�еĻ�����Ϊ0��0xffffffff��ʾ���е�fuin
// @return  �ɹ���������ҵ��������ݷ���0��ʧ�ܷ��ظ���
int 
OI_IterateDeletedInfo(const char *pszDelFileName, DeletedInfoCallback pCallback, void *pCtx,
    unsigned short wBusinessType, unsigned int dwUin, unsigned int dwFUin);

#define IterateDeletedInfo(arg1, args...) OI_IterateDeletedInfo(arg1, ## args)

#ifdef  __cplusplus
}
#endif


#endif
