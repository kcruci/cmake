#ifndef _IDS_REDIS_HELPER_H
#define _IDS_REDIS_HELPER_H
#include "basic/trans.h"
#include "redis_pool.h"
#include "redis_conf.h"
#include "redis_wrapper.h"

CRedis *get_redis_by_uid(CRedisConf & redisConf, unsigned long long udid);

#endif //_IDS_REDIS_HELPER_H
	

