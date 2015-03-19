#ifndef _LIB_NET_CONST_H_
#define _LIB_NET_CONST_H_

#include <sys/types.h>
#include <stdio.h>

#include "common.h"

enum
{
    // indepence of service
    MAXEMLINE = 1000,
    MAXEMLEN = 500,
    MAXELINE = 512,
    MAXPACKLEN = 5120,
    MAXIPSIZE = 30,
    MAXPLINE = 1024,
    MAXLINE = 4096,
    DMAXLINE = 10240,

    MAXSENDBUF = 40 * 1020,
    MAXRECVBUF = 60 * 1020,

};

/* /////////////////////////////////////////////////////////// */
enum
{
    MEM_SEND = 1,
    MEM_RECV = 0,

    CS_NOT_CONNECT = 0,
    CS_CONNECT = 1,
    CS_LOGIN = 2,
    CS_LISTEN = 3,
    CS_CONNECTTING = 4,
};


/* /////////////////////////////////////////////////////////////////////////////////// */
/*
#if defined(PROTOCOL_VERSION) && (PROTOCOL_VERSION>=10)
#define ESCAPE_STRING(dblink,outstr,instr,instrlenght)  mysql_real_escape_string(dblink,outstr,instr,instrlenght)
#else
#define ESCAPE_STRING(dblink,outstr,instr,instrlenght)  mysql_escape_string(outstr,instr,instrlenght)
#endif
*/
// mysql 3.23.58下，strange bug,如果MYSQL未连接，则调用real_escape_string时会core dump
#define ESCAPE_STRING(dblink,outstr,instr,instrlenght)  mysql_escape_string(outstr,instr,instrlenght)

#define PKG_GETWORD(type,buffer,member,value) do{ \
	type *pkg = (type*)buffer; \
	value = ntohs(Chars_2_Word((unsigned char*)pkg->member)); \
}while(0)
#define PKG_GETDWORD(type,buffer,member,value) do{ \
	type *pkg = (type*)buffer; \
	value = ntohl(Chars_2_DW((unsigned char*)pkg->member)); \
}while(0)
#define PKG_GETFIXSTRING(type,buffer,member,value) do{ \
        type *pkg = (type*)buffer; \
        snprintf(value,sizeof(value),"%s",pkg->member); \
}while(0)

//简单的 snprintf的封装，需要注意 buf不能是指针（即使是指向数组的指针也不行，
//传递给函数的数组参数也不行），否则 sizeof(buf)就会判断错误
#define SET_FIX_STR(buf, val) do{ \
	snprintf(buf,sizeof(buf), "%s", val);\
}while(0)

#define SET_FIX_STR_INT(buf, val) snprintf(buf,sizeof(buf), "%d", val)

#define PKG_SETWORD(type,buffer,member,value) do{ \
	type *pkg = (type*)buffer; \
	Word_2_Chars((unsigned char*)pkg->member, htons(value)); \
}while(0)

#define PKG_SETDWORD(type,buffer,member,value) do{ \
	type *pkg = (type*)buffer; \
	DW_2_Chars((unsigned char*)pkg->member, htonl(value)); \
}while(0)

#define PKG_SETFIXSTRING(type,buffer,member,value) do{ \
	type *pkg = (type*)buffer; \
	assert(sizeof(pkg->member)>=strlen(value)); \
	strncpy(pkg->member,value,sizeof(pkg->member)); \
}while(0)

#define TO_STRING(arg) #arg
#define TMARK_BEGIN(mark) "<%$"mark##TO_STRING(_begin)"$%>"
#define TMARK_END(mark) "<%$"mark##TO_STRING(_end)"$%>"

#endif
