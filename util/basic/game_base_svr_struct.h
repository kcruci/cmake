#ifndef _GAME_BASE_SVR_STRUCT_H
#define _GAME_BASE_SVR_STRUCT_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "basic/MyNetworkByteOrder.h"

#define TLV_VALUE_MAX_SIZE (8)
#define MAX_TLV_NUM (4)
#define OPENID_MAX_LEN  (128)

#pragma pack(1)
typedef enum
{
	e_pkg_req = 1,
	e_pkg_rsp = 2,
	e_pkg_push = 3,
	e_pkg_gm_kick = 4,
	e_pkg_gm_push = 5,
}pkg_type;

typedef enum
{
	e_plat_ios = 0,
	e_plat_and = 1
}plat_type;

typedef enum
{
	e_cmd_status_no_stat = 0,
	e_cmd_status_stat = 1,
	e_cmd_status_route_to_cmd_svr_key = 2 //added by Frank.cheng at 2014.01.13
}cmd_status;

typedef enum
{
	e_src_cmd = 0,							//给cmd_svr用的
	e_src_shm_key = 1,						//给cmd_svr用的
	e_src_logicsvr_shm_key = 2,			//给logic_svr用的
	e_src_route_to_cmd_svr_key = 3		//added by Frank.cheng at 2014.01.13
}pkg_head_tlv_type;

/*
Tlv = wTypeId+wLen+cValue[256]
*/
typedef struct
{
	uint16_t wTypeId;
	uint16_t wLen;
	uint8_t cValue[TLV_VALUE_MAX_SIZE];
}Tlv;

/*
pkgHead = dwPkgLen+dwHeadLen+cPkgType+cCmdStatus+dwCmdId+cPlatId+dwSeqId+ddwUid+
wZoneId+dwSrcIp+wSrcPort+dwDstIp+wDstPort+dwResult+wTlvNum+stTlv[64]
*/
typedef struct
{
	uint32_t dwPkgLen;
	uint32_t dwHeadLen;
	uint8_t cPkgType;		//1--req 2--rsp
	uint8_t cCmdStatus;		//是否有状态
	uint32_t dwCmdId;
	uint32_t dwSeqId;
	uint64_t ddwUid;
	uint8_t cPlatId;
	uint16_t wZoneId;
	uint32_t dwSrcIp;
	uint16_t wSrcPort;
	uint32_t dwDstIp;
	uint16_t wDstPort;
	uint32_t dwResult;
	uint16_t wTlvNum;
	Tlv stTlv[MAX_TLV_NUM];
}pkg_head;
#pragma pack()

#define RET_LINE() {return -__LINE__;}
#define RET_TRUE_LINE(x) do{ if(x) return -__LINE__; }while(0)
#define CHECKNOTNULL(node) \
	if(node == NULL) \
	{\
		return  -1;\
	}\

int DecodePkgHead(pkg_head *pstPkgHead, char *pPkg, int iPkgLen);
int EncodePkgHead(pkg_head *pstPkgHead, char *pPkg, int *iPkgLen);

int GetPkgHeadLen(pkg_head *pstPkgHead);
int GetLogicSvrMsgBusShmKey(pkg_head *pstPkgHead, uint32_t *pdwMsgBusShmKey);

#endif
