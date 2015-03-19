#ifndef __SVRLOG_H__
#define __SVRLOG_H__

#include <string.h>

#include "ids_basic_log.h"
#include "lib_utils.h"

typedef struct{
	char cLogFileName[256];
	LogFile stLogEvent;
	
	//需要跟踪的号码，不管日志级别如何设置，该号码的信息，都会进行跟踪
	unsigned long ulTraceUin;    
	int iLogLevel;
}SVRLOG;

/**********************************************************************
各级别详细定义：（向下嵌套，如：级别设置为WARN就会包含WARN、ERROR、FATAL信息）
ALL:所有LOG信息
DEBUG:调试类信息
INFO:流水类信息
WARN:警告类信息
ERROR:一般性错误信息
FATAL:致命错误信息
OFF:关闭信息

建议默认设置使用4级
DEBUG(5)、INFO(4)、WARN(3)、ERROR(2)
 ***********************************************************************/
#define LOGLEVEL_ALL    7
#define LOGLEVEL_DEBUGBUF  6
#define LOGLEVEL_DEBUG  5
#define LOGLEVEL_INFO   4
#define LOGLEVEL_WARN   3
#define LOGLEVEL_ERROR  2
#define LOGLEVEL_FATAL  1
#define LOGLEVEL_OFF    0

extern SVRLOG gstLog;        //需要在主函数所在的文件里先定义这个全局变量
extern unsigned long ulCurUin; //需要在主函数所在的文件里先定义这个全局变量

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

//一种简单的定位问题的方法
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

