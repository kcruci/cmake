#ifndef _GAME_BASE_GET_CONFIG_H
#define _GAME_BASE_GET_CONFIG_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "common.h"
#include "tinyxml.h"
/*Begin added by Frank.cheng at 20155.03.03*/
struct IDS_ConfigCommon
{
	IDS_ConfigCommon()
	{
		dwTimerCheckIntervalUs = 10* 1000;
	}

	uint32_t dwTimerCheckIntervalUs;
	std::vector<unsigned> vecTimerNodeNum;
	std::vector<unsigned> vecTimerNodeNumForTest;

	uint32_t iTest;
};

void atoi_num_list(const char* s, char splitchar,std::vector<unsigned>& vecOut);  

int IDS_GetCommonConfig(TiXmlElement *element, IDS_ConfigCommon & configOut);

/*End added by Frank.cheng at 20155.03.03*/

int GetOneNodeConfig(TiXmlElement *dbelement, const char *strNodeName, basic_data_type eDataType, 
	void *pValue, size_t iValueBufLen, ...);

#endif
