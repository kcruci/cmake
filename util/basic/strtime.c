#include <stdio.h>
#include <stdarg.h>

#include "strtime.h"

char *
OI_GetCurShortDateStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetShortDateStr(&mytime);
}

char *
OI_GetShortDateStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d%02d%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday);
    else
        snprintf(s, sizeof(s), "%04d%02d%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday);

    return s;
}

char *
OI_GetCurDateStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetDateStr(&mytime);
}

char *
OI_GetDateStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d-%02d-%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday);
    else
        snprintf(s, sizeof(s), "%04d-%02d-%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday);

    return s;
}

char *
OI_GetCurDateTimeStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetDateTimeStr(&mytime);
}

char *
OI_GetDateTimeStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d-%02d-%02d %02d:%02d:%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);
    else
        snprintf(s, sizeof(s), "%04d-%02d-%02d %02d:%02d:%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);

    return s;
}



int 
OI_GetDateTime(const time_t * mytime,int *piYear,int *piMon,int *piMday,int *piHour,int *piMin,int *pisec,int *piWday,int *piYday)
{
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
	     *piYear = curr.tm_year + 1900 ;
    else
	     *piYear = curr.tm_year + 2000 ;

	*piMon = curr.tm_mon + 1 ;
	*piMday = curr.tm_mday ;
	*piHour =  curr.tm_hour ;
	*piMin = curr.tm_min ;
	*pisec = curr.tm_sec ;
	*piWday = curr.tm_wday ;
	*piYday = curr.tm_yday ;
	return(0);
}

int OI_GetCurDateTime(int *piYear,int *piMon,int *piMday,int *piHour,int *piMin,int *pisec,int *piWday,int *piYday)
{
    time_t mytime = time(NULL);
	return OI_GetDateTime(&mytime,piYear,piMon,piMday,piHour,piMin,pisec,piWday,piYday);
}

char *
OI_GetCurShortDateTimeStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetShortDateTimeStr(&mytime);
}

char *
OI_GetShortDateTimeStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    if(curr.tm_year > 50)
        snprintf(s, sizeof(s), "%04d%02d%02d%02d%02d%02d", curr.tm_year + 1900, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);
    else
        snprintf(s, sizeof(s), "%04d%02d%02d%02d%02d%02d", curr.tm_year + 2000, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);

    return s;
}

char *
OI_GetCurTimeStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetTimeStr(&mytime);
}

char *
OI_GetTimeStr(const time_t * mytime)
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    snprintf(s, sizeof(s), "%02d:%02d:%02d", curr.tm_hour, curr.tm_min, curr.tm_sec);

    return s;
}

char *
OI_GetCurShortTimeStr(void)
{
    time_t mytime = time(NULL);

    return OI_GetShortTimeStr(&mytime);
}

char *
OI_GetShortTimeStr(const time_t * mytime)   //205301
{
    static char s[50];
    struct tm curr = *localtime(mytime);

    snprintf(s, sizeof(s), "%02d%02d%02d", curr.tm_hour, curr.tm_min, curr.tm_sec);

    return s;
}


int OI_DiffTime(struct timeval *pstStartTv, struct timeval *pstCurrTv)
{
	long sec, usec = 0;
	sec = pstCurrTv->tv_sec - pstStartTv->tv_sec;
	if ((usec = pstCurrTv->tv_usec - pstStartTv->tv_usec) < 0) {
		sec--;
		usec += 1000000;
	}
	usec += sec*1000000;
	return usec;
}
