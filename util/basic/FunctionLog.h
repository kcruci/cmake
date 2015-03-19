#ifndef __FUNCTION_LOG_H
#define __FUNCTION_LOG_H

#include "game_base_svr_struct.h"
#include "endecode_base.h"


#define LOG_FUNCTION_START CFunctionLog __var_##__FUNCTION__(__PRETTY_FUNCTION__); //__func__

class CFunctionLog
{
    public:
        CFunctionLog(const char * pszFuncTionName)
            {
                m_pszFuncTionName = pszFuncTionName;
                //printf("Start================%s=============\n", m_pszFuncTionName);
				LOG_INFO("Start================%s=============", m_pszFuncTionName);
            }
        ~CFunctionLog()
            {
                //printf("Finish================%s=============\n", m_pszFuncTionName);
				LOG_INFO("Finish================%s=============", m_pszFuncTionName);
            }
    private:
    CFunctionLog();
    CFunctionLog(const CFunctionLog&);
    const char * m_pszFuncTionName;
};

static void MsgPerformanceStatistic(uint32_t dwSeqId, uint64_t STATISTIC_COUNT = 100000)
{
    //#ifdef _DEBUG
#define USDIFF(new, old) (1000000 * (unsigned long long)((new).tv_sec - (old).tv_sec) \
                       + (new).tv_usec - (old).tv_usec)
       
          static uint64_t s_totalMsgCount = 0;    
           if(s_totalMsgCount > 0 && s_totalMsgCount % STATISTIC_COUNT == 0)
           {       
               static struct timeval s_tMsgTimeFrom;
               if(s_totalMsgCount % (STATISTIC_COUNT*2) == 0)
               {      
                   static struct timeval tCurTime;
                   gettimeofday(&tCurTime, NULL);
           
                   uint64_t passedUs = USDIFF(tCurTime, s_tMsgTimeFrom);
                   double fSeconds = double(passedUs) / 1000000;
                   double fMsgNumPerSecond = STATISTIC_COUNT / fSeconds;
                   LOG_VIP("s_totalMsgCount=%llu, head.dwSeqId=%u, GetCurTimerNode()=%u, GetMaxTimerNode()=%u fSeconds=%lf fMsgNumPerSecond=%lf", s_totalMsgCount, dwSeqId, GetCurTimerNode(), GetMaxTimerNode(), fSeconds, fMsgNumPerSecond);
               }
    
               gettimeofday(&s_tMsgTimeFrom, NULL);
           }
     
           s_totalMsgCount++;
//#endif
}

#define LOG_DEBUG_UINT_VAR(var) LOG_DEBUG(#var"=%u", (uint32_t)(var))
#define LOG_INFO_UINT_VAR(var) LOG_INFO(#var"=%u", (uint32_t)(var))
#define LOG_INFO_UINT64_VAR(var) LOG_INFO(#var"=%llu", (uint64_t)(var))

static void PrintPkgHead(const pkg_head  &stPkgHead)
{
	return;
	LOG_DEBUG_UINT_VAR(sizeof(pkg_head));
	LOG_DEBUG_UINT_VAR(stPkgHead.dwPkgLen);
	LOG_DEBUG_UINT_VAR(stPkgHead.dwHeadLen);
	LOG_DEBUG_UINT_VAR(stPkgHead.cPkgType);		//1--req 2--rsp
    if(e_pkg_rsp == stPkgHead.cPkgType)
    {
        LOG_DEBUG("cPkgType=e_pkg_rsp");
    }
    else if(e_pkg_req == stPkgHead.cPkgType)
        {
            LOG_DEBUG("cPkgType=e_pkg_req");
        }
    else if(e_pkg_push == stPkgHead.cPkgType)
        {
            LOG_DEBUG("cPkgType=e_pkg_push");
        }
    else if(e_pkg_gm_kick == stPkgHead.cPkgType)
        {
            LOG_DEBUG("cPkgType=e_pkg_gm_kick");
        }
    else if(e_pkg_gm_push == stPkgHead.cPkgType)
        {
            LOG_DEBUG("cPkgType=e_pkg_gm_push");
        }
    else 
        {
            LOG_ERROR("error cPkgType unknow!!!!");
        }
	
	LOG_DEBUG_UINT_VAR(stPkgHead.cCmdStatus);		//ÊÇ·ñÓÐ×´Ì¬
	LOG_DEBUG_UINT_VAR(stPkgHead.dwCmdId);
	LOG_DEBUG_UINT_VAR(stPkgHead.dwSeqId);
	LOG_INFO_UINT64_VAR(stPkgHead.ddwUid);
	LOG_DEBUG_UINT_VAR(stPkgHead.cPlatId);
	LOG_DEBUG_UINT_VAR(stPkgHead.wZoneId);
	LOG_DEBUG_UINT_VAR(stPkgHead.dwSrcIp);
	LOG_DEBUG_UINT_VAR(stPkgHead.wSrcPort);
	LOG_DEBUG_UINT_VAR(stPkgHead.dwDstIp);
	LOG_DEBUG_UINT_VAR(stPkgHead.wDstPort);
	LOG_DEBUG_UINT_VAR(stPkgHead.dwResult);
	LOG_DEBUG_UINT_VAR(stPkgHead.wTlvNum);
	for(int i=0;i < stPkgHead.wTlvNum && i < MAX_TLV_NUM; i++)
	{
		LOG_DEBUG_UINT_VAR(stPkgHead.stTlv[i].wTypeId);
		LOG_DEBUG_UINT_VAR(stPkgHead.stTlv[i].wLen);
		LOG_DEBUG_UINT_VAR(*((uint32_t*)(&(stPkgHead.stTlv[i].cValue[0]))));
		LOG_DEBUG_UINT_VAR(ntohl(*((uint32_t*)(&(stPkgHead.stTlv[i].cValue[0])))));
	}
}
#endif
