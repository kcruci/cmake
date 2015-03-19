#ifndef __SVRLOG_H__
#define __SVRLOG_H__

#include <string.h>

#include "ids_basic_log.h"
#include "lib_utils.h"

typedef struct{
	char cLogFileName[256];
	LogFile stLogEvent;
	
	//��Ҫ���ٵĺ��룬������־����������ã��ú������Ϣ��������и���
	unsigned long ulTraceUin;    
	int iLogLevel;
}SVRLOG;

/**********************************************************************
��������ϸ���壺������Ƕ�ף��磺��������ΪWARN�ͻ����WARN��ERROR��FATAL��Ϣ��
ALL:����LOG��Ϣ
DEBUG:��������Ϣ
INFO:��ˮ����Ϣ
WARN:��������Ϣ
ERROR:һ���Դ�����Ϣ
FATAL:����������Ϣ
OFF:�ر���Ϣ

����Ĭ������ʹ��4��
DEBUG(5)��INFO(4)��WARN(3)��ERROR(2)
 ***********************************************************************/
#define LOGLEVEL_ALL    7
#define LOGLEVEL_DEBUGBUF  6
#define LOGLEVEL_DEBUG  5
#define LOGLEVEL_INFO   4
#define LOGLEVEL_WARN   3
#define LOGLEVEL_ERROR  2
#define LOGLEVEL_FATAL  1
#define LOGLEVEL_OFF    0

extern SVRLOG gstLog;        //��Ҫ�����������ڵ��ļ����ȶ������ȫ�ֱ���
extern unsigned long ulCurUin; //��Ҫ�����������ڵ��ļ����ȶ������ȫ�ֱ���

//#define INIT_SVRLOG(pLogPath, ulTraceuin, iLoglevel, iMaxLogNum, iMaxLogSize) do {gstLog.ulTraceUin = ulTraceuin;gstLog.iLogLevel = iLoglevel;InitLogFile(&(gstLog.stLogEvent), pLogPath, 0, iMaxLogNum, iMaxLogSize);} while(0)

#undef LOG_ERROR
#undef LOG_WARN
#undef LOG_INFO
#undef LOG_DEBUG


#define LOG_TMP_TEST(fmt, args...) do { OI_Log(&(gstLog.stLogEvent), 2, "[TMP_TEST] " fmt , ## args);} while (0)

#define LOG_FATAL(fmt, args...) do { if (gstLog.iLogLevel >= LOGLEVEL_FATAL) OI_Log(&(gstLog.stLogEvent), 2, "[FATAL] " fmt , ## args);} while (0)
#define LOG_VIP(fmt, args...) do {OI_Log(&(gstLog.stLogEvent), 2, "" fmt , ## args);} while (0)
#define LOG_ERROR(fmt, args...) do { if (gstLog.iLogLevel >= LOGLEVEL_ERROR) OI_Log(&(gstLog.stLogEvent), 2, "[ERROR] " fmt " %s:%zu(%s)" , ## args, __FILE__, __LINE__, __FUNCTION__);} while (0)
#define LOG_WARN(fmt, args...) do { if (gstLog.iLogLevel >= LOGLEVEL_WARN) OI_Log(&(gstLog.stLogEvent), 2, "[WARN] " fmt " %s:%zu(%s)", ##args, __FILE__, __LINE__, __FUNCTION__);} while (0)
#define LOG_INFO(fmt, args...) do { if (gstLog.iLogLevel >= LOGLEVEL_INFO) OI_Log(&(gstLog.stLogEvent), 2, "[INFO] " fmt " %s:%zu(%s)", ##args, __FILE__, __LINE__, __FUNCTION__);} while (0)
#define LOG_DEBUG(fmt, args...) do { if (gstLog.iLogLevel >= LOGLEVEL_DEBUG) OI_Log(&(gstLog.stLogEvent), 2, "[DEBUG] " fmt " %s:%zu(%s)", ##args, __FILE__, __LINE__, __FUNCTION__);} while (0)

#define LOG_DEBUGBUF(sOutBuf, iLen, szMsg) do { if (gstLog.iLogLevel >= LOGLEVEL_DEBUGBUF) LogBufInfo(&(gstLog.stLogEvent), (char*)sOutBuf, iLen, szMsg);} while(0)

#define DEBUG_STATUS (gstLog.iLogLevel >= LOGLEVEL_DEBUG)

//һ�ּ򵥵Ķ�λ����ķ���
#define LOG_BUG_RET(retval) do { LOG_DEBUG("BUG!"); return (retval); } while (0)


#define COL_TITLE "______00_01_02_03_04_05_06_07_08_09_0A_0B_0C_0D_0E_0F\n"
static void LogBufInfo(LogFile *pstLogFile,char *sOutBuf, int iLen, const char *szMsg)
{
	static unsigned char sHexBuf[1000000];
	int iBufLen = 0;

	if (iLen > 20000)
	{
		iBufLen = 20000;
	}
	else
	{
		iBufLen = iLen;
	}
	strcpy((char*)sHexBuf, COL_TITLE);
	HexDumpStr(sOutBuf, iBufLen, sHexBuf + strlen(COL_TITLE));
	Log(pstLogFile, 2, "%s, pkg len:%d\n%s", szMsg, iLen, sHexBuf);
	return ;
}

#endif

