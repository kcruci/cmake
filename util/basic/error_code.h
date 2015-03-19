#ifndef _ERROR_CODE_H
#define _ERROR_CODE_H

#define E_OK 0
#define E_TIMER_OUT 0x9a
#define E_OTHER_ERROR 0x9b
#define E_CHECK_PKG_ERROR 0x9c
#define E_INVALID_PKG_TYPE 0x9d
#define E_INVALID_CMD 0x9e
#define E_RSPBODYLEN_ERROR 0x9f
#define E_ADDTIMER_ERROR 0xa0
#define E_GETTIMER_ERROR 0xa1
#define E_INVALID_PKG_CONTEXT 0xa2
#define E_REDIS_DISCONNECT    0xf1

#define E_PROTOCAL_VERSION_ERROR    0xfa
#define E_TOO_MANY_CLIENTS    	   0xfb


#endif
