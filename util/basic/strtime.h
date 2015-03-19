#ifndef _STRTIME_EX_H_
#define _STRTIME_EX_H_
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define GetCurShortDateStr()		OI_GetCurShortDateStr()
#define GetShortDateStr(mytime)		OI_GetShortDateStr(mytime)
#define GetCurDateStr() 			OI_GetCurDateStr()
#define GetDateStr(mytime)			OI_GetDateStr(mytime)
#define GetCurDateTimeStr()			OI_GetCurDateTimeStr()
#define GetDateTimeStr(mytime)		OI_GetDateTimeStr(mytime)
#define GetCurShortDateTimeStr()	OI_GetCurShortDateTimeStr()
#define GetShortDateTimeStr(mytime)	OI_GetShortDateTimeStr(mytime)
#define GetCurTimeStr()				OI_GetCurTimeStr()
#define GetTimeStr(mytime)			OI_GetTimeStr(mytime)
#define GetCurShortTimeStr()		OI_GetCurShortTimeStr()
#define GetShortTimeStr(mytime) 	OI_GetShortTimeStr(mytime)

    //注意以下函数返回的指针指向的是一个static变量, 不要在一个函数中使用以下函数两次做参数
    char *OI_GetCurShortDateStr(void);  //20020204
    char *OI_GetShortDateStr(const time_t * mytime);    //20020204

    char *OI_GetCurDateStr(void);   //2002-02-04
    char *OI_GetDateStr(const time_t * mytime); //2002-02-04

    char *OI_GetCurDateTimeStr(void);   //2002-02-04 20:53:01
    char *OI_GetDateTimeStr(const time_t * mytime); //2002-02-04 20:53:01

    char *OI_GetCurShortDateTimeStr(void);  //20020204205301
    char *OI_GetShortDateTimeStr(const time_t * mytime);    //20020204205301

    char *OI_GetCurTimeStr(void);   //20:53:01
    char *OI_GetTimeStr(const time_t * mytime); //20:53:01

    char *OI_GetCurShortTimeStr(void);  //205301
    char *OI_GetShortTimeStr(const time_t * mytime);    //205301

    // 得到当前， 年，月，日，时，分，秒，星期(0~6 日:0, 一:1), 本年第几天
    int OI_GetCurDateTime(int *piYear,int *piMon,int *piMday,int *piHour,int *piMin,int *pisec,int *piWday,int *piYday) ;
	int OI_GetDateTime(const time_t * mytime,int *piYear,int *piMon,int *piMday,int *piHour,int *piMin,int *pisec,int *piWday,int *piYday);

    //取秒和微秒
    //struct timeval stTv ; gettimeofday(&stTv, NULL);
	//秒Seconds:stTv.tv_sec   微秒Microseconds:stTv.tv_usec  
	// 返回相差的微秒Microseconds 数
	int OI_DiffTime(struct timeval *pstStartTv, struct timeval *pstCurrTv);


#ifdef __cplusplus
}
#endif

#endif
