#include <stdarg.h>
#include "cfg_api.h" //for RET_LINE only
#include "get_config.h"

using namespace std;

void atoi_num_list(const char* s, char splitchar,std::vector<unsigned>& vecOut)
{  
    vecOut.clear();

    int length = strlen(s);

    int start=0;  
    for(int i=0;i<length;i++)  
    {  
        if(s[i] == splitchar && i == 0)
        {  
            start += 1;  
        }  
        else if(s[i] == splitchar)  
        {  
            std::string strTmp;
            strTmp.assign(s + start,i - start);
            vecOut.push_back(atoi(strTmp.c_str()));  
            start = i+1;  
        }  
        else if(i == length-1)//µ½´ïÎ²²¿  
        {  
            std::string strTmp;
            strTmp.assign(s + start,i+1 - start);
            vecOut.push_back(atoi(strTmp.c_str()));   
        }  
    }  
}

int IDS_GetCommonConfig(TiXmlElement *element, IDS_ConfigCommon & configOut)
{
    if(NULL == element)
    {
        RET_LINE(); 
    }

    configOut.vecTimerNodeNum.clear();
	configOut.vecTimerNodeNumForTest.clear();

    RET_TRUE_LINE(GetOneNodeConfig(element, "test", DT_UINT32_T,
					&(configOut.iTest), sizeof(configOut.iTest), 0));

    printf("configOut.iTest=%d\n", configOut.iTest);
    
    RET_TRUE_LINE(GetOneNodeConfig(element, "TIMER_CHECK_INTERVAL_US", DT_UINT32_T,
                &(configOut.dwTimerCheckIntervalUs), sizeof(configOut.dwTimerCheckIntervalUs), 10*1000));
    char szTimerNodeNumBuf[2048];
    memset(szTimerNodeNumBuf, 0x00, sizeof(szTimerNodeNumBuf));
    RET_TRUE_LINE(GetOneNodeConfig(element, "TIMER_NODE_NUM_IN_LIST", DT_STRING,
                szTimerNodeNumBuf, sizeof(szTimerNodeNumBuf), "200000,200000")); 

    atoi_num_list(szTimerNodeNumBuf, ',', configOut.vecTimerNodeNum); 

    memset(szTimerNodeNumBuf, 0x00, sizeof(szTimerNodeNumBuf));
    RET_TRUE_LINE(GetOneNodeConfig(element, "TIMER_NODE_NUM_IN_LIST_FOR_TEST", DT_STRING,
                szTimerNodeNumBuf, sizeof(szTimerNodeNumBuf), "200000,200000")); 

    atoi_num_list(szTimerNodeNumBuf, ',', configOut.vecTimerNodeNumForTest); 

    if(configOut.vecTimerNodeNum.empty() || configOut.vecTimerNodeNumForTest.empty())
    {
        RET_LINE();
    }

    //Frank.cheng Temp test only
    //TODO:  remove  this
    //configOut.iTest = 1;
    //configOut.vecTimerNodeNumForTest.clear();
    //configOut.vecTimerNodeNumForTest.push_back(200000);
    //configOut.vecTimerNodeNumForTest.push_back(200000);
    
    return 0;
}

int GetOneNodeConfig(TiXmlElement *dbelement, const char *strNodeName, basic_data_type eDataType, 
	void *pValue, size_t iValueBufLen, ...)
{
	if(NULL==dbelement || NULL==pValue)
		return -1;

	std::string strValue;
	strValue.clear();

	uint8_t *pString = NULL;
	uint8_t cValue = 0;
	uint16_t wValue = 0;
	uint32_t dwValue = 0;
	uint64_t ddwValue = 0;

	va_list ap;
	va_start(ap, iValueBufLen);
	
	if(NULL == dbelement->Attribute(strNodeName))
	{
		switch(eDataType)
		{
			case DT_UINT8_T:
				if(sizeof(uint8_t) > iValueBufLen)
					return -2;
				
				cValue = va_arg(ap, uint8_t);
				memcpy(pValue, &cValue, iValueBufLen);
				break;
			case DT_UINT16_T:
				if(sizeof(uint16_t) > iValueBufLen)
					return -2;
				
				wValue = va_arg(ap, uint16_t);
				memcpy(pValue, &wValue, iValueBufLen);
				break;
			case DT_UINT32_T:
				if(sizeof(uint32_t) > iValueBufLen)
					return -2;
				
				dwValue = va_arg(ap, uint32_t);
				memcpy(pValue, &dwValue, iValueBufLen);
				break;
			case DT_UINT64_T:
				if(sizeof(uint64_t) > iValueBufLen)
					return -2;
				
				ddwValue = va_arg(ap, uint64_t);
				memcpy(pValue, &ddwValue, iValueBufLen);
			case DT_STRING:
				pString = va_arg(ap, uint8_t *);
				memcpy(pValue, pString, iValueBufLen);
				break;
			default:
				return -3;
		}
	}
	else
	{
		strValue = dbelement->Attribute(strNodeName);
		switch(eDataType)
		{
			case DT_UINT8_T:
				if(sizeof(uint8_t) > iValueBufLen)
					return -2;

				if(0 == strncmp(strValue.c_str(), "0x", 2))
					cValue = (uint8_t)strtoul(strValue.c_str(), 0, 16);
				else
					cValue = (uint8_t)atoi(strValue.c_str());
				memcpy(pValue, &cValue, iValueBufLen);
				break;
			case DT_UINT16_T:
				if(sizeof(uint16_t) > iValueBufLen)
					return -2;

				if(0 == strncmp(strValue.c_str(), "0x", 2))
					wValue = (uint16_t)strtoul(strValue.c_str(), 0, 16);
				else
					wValue = (uint16_t)atoi(strValue.c_str());
				memcpy(pValue, &wValue, iValueBufLen);
				break;
			case DT_UINT32_T:
				if(sizeof(uint32_t) > iValueBufLen)
					return -2;

				if(0 == strncmp(strValue.c_str(), "0x", 2))
					dwValue = (uint32_t)strtoul(strValue.c_str(), 0, 16);
				else
					dwValue = (uint32_t)atoi(strValue.c_str());
				memcpy(pValue, &dwValue, iValueBufLen);
				break;
			case DT_UINT64_T:
				if(sizeof(uint64_t) > iValueBufLen)
					return -2;

				if(0 == strncmp(strValue.c_str(), "0x", 2))
					ddwValue = (uint64_t)strtoull(strValue.c_str(), 0, 16);
				else
					ddwValue = (uint64_t)atoll(strValue.c_str());
				memcpy(pValue, &ddwValue, iValueBufLen);
				break;
			case DT_STRING:
				memcpy(pValue, strValue.c_str(), iValueBufLen);
				break;
			default:
				return -3;
		}
		
	}

	va_end(ap);
	
	return 0;
}

