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

#define CHANGE_INFO_SIZE 6											//�������
#define CFG_CENTER_DB_NAME "db_cfg_center"
#define TABLE_NAME_PROXY_CFG "t_proxy_cfg"
#define TABLE_NAME_PROXY_CMD_CFG "t_proxy_cmd_cfg"
#define TABLE_NAME_PROXY_CMD_ROUTE_CFG "t_proxy_cmd_route_cfg"
#define TABLE_NAME_LOGIC_SVR_CFG "t_logic_svr_cfg"
#define TABLE_NAME_LOGIC_SVR_CMD_CFG "t_logic_svr_cmd_cfg"
#define TABLE_NAME_LOGIC_SVR_CMD_ROUTE_CFG "t_logic_svr_cmd_route_cfg"
#define TABLE_NAME_CHANGE_INFO "t_change_info"

#define MAX_SHM_KEY_NUM (32)
#define MAX_CMD_CFG_NUM (256)										//ÿ��proxy������������������
#define MAX_LOGIC_SVR_NUM_PER_CMD (32)								//ÿ�������������n���߼���svr������

#define MAX_CFG_AGENT_REGISTER_NUM (102)//frank modify to 102 from 1024							//����cfg_agent����
#define MAX_GAME_BASE_SVR_PROXY_CFG_NUM (32)						//proxy��������
#define MAX_GAME_BASE_SVR_PROXY_CMD_CFG_NUM (32)					//������ָ�proxy����������ͬ
#define MAX_GAME_BASE_SVR_PROXY_CMD_ROUTE_CFG_NUM (256*32)			//proxy������ÿ��proxy�������������޵ĳ˻�
#define MAX_GAME_BASE_SVR_LOGIC_SVR_NUM (102)						//�߼���svr��������
#define MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_CFG_NUM (102)				//�߼���svr�������������������
#define MAX_GAME_BASE_SVR_LOGIC_SVR_CMD_ROUTE_CFG_NUM (102*256)	//�߼���svr������������������

#define MAX_GAME_BASE_SVR_SHM_SIZE (100000000)						//���ù����ڴ�����ֵ

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
}game_base_svr_table_id;						//��id

typedef enum
{
	e_t_read = 1,
	e_t_write = 2,
	e_t_update = 3,
	e_t_sync = 4,
	e_t_exception = 5
}game_base_svr_route_cfg_type;				//�������Ĺ����ڴ������ݶ�д��־λ

typedef enum
{
	HEART_BEAT_OK = 0x0,							//
	HEART_BEAT_ERR = 0x21,							//
	HEART_BEAT_UPDATE = 0x31						//
}HEART_BEAT_TYPE;								//����״̬

typedef enum
{
	e_unworking = 0,								//������������
	e_working = 1									//����������
}working_state;									//����svr�Ĺ���״̬������״̬

typedef enum
{
	e_t_cfg_agent_using = 1,						//����������
	e_t_cfg_agent_not_use = 2						//�쳣��û������
}cfg_agent_use_less;							//cfg_agent��ʹ��״̬�����ֵ���������������ע����Ϣ����

typedef struct
{
	uint16_t wZoneId;				//����id
	uint32_t dwSvrId;				//proxy ������id
	uint32_t dwIp;					//proxy ������ip
	uint16_t wPort;					//proxy �����������˿�
	uint8_t cWorkingState;			//proxy ����������״̬��0��ʾ��������������������ʾ��������
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cfg;

typedef struct
{
	uint32_t dwSvrId;							//proxy ������id
	uint32_t dwCmdCfgNum;						//proxy ����·�ɵ������ָ���
	uint32_t dwCmdIdList[MAX_CMD_CFG_NUM];		//proxy ����·�ɵ������ֵ�id�б�
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cmd_cfg;

typedef struct
{
	uint16_t wZoneId;										//����id
	uint32_t dwCmdId;										//������id
	uint32_t dwLogicSvrNum;									//�����ֿ���·�ɵ����ε�inner_gateway�ĸ���
	uint32_t dwLogicSvrIdList[MAX_LOGIC_SVR_NUM_PER_CMD];	//inner_gateway ��������id�б�
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_proxy_cmd_route_cfg;

typedef struct
{
	uint16_t wZoneId;				//����id
	uint32_t dwSvrId;				//logic_svr ������id
	uint32_t dwIp;					//logic_svr ������ip
	uint16_t wPort;					//logic_svr �����������˿�
	uint8_t cWorkingState;			//logic_svr ����������״̬��0��ʾ��������������������ʾ��������
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_logic_svr_cfg;

typedef struct
{
	uint32_t dwSvrId;							//logic_svr ������id
	uint32_t dwCmdCfgNum;						//logic_svr ����·�ɵ������ָ���
	uint32_t dwCmdIdList[MAX_CMD_CFG_NUM];		//logic_svr ����·�ɵ������ֵ�id�б�
	uint64_t ddwResv1;
	uint64_t ddwResv2;
}game_base_svr_logic_svr_cmd_cfg;

typedef struct
{
	uint16_t wZoneId;							//����id
	uint32_t dwSvrId;							//logic_svr������id
	uint32_t dwCmdId;							//������id
	uint32_t dwShmKeyNum;						//������������߼���ҵ���ӽ���ӵ�еĹ����ڴ���Ŀ
	uint32_t dwShmKeyList[MAX_SHM_KEY_NUM];		//�����ڴ��key_list
	uint32_t dwMaxShmSize;						//ÿ�������ڴ�Ĵ�С��Ĭ��100000
	uint64_t ddwResv1;							//
	uint64_t ddwResv2;							//
}game_base_svr_logic_svr_cmd_route_cfg;

typedef struct
{
	uint8_t cTableName[MAX_TABLE_NAME_LEN];				//����
	volatile uint64_t ddwChangeTime;					//����������ʱ��
	volatile uint8_t cNeedUpdate;						//�Ƿ���Ҫ���±�־
	uint64_t ddwResv;
}game_base_svr_change_info;

typedef struct
{
	volatile uint32_t *pRWFlag;							//��д��־λ

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


//cfg_agent���������������ע����Ϣ
typedef struct
{
	uint32_t dwAgentId;								//cfg_agent��id
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
	��ȡ�����ڴ����game_base_svr_route_cfg �Ĵ�С
*/
int CFG_GetShmSize();


/*
	��ʼ�������ڴ����game_base_svr_route_cfg
	����ֵ:
			0 -- �ɹ�
			-1 -- ָ��pstCfgShm Ϊ��
			-2 -- ��ȡ�Ĺ����ڴ�����С����
			-3 -- �ҹ����ڴ����
			-4 -- ��ʼ�������ڴ�ָ�����
*/
int CFG_InitGameBaseSvrShm(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm);


int CFG_InitGameBaseSvrShmEx(uint32_t dwShmKey, game_base_svr_route_cfg *pstCfgShm);
/*
	���ĳ�ű��Ƿ��и���
	0 -- û�и���
	1 -- �и���
	-1 -- ��������
*/
int CFG_CheckUpdate(game_base_svr_route_cfg *pstCfgShm, game_base_svr_table_id TableId, uint64_t *pddwTime);

/*
	���������������ȡһ��inner_gateway �ڵ���Ϣ
	0 -- �ɹ�
	-1 -- ��������
	-2 -- û�ж�Ӧ��inner_gateway�ڵ�
*/
int CFG_GetOneLogicSvrNode(game_base_svr_route_cfg *pstCfgShm, 
		uint16_t wZoneId, uint32_t dwCmdId, 
		uint32_t dwLocalIp, game_base_svr_logic_svr_cfg *pstLogicSvrNode);

int CFG_GetLogicSvrCfg(game_base_svr_route_cfg *pstCfgShm, uint32_t dwInnerGWId, game_base_svr_logic_svr_cfg *pstInnerGWNode);


int CFG_GetLocalChangeInfoByTableName(game_base_svr_route_cfg *pstCfgShm, game_base_svr_change_info **pstChangeInfo, char *cTableName);

int CFG_GetLocalChangeInfoByTableId(game_base_svr_route_cfg *pstCfgShm, int iTableType, game_base_svr_change_info **ppChangeInfo);

#endif
