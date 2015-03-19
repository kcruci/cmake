#include "basic/crc.h"
#include "ids_redis_helper.h"
#include <cstdlib>

unsigned int GetUidCrc32(unsigned long long udid)
{
    //need to check crc32(uid);
    std::string sAccUid = CTrans::ITOS(udid);
    unsigned int iCrcKeyUid= load_crc32(0,sAccUid.c_str(),sAccUid.size());
    return iCrcKeyUid;
}

 CRedis *get_redis_by_uid(CRedisConf & redisConf,unsigned long long udid)
{
	int iRet = 0;
	NETPOINT cIpPort;
	CRedis *pstRedis = NULL;

	unsigned int iCrcKeyUid = GetUidCrc32(udid);
	iRet = redisConf.GetUserIpPort(iCrcKeyUid, cIpPort);
    if(0 != iRet)
	{
		return NULL;
	}

	pstRedis = GET_REDIS_AUTH(cIpPort.ip, atoi(cIpPort.port.c_str()), cIpPort.passwd);

	return pstRedis;
}


