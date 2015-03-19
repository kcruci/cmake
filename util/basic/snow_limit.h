#ifndef _SNOW_LIMIT_H
#define _SNOW_LIMIT_H

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#pragma pack(1)

typedef enum
{
	ret_ok = 0,
	ret_snow_slide = 1,
	ret_null_snow_node = -1,
	ret_bad_sys_time = -2,
	ret_bad_snow_flag = -3
}snow_ret_code;

typedef enum
{
	f_snow_read = 1,
	f_snow_write = 2
}snow_flag;

typedef struct
{
	uint32_t dwMaxReadLimit;
	uint32_t dwMaxWriteLimit;

	uint32_t dwCurReadNum;
	uint32_t dwCurWriteNum;

	time_t tCurReadTime;
	time_t tCurWriteTime;
}snow_node;

#pragma pack()

int check_snow(snow_node *pstSnowNode, snow_flag eFlag);
int check_snow2(snow_node *pstSnowNode, snow_flag eFlag);

#endif
