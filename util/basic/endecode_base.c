#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "basic/MyNetworkByteOrder.h"
#include "endecode_base.h"

int GetINT8(char **p, int *piLen, uint8_t *Value)
{
	int iLen;
	char *pCur;
	char *cValue = (char *)Value;
	
	pCur = *p;
	if (piLen) iLen = *piLen;
	else iLen = sizeof(char);
	
	if (iLen < (signed int)sizeof(char)) return -1;
	
	*cValue = *pCur;
	
	pCur += sizeof(char);
	iLen -= sizeof(char);
	
	*p = pCur;
	if (piLen) *piLen = iLen;
	
	return 0;		
	//return GetChar(p, piLen, (char *)Value);
}

int AddINT8(char **p, int *piLenLeft, uint8_t Value)
{
	int iLenLeft = 100;
	char *pCur;
	char cValue = (char)Value;

	pCur = *p;
	if (piLenLeft) iLenLeft = *piLenLeft;

	if (iLenLeft < (signed int)sizeof(cValue)) return -1;
	
	*pCur = cValue;
	pCur += sizeof(cValue);
	iLenLeft -= sizeof(cValue);

	*p = pCur;
	if (piLenLeft) *piLenLeft = iLenLeft;
	
	return 0;	
	//return AddChar(p, piLenLeft, (char)Value);
}

int GetINT16(char **p, int *piLen, uint16_t *pshValue)
{
	uint16_t shNValue;
	int iLen;
	char *pCur;
	
	pCur = *p;
	if (piLen) iLen = *piLen;
	else iLen = sizeof(short);
	
	if (iLen < (signed int)sizeof(shNValue)) return -1;
	
	memcpy(&shNValue, pCur, sizeof(shNValue));
	*pshValue = ntohs(shNValue);
	pCur += sizeof(shNValue);
	iLen -= sizeof(shNValue);
	
	*p = pCur;
	if (piLen) *piLen = iLen;
	
	return 0;	
	//return GetWord(p, piLen, Value);
}

int AddINT16(char **p, int *piLenLeft, uint16_t shValue)
{
	int iLenLeft = 100;
	char *pCur;
	uint16_t shNValue;
	
	pCur = *p;
	if (piLenLeft) iLenLeft = *piLenLeft;

	if (iLenLeft < (signed int)sizeof(shValue)) return -1;
	
	shNValue = htons(shValue);
	memcpy(pCur, &shNValue, sizeof(shNValue));
	pCur += sizeof(shValue);
	iLenLeft -= sizeof(shValue);
		
	*p = pCur;
	if (piLenLeft) *piLenLeft = iLenLeft;
	
	return 0;
	//return AddWord(p, piLenLeft, Value);
}

int GetINT32(char **p, int *piLen, uint32_t *plValue)
{
	uint32_t lNValue;
	int iLen;
	char *pCur;
	
	pCur = *p;
	if (piLen) iLen = *piLen;
	else iLen = sizeof(long);
	
	if (iLen < (signed int)sizeof(lNValue)) return -1;
	
	memcpy(&lNValue, pCur, sizeof(lNValue));
	*plValue = ntohl(lNValue);
	pCur += sizeof(lNValue);
	iLen -= sizeof(lNValue);
	
	*p = pCur;
	if (piLen) *piLen = iLen;
	
	return 0;	
	//return GetDWord(p, piLen, Value);
}

int AddINT32(char **p, int *piLenLeft, uint32_t lValue)
{
	int iLenLeft = 100;
	char *pCur;
	uint32_t lNValue;
	
	pCur = *p;
	if (piLenLeft) iLenLeft = *piLenLeft;

	if (iLenLeft < (signed int)sizeof(lValue)) return -1;

	lNValue = htonl(lValue);
	memcpy(pCur, &lNValue, sizeof(lNValue));
	pCur += sizeof(lValue);
	iLenLeft -= sizeof(lValue);
	
	*p = pCur;
	if (piLenLeft) *piLenLeft = iLenLeft;
	
	return 0;	
	//return AddDWord(p, piLenLeft, Value);
}

int GetINT64(char **ppCur, int *piLeftLen, uint64_t *pval)
{
	if(NULL==ppCur || NULL==pval)
    	{
        	return -1;
    	}
    	if (piLeftLen != NULL && 
        	(*piLeftLen < 0 || *piLeftLen < (int)sizeof(uint64_t)))
    	{
        	return -2;
    	}
    	*pval = *(uint64_t*)(*ppCur);
    	*ppCur = *ppCur + sizeof(uint64_t);
    	if (piLeftLen)
    	{
        	*piLeftLen = *piLeftLen - sizeof(uint64_t);
    	}
    	*pval = ntohll(*pval);
    	return 0;
	//return GetDDWord(p, piLen, Value);
}

int AddINT64(char **ppCur, int *piLeftLen, uint64_t val)
{
	if (NULL == ppCur)
    	{
        	return -1;
    	}
    	if (piLeftLen != NULL && 
        	(*piLeftLen < 0 || *piLeftLen < (int)sizeof(uint64_t)))
    	{
        	return -2;
    	}
    	*(uint64_t*)(*ppCur) = htonll(val);
    	*ppCur = *ppCur + sizeof(uint64_t);
    	if (piLeftLen)
    	{
        	*piLeftLen = *piLeftLen - sizeof(uint64_t);
    	}
    	return 0;
	//return AddDDWord(p, piLenLeft, Value);
}

int GetINT8Array_zero(char **p, int *piLen, uint8_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT8(p, piLen, Buf + (i++))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT8Array_zero(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(AddINT8(p, piLenLeft, *(Buf + (i++)))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT16Array_zero(char **p, int *piLen, uint16_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT16(p, piLen, Buf + (i++))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT16Array_zero(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(AddINT16(p, piLenLeft, *(Buf + (i++)))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT32Array_zero(char **p, int *piLen, uint32_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT32(p, piLen, Buf + (i++))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT32Array_zero(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(AddINT32(p, piLenLeft, *(Buf + (i++)))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT64Array_zero(char **p, int *piLen, uint64_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT64(p, piLen, Buf + (i++))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT64Array_zero(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(AddINT64(p, piLenLeft, *(Buf + (i++)))) return -2;
		if(!Buf[i-1])return 0;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT8Array_split(char **p, int *piLen, uint8_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT8(p, piLen, Buf + (i++))) return -2;
		if(Buf[i-1] == spliter)
		{
			Buf[i-1] = '\0';
			return 0;
		}
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT8Array_split(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(!Buf[i])
		{
			if(AddINT8(p, piLenLeft, spliter)) return -2;
			return 0;
		}
		if(AddINT8(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT16Array_split(char **p, int *piLen, uint16_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT16(p, piLen, Buf + (i++))) return -2;
		if(Buf[i-1] == spliter)
		{
			Buf[i-1] = '\0';
			return 0;
		}
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT16Array_split(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(!Buf[i])
		{
			if(AddINT16(p, piLenLeft, spliter)) return -2;
			return 0;
		}
		if(AddINT16(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT32Array_split(char **p, int *piLen, uint32_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT32(p, piLen, Buf + (i++))) return -2;
		if(Buf[i-1] == spliter)
		{
			Buf[i-1] = '\0';
			return 0;
		}
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT32Array_split(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(!Buf[i])
		{
			if(AddINT32(p, piLenLeft, spliter)) return -2;
			return 0;
		}
		if(AddINT32(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT64Array_split(char **p, int *piLen, uint64_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT64(p, piLen, Buf + (i++))) return -2;
		if(Buf[i-1] == spliter)
		{
			Buf[i-1] = '\0';
			return 0;
		}
	}
	if(*piLen <= 0) return -2;
	else return -3;
}

int AddINT64Array_split(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen, char spliter)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen)
	{
		if(!Buf[i])
		{
			if(AddINT64(p, piLenLeft, spliter)) return -2;
			return 0;
		}
		if(AddINT64(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT8Array_end(char **p, int *piLen, uint8_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT8(p, piLen, Buf + (i++))) return -2;
	}
	if(*piLen == 0) return 0;
	else if(*piLen < 0) return -2;
	else return -3;
}

int AddINT8Array_end(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen && Buf[i] != 0)
	{
		if(AddINT8(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(!Buf[i])return 0;
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT16Array_end(char **p, int *piLen, uint16_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT16(p, piLen, Buf + (i++))) return -2;
	}
	if(*piLen == 0) return 0;
	else if(*piLen < 0) return -2;
	else return -3;
}

int AddINT16Array_end(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen && Buf[i] != 0)
	{
		if(AddINT16(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(!Buf[i])return 0;
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT32Array_end(char **p, int *piLen, uint32_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT32(p, piLen, Buf + (i++))) return -2;
	}
	if(*piLen == 0) return 0;
	else if(*piLen < 0) return -2;
	else return -3;
}

int AddINT32Array_end(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen && Buf[i] != 0)
	{
		if(AddINT32(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(!Buf[i])return 0;
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int GetINT64Array_end(char **p, int *piLen, uint64_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLen || NULL==Buf)return -1;
	int i = 0;
	while(*piLen > 0 && i < iBufLen)
	{
		if(GetINT64(p, piLen, Buf + (i++))) return -2;
	}
	if(*piLen == 0) return 0;
	else if(*piLen < 0) return -2;
	else return -3;
}

int AddINT64Array_end(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen)
{
	if(NULL==p || NULL==piLenLeft || NULL==Buf)return -1;
	int i = 0;
	while(*piLenLeft > 0 && i < iBufLen && Buf[i] != 0)
	{
		if(AddINT64(p, piLenLeft, *(Buf + (i++)))) return -2;
	}
	if(!Buf[i]) return 0;
	if(*piLenLeft <= 0) return -2;
	else return -3;
}

int Set64AtoIBuf(uint64_t *stTo, char  *stFrom)
{
	if(NULL == stTo)
		return -1;
	
	*stTo = (uint64_t)0;
	if(NULL == stFrom)
		return  0;
	else 
	{
		*stTo = (uint64_t )atol(stFrom);
	}

	return 0;
}

int Set32AtoIBuf(uint32_t *stTo,char  *stFrom)
{
	if(NULL == stTo)
		return -1;
	
	*stTo = (uint32_t)0;
	if(NULL == stFrom)
		return  0;
	else 
	{
		*stTo = (uint32_t)atoi(stFrom);
	}

	return 0;
}

int Set16AtoIBuf(uint16_t *stTo,char  *stFrom)
{
	if(NULL == stTo)
		return -1;
	
	*stTo = (uint16_t)0;
	if(NULL == stFrom)
		return  0;
	else 
	{
		*stTo = (uint16_t)atoi(stFrom);
	}

	return 0;
}

int Set8AtoIBuf(uint8_t *stTo,char  *stFrom)
{
	if(NULL == stTo)
		return -1;
	
	*stTo = (uint8_t)0;
	if(NULL == stFrom)
		return  0;
	else 
	{
		*stTo = (uint8_t)atoi(stFrom);
	}

	return 0;
}

int StringTo32Array(char *sList, uint32_t adwList[], uint32_t *pdwListLen)
{
	if(NULL == sList || NULL == adwList || NULL==pdwListLen)
	{
		return -1;
	}
	uint32_t iLen, k;
	char sId[1024];
	char *sPt;
	
	iLen = 0; k = 0;
	sPt = sList;

	while(*sPt != '\0')
	{
		if(*sPt <= '9' && *sPt >= '0')
		{
			sId[k++] = *sPt;
		}
		else
		{
			if(k > 0)
			{
				sId[k] = '\0';
				adwList[iLen++] = atoi(sId);
			}
			k = 0;
		}
		sPt++;
	}
	if(k > 0)
	{
		sId[k] = '\0';
		adwList[iLen++] = atoi(sId);
	}
	*pdwListLen = iLen;
	
	return 0;
}

