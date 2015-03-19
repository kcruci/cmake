#include "snow_limit.h"

int check_snow(snow_node *pstSnowNode, snow_flag eFlag)
{
	if(NULL == pstSnowNode)
		return ret_null_snow_node;
	
	time_t tTmpTime;
	time(&tTmpTime);

	if(f_snow_read == eFlag)
	{
		if(tTmpTime == pstSnowNode->tCurReadTime)
		{
			if(pstSnowNode->dwMaxReadLimit <= (pstSnowNode->dwCurReadNum+1))
			{
				return ret_snow_slide;
			}
			else
			{
				pstSnowNode->dwCurReadNum ++;
			}
		}
		else if(tTmpTime > pstSnowNode->tCurReadTime)
		{
			pstSnowNode->tCurReadTime = tTmpTime;
			pstSnowNode->dwCurReadNum = 0;
		}
		else
		{
			return ret_bad_sys_time;
		}
	}
	else if(f_snow_write == eFlag)
	{
		if(tTmpTime == pstSnowNode->tCurWriteTime)
		{
			if(pstSnowNode->dwMaxWriteLimit <= (pstSnowNode->dwCurWriteNum+1))
			{
				return ret_snow_slide;
			}
			else
			{
				pstSnowNode->dwCurWriteNum ++;
			}
		}
		else if(tTmpTime > pstSnowNode->tCurWriteTime)
		{
			pstSnowNode->tCurWriteTime = tTmpTime;
			pstSnowNode->dwCurWriteNum = 0;
		}
		else
		{
			return ret_bad_sys_time;
		}
	}
	else
	{
		return ret_bad_snow_flag;
	}
	
	return ret_ok;
}

int check_snow2(snow_node *pstSnowNode, snow_flag eFlag)
{
	if(NULL == pstSnowNode)
		return ret_null_snow_node;
	
	time_t tTmpTime;

	if(f_snow_read == eFlag)
	{
		pstSnowNode->dwCurReadNum ++;
		if(pstSnowNode->dwMaxReadLimit <= pstSnowNode->dwCurReadNum)
		{
			time(&tTmpTime);
			if(tTmpTime == pstSnowNode->tCurReadTime)
			{
				return ret_snow_slide;
			}
			else if(tTmpTime > pstSnowNode->tCurReadTime)
			{
				pstSnowNode->tCurReadTime = tTmpTime;
				pstSnowNode->dwCurReadNum = 0;
			}
			else
			{
				return ret_bad_sys_time;
			}
		}
	}
	else if(f_snow_write == eFlag)
	{
		pstSnowNode->dwCurWriteNum ++;
		if(pstSnowNode->dwMaxWriteLimit <= pstSnowNode->dwCurWriteNum)
		{
			time(&tTmpTime);
			if(tTmpTime == pstSnowNode->tCurWriteTime)
			{
				return ret_snow_slide;
			}

			else if(tTmpTime > pstSnowNode->tCurWriteTime)
			{
				pstSnowNode->tCurWriteTime = tTmpTime;
				pstSnowNode->dwCurWriteNum = 0;
			}
			else
			{
				return ret_bad_sys_time;
			}
		}
	}
	else
	{
		return ret_bad_snow_flag;
	}
	
	return ret_ok;
}

