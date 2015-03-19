#ifndef _GAME_BASE_CFG_API_H
#define _GAME_BASE_CFG_API_H

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include "ids_basic_shm.h"

#define START 0xa
#define END 0x3

#define MAX_TABLE_NAME_LEN 64
#define IP_LEN 16
#define UPDATE_END 1987
#define SUB_HEARDBIT 0x10				//
#define SUB_UPREQ 0x20
#define SUB_FORCE_RELOAD 0x30

#define UPDATE_END_TYPE_AGENT_INFO 200

#define CHANGE_INFO_SIZE 6											//表的总数
#define CFG_CENTER_DB_NAME "db_cfg_center"
#define TABLE_NAME_PROXY_CFG "t_proxy_cfg"
#define TABLE_NAME_PROXY_CMD_CFG "t_proxy_cmd_cfg"
#define TABLE_NAME_PROXY_CMD_ROUTE_CFG "t_proxy_cmd_route_cfg"
#define TABLE_NAME_LOGIC_SVR_CFG "t_logic_svr_cfg"
#define TABLE_NAME_LOGIC_SVR_CMD_CFG "t_logic_svr_cmd_cfg"
#define TABLE_NAME_LOGIC_SVR_CMD_ROUTE_CFG "t_logic_svr_cmd_route_cfg"
#define TABLE_NAME_CHANGE_INFO "t_change_info"

#define MAX_SHM_KEY_NUM (32)
#define MAX_CMD_CFG_NUM (256)										//每个proxy负责命令字总数上限
#define MAX_LOGIC_SVR_NUM_PER_CMD (32)								//每个命令字最多由n个逻辑层svr来处理

#define MAX_CFG_AGENT_REGISTER_NUM (102)//frank modify to 102 from 1024							//最多的cfg_agent总数
#define MAX_GAME_BASE_SVR_PROXY_CFG_NUM (32)						//proxy总数上限
#define MAX_GAME_BASE_SVR_PROXY_CMD_CFG_NUM (32)					//这个数字跟proxy个数上线相同
#define MAX_GAME_BASE_SVR_PROXY_CMD_ROUTE_CFG_NUM (256*32)			//proxy总数和每个proxy负责命令字上限的乘积
#define MAX_GAME_BASE_SVR_LOGIC_SVR_NUM (102)						//逻辑层svr总数上限
#define MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_CFG_NUM (102)				//逻辑层svr负责的命令字总数上限
#define MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_ROUTE_CFG_NUM (102*256)	//逻辑层svr上面命令字行数上限

#define MAX_GAME_BASE_SVR_SHM_SIZE (100000000)						//配置共享内存的最大值

#define ERRSTR strerror(errno)

#pragma pack(1)
typedef enum
{
	e_t_proxy_cfg = 1,								//
	e_t_proxy_cmd_cfg = 2,							//
	e_t_proxy_cmd_route_cfg = 3,					//
	e_t_logic_svr_cfg = 4,							//
	e_t_logic_svr_cmd_cfg = 5,						//
	e_t_logic_svr_cmd_route_cfg = 6,				//
	e_t_change_info = 7,							//
	e_t_max_id,										
}game_base_svr_table_id;						//表id

typedef enum
{
	e_t_read = 1,
	e_t_write = 2,
	e_t_update = 3,
	e_t_sync = 4,
	e_t_exception = 5
}game_base_svr_route_cfg_type;				//配置中心共享内存中数据读写标志位

typedef enum
{
	HEART_BEAT_OK = 0x0,							//
	HEART_BEAT_ERR = 0x21,							//
	HEART_BEAT_UPDATE = 0x31						//
}HEART_BEAT_TYPE;								//心跳状态

typedef enum
{
	e_unworking = 0,								//不能正常工作
	e_working = 1									//正常工作中
}working_state;									//各个svr的工作状态，两种状态

typedef enum
{
	e_t_cfg_agent_using = 1,						//正常在用中
	e_t_cfg_agent_not_use = 2						//异常，没有在用
}cfg_agent_use_less;							//cfg_agent的使用状态，这个值在配置中心里面的注册信息里面

typedef struct
{
	uint16_t wZoneId;				//分区id
	uint32_t dwSvrId;				//proxy 服务器id
	uint32_t dwIp;					//proxy 服务器ip
	uint16_t wPort;					//proxy 服务器监听端口
	uint8_t cWorkingState;			//proxy 服务器工作状态，0表示不能正常工作，其他表示正常工作
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cfg;

typedef struct
{
	uint32_t dwSvrId;							//proxy 服务器id
	uint32_t dwCmdCfgNum;						//proxy 负责路由的命令字个数
	uint32_t dwCmdIdList[MAX_CMD_CFG_NUM];		//proxy 负责路由的命令字的id列表
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cmd_cfg;

typedef struct
{
	uint16_t wZoneId;										//分区id
	uint32_t dwCmdId;										//命令字id
	uint32_t dwLogicSvrNum;									//命令字可以路由到下游的inner_gateway的个数
	uint32_t dwLogicSvrIdList[MAX_LOGIC_SVR_NUM_PER_CMD];	//inner_gateway 服务器的id列表
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cmd_route_cfg;

typedef struct
{
	uint16_t wZoneId;				//分区id
	uint32_t dwSvrId;				//logic_svr 服务器id
	uint32_t dwIp;					//logic_svr 服务器ip
	uint16_t wPort;					//logic_svr 服务器监听端口
	uint8_t cWorkingState;			//logic_svr 服务器工作状态，0表示不能正常工作，其他表示正常工作
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_logic_svr_cfg;

typedef struct
{
	uint32_t dwSvrId;							//logic_svr 服务器id
	uint32_t dwCmdCfgNum;						//logic_svr 负责路由的命令字个数
	uint32_t dwCmdIdList[MAX_CMD_CFG_NUM];		//logic_svr 负责路由的命令字的id列表
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_logic_svr_cmd_cfg;

typedef struct
{
	uint16_t wZoneId;							//分区id
	uint32_t dwSvrId;							//logic_svr服务器id
	uint32_t dwCmdId;							//命令字id
	uint32_t dwShmKeyNum;						//负责此命令字逻辑的业务子进程拥有的共享内存数目
	uint32_t dwShmKeyList[MAX_SHM_KEY_NUM];		//共享内存的key_list
	uint32_t dwMaxShmSize;						//每个共享内存的大小，默认100000
	uint64_t ddwResv1;							//
	uint64_t ddwResv2;							//
}game_base_svr_logic_svr_cmd_route_cfg;

typedef struct
{
	uint8_t cTableName[MAX_TABLE_NAME_LEN];				//表名
	volatile uint64_t ddwChangeTime;					//表的最近更新时间
	volatile uint8_t cNeedUpdate;						//是否需要更新标志
	uint64_t ddwResv;
}game_base_svr_change_info;

typedef struct
{
	volatile uint32_t *pRWFlag;							//读写标志位

	uint32_t *pdwGameBaseSvrProxyNum;
	game_base_svr_proxy_cfg *pstProxyCfgList;

	uint32_t *pdwGameBaseSvrProxyCmdCfgNum;
	game_base_svr_proxy_cmd_cfg *pstProxyCmdCfgList;

	uint32_t *pdwGameBaseSvrProxyCmdRouteCfgNum;
	game_base_svr_proxy_cmd_route_cfg *pstProxyCmdRouteCfgList;

	uint32_t *pdwGameBaseSvrLogicSvrCfgNum;
	game_base_svr_logic_svr_cfg *pstLogicSvrCfgList;

	uint32_t *pdwGameBaseSvrLogicSvrCmdCfgNum;
	game_base_svr_logic_svr_cmd_cfg *pstLogicSvrCmdCfgList;

	uint32_t *pdwGameBaseSvrLogicSvrCmdRouteCfgNum;
	game_base_svr_logic_svr_cmd_route_cfg *pstLogicSvrCmdRouteCfgList;

	game_base_svr_change_info *pstChangeInfo;
}game_base_svr_route_cfg;


//cfg_agent在配置中心里面的注册信息
typedef struct
{
	uint32_t dwAgentId;								//cfg_agent的id
	uint32_t dwAgentType;

	uint8_t cAgentIp[IP_LEN];
	uint16_t wAgentUdpPort;

	time_t tLastHeartBeatTime;
	uint8_t cAgentStatus;
	uint8_t cAgentNodeStatus;

	time_t tLastUpdateTime;
	uint8_t cAgentVersion;
	uint8_t cUseLess;
	uint8_t cNoteStatus;

	uint64_t ddwResv;
}game_base_svr_cfg_agent_register_node;

typedef struct
{
	uint32_t *pdwCfgAgentNum;
	game_base_svr_cfg_agent_register_node *pstCfgAgentNodeList;
}game_base_svr_cfg_agent_register_shm;
#pragma pack()

#define RET_LINE() {return -__LINE__;}
#define RET_TRUE_LINE(x) do{ if(x) return -__LINE__; }while(0)
#define CHECKNOTNULL(node) \
	if(node == NULL) \
	{\
		return  -1;\
	}\

/*
	获取共享内存对象game_base_svr_route_cfg 的大小
*/
int CFG_GetShmSize();


/*
	初始化共享内存对象game_base_svr_route_cfg
	返回值:
			0 -- 成功
			-1 -- 指针pstCfgShm 为空
			-2 -- 获取的共享内存对象大小有误
			-3 -- 挂共享内存错误
			-4 -- 初始化共享内存指针错误
*/
int CFG_InitGameBaseSvrShm(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm);


int CFG_InitGameBaseSvrShmEx(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm);
/*
	检查某张表是否有更新
	0 -- 没有更新
	1 -- 有更新
	-1 -- 参数错误
*/
int CFG_CheckUpdate(game_base_svr_route_cfg *pstCfgShm, game_base_svr_table_id TableId, uint64_t *pddwTime);

/*
	根据命令字随机获取一个inner_gateway 节点信息
	0 -- 成功
	-1 -- 参数错误
	-2 -- 没有对应的inner_gateway节点
*/
int CFG_GetOneLogicSvrNode(game_base_svr_route_cfg *pstCfgShm, 
		uint16_t wZoneId, uint32_t dwCmdId, 
		uint32_t dwLocalIp, game_base_svr_logic_svr_cfg *pstLogicSvrNode);

int CFG_GetLogicSvrCfg(game_base_svr_route_cfg *pstCfgShm, uint32_t dwInnerGWId, game_base_svr_logic_svr_cfg *pstInnerGWNode);


int CFG_GetLocalChangeInfoByTableName(game_base_svr_route_cfg *pstCfgShm, game_base_svr_change_info **pstChangeInfo, char *cTableName);

int CFG_GetLocalChangeInfoByTableId(game_base_svr_route_cfg *pstCfgShm, int iTableType, game_base_svr_change_info **ppChangeInfo);

#endif
