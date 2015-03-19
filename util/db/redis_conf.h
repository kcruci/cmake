// Copyright (C) 2014 - isky Inc.
// Author: davidluan
// CreateTime: 2014-02-13

#ifndef REDIS_CONF_H
#define REDIS_CONF_H

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>


#define ERR_WK_LOAD_CONF   -1001 //配置读取失败
#define ERR_WK_NO_IPPORT   -1002 //找不到对应uid的ipport
#define ERR_WK_WRONG_CONF  -1003 //xml格式错误


#define INIT_REDISCONF(srcfile) CSingleton<CRedisConf>::Instance()->LoadConf(srcfile)

#define QUERY_REDISDEST(uid,stPoint) CSingleton<CRedisConf>::Instance()->GetIpPort(uid,stPoint)

struct NETPOINT
{
	std::string ip;
	std::string port;
	std::string passwd;
};

//mod(100)后全部ip
typedef std::map<int, NETPOINT> REDISCONFMAP;
typedef std::vector<NETPOINT> VECNETPORT;
class CRedisConf
{
public:
	CRedisConf();
    //获取特定用户ip port
	int GetUserIpPort(unsigned long long uid, NETPOINT& stPoint);

    //获取队列全部 ip port
    int GetQueenAllIpPort(std::vector<NETPOINT>& allPoint);

	int GetAllUserIpPort(std::map<int, NETPOINT> & allPoint);

    //更新配置
    int LoadConf(const std::string& confFile);

private:

	REDISCONFMAP _mapUserRedis;//用户redis
	VECNETPORT _allModRedis;//bitmap redis
	int Init();
	int InitOne(const std::string& strFilePath);
    //获取指定uid对应的map映射idx
    int GetKeyIndex(long long uid);

	bool bInit;
	int InitRet;
    std::string _confFile;
};
#endif
