#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "ids_basic_str2.h"

int
OI_GetChar(char **p, int *piLen, char *cValue)
{
    int iLen;
    char *pCur;

    if(p == NULL || cValue == NULL)
        return -1;
    if(piLen != NULL &&(*piLen < 0 || *piLen < sizeof(char))) //modified by diegojiang at 2011-10-14
        return -2;
   
    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = sizeof(char);

    *cValue = *pCur;

    pCur += sizeof(char);
    iLen -= sizeof(char);

    *p = pCur;
    if(piLen)
        *piLen = iLen;

    return 0;
}

int
OI_GetWord(char **p, int *piLen, unsigned short *pshValue)
{
    unsigned short shNValue;
    int iLen;
    char *pCur;

    if(p == NULL || pshValue == NULL)
        return -1;
    if(piLen != NULL &&(*piLen < 0 || *piLen < sizeof(short))) //modified by diegojiang at 2011-10-14
        return -2;

    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = sizeof(short);

    memcpy(&shNValue, pCur, sizeof(shNValue));
    *pshValue = ntohs(shNValue);
    pCur += sizeof(shNValue);
    iLen -= sizeof(shNValue);

    *p = pCur;
    if(piLen)
        *piLen = iLen;

    return 0;
}

int
OI_GetDWord(char **p, int *piLen, unsigned int *piValue)
{
    unsigned int iNValue;
    int iLen;
    char *pCur;

    if(p == NULL || piValue == NULL)
        return -1;
    if(piLen != NULL &&(*piLen < 0 || *piLen < sizeof(int))) //modified by diegojiang at 2011-10-14
        return -2;

    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = sizeof(int);

    memcpy(&iNValue, pCur, sizeof(iNValue));
    *piValue = ntohl(iNValue);
    pCur += sizeof(iNValue);
    iLen -= sizeof(iNValue);

    *p = pCur;
    if(piLen)
        *piLen = iLen;

    return 0;
}

int
OI_GetString(char **p, int *piLen, char *pBuf, int iBufLen)
{
    unsigned short shLen, shCopyLen;
    int iLen;
    char *pCur;

    if(p == NULL || pBuf == NULL)
        return -1;
    if(OI_GetWord(p, piLen, &shLen) < 0)
        return -2;

    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = iBufLen;

    if(shLen > iLen)
        return -3;
    if(iBufLen > shLen)
        shCopyLen = shLen;
    else
        shCopyLen = iBufLen - 1;

    memcpy(pBuf, pCur, shCopyLen);
    *(pBuf + shCopyLen) = 0;
    pCur += shLen;
    iLen -= shLen;

    *p = pCur;
    if(piLen)
        *piLen = iLen;

    return 0;
}

int
OI_GetString2(char **p, int *piLen, char *pBuf, int iBufLen)
{
    unsigned short shCopyLen;
    int iLen;
    char *pCur;
    unsigned char cLen;

    if(p == NULL || pBuf == NULL)
        return -1;
    if(OI_GetChar(p, piLen, (char *) &cLen) < 0)
        return -2;

    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = iBufLen;

    if(cLen > iLen)
        return -2;
    if(iBufLen > cLen)
        shCopyLen = cLen;
    else
        shCopyLen = iBufLen - 1;

    memcpy(pBuf, pCur, shCopyLen);
    *(pBuf + shCopyLen) = 0;
    pCur += cLen;
    iLen -= cLen;

    *p = pCur;
    if(piLen)
        *piLen = iLen;

    return 0;
}

int
OI_GetBuffer(char **p, int *piLen, char *pBuf, int *piBufLen)
{
    unsigned short shLen, shCopyLen;
    int iLen, iBufLen;
    char *pCur;

    if(p == NULL || piLen == NULL || pBuf == NULL || piBufLen == NULL) //modified by diegojiang at 2011-10-14
        return -1;
    iBufLen = *piBufLen;

    if(OI_GetWord(p, piLen, &shLen) < 0)
        return -2;

    pCur = *p;
    if(piLen)
        iLen = *piLen;
    else
        iLen = shLen;

    if(shLen > iLen)
        return -3;
    if(iBufLen > shLen)
        shCopyLen = shLen;
    else
        shCopyLen = iBufLen - 1;

    memcpy(pBuf, pCur, shCopyLen);
    *(pBuf + shCopyLen) = 0;
    pCur += shLen;
    iLen -= shLen;

    *p = pCur;
    if(piLen)
        *piLen = iLen;
    *piBufLen = shLen;

    return 0;
}


int
OI_AddChar(char **p, int *piLenLeft, char cValue)
{
    int iLenLeft = 100;
    char *pCur;

    if(p == NULL)
        return -1;
    if(piLenLeft != NULL &&(*piLenLeft < 0 || *piLenLeft < sizeof(char))) //modified by diegojiang at 2011-10-14
        return -2;

    pCur = *p;
    if(piLenLeft)
        iLenLeft = *piLenLeft;

    *pCur = cValue;
    pCur += sizeof(cValue);
    iLenLeft -= sizeof(cValue);

    *p = pCur;
    if(piLenLeft)
        *piLenLeft = iLenLeft;

    return 0;
}

int
OI_AddWord(char **p, int *piLenLeft, unsigned short shValue)
{
    int iLenLeft = 100;
    char *pCur;
    unsigned short shNValue;

    if(p == NULL)
        return -1;
    if(piLenLeft != NULL &&(*piLenLeft < 0 || *piLenLeft < sizeof(short))) //modified by diegojiang at 2011-10-14
        return -2;

    pCur = *p;
    if(piLenLeft)
        iLenLeft = *piLenLeft;

    shNValue = htons(shValue);
    memcpy(pCur, &shNValue, sizeof(shNValue));
    pCur += sizeof(shValue);
    iLenLeft -= sizeof(shValue);

    *p = pCur;
    if(piLenLeft)
        *piLenLeft = iLenLeft;

    return 0;
}

int
OI_AddDWord(char **p, int *piLenLeft, unsigned int iValue)
{
    int iLenLeft = 100;
    char *pCur;
    unsigned int iNValue;

    if(p == NULL)
        return -1;
    if(piLenLeft != NULL &&(*piLenLeft < 0 || *piLenLeft < sizeof(int))) //modified by diegojiang at 2011-10-14
        return -2;

    pCur = *p;
    if(piLenLeft)
        iLenLeft = *piLenLeft;

    iNValue = htonl(iValue);
    memcpy(pCur, &iNValue, sizeof(iNValue));
    pCur += sizeof(iValue);
    iLenLeft -= sizeof(iValue);

    *p = pCur;
    if(piLenLeft)
        *piLenLeft = iLenLeft;

    return 0;
}

int
OI_AddString(char **p, int *piLenLeft, char *pBuf, unsigned short shBufLen)
{
    int iLenLeft = 5000;
    char *pCur;

    if(pBuf == NULL)
        return -1;
    if(OI_AddWord(p, piLenLeft, shBufLen) < 0)
        return -2;

    pCur = *p;
    if(piLenLeft)
        iLenLeft = *piLenLeft;

    if(iLenLeft < shBufLen)
        return -3;

    memcpy(pCur, pBuf, shBufLen);
    pCur += shBufLen;
    iLenLeft -= shBufLen;

    *p = pCur;
    if(piLenLeft)
        *piLenLeft = iLenLeft;

    return 0;
}

int
OI_AddString2(char **p, int *piLenLeft, char *pBuf, unsigned short shBufLen)
{
    int iLenLeft = 5000;
    char *pCur;

    if(pBuf == NULL)
        return -1;
    if(OI_AddChar(p, piLenLeft, shBufLen) < 0)
        return -2;

    pCur = *p;
    if(piLenLeft)
        iLenLeft = *piLenLeft;

    if(iLenLeft < shBufLen)
        return -3;

    memcpy(pCur, pBuf, shBufLen);
    pCur += shBufLen;
    iLenLeft -= shBufLen;

    *p = pCur;
    if(piLenLeft)
        *piLenLeft = iLenLeft;

    return 0;
}

char *
OI_MyGetToken(char *sToken, int iTokenSize, char *sString, char *sSeparator)
{
    char *pe;
    int iBytesToCopy;
    int i;

    pe = strstr(sString, sSeparator);
    if(pe == NULL)
    {
        if(sToken != NULL)
        {
            strcpy(sToken, sString);
        }
        return sString + strlen(sString);
    }
    else
    {
        if(sToken != NULL)
        {
            iBytesToCopy = pe - sString;
            if(iBytesToCopy > iTokenSize - 1)
            {
                iBytesToCopy = iTokenSize - 1;
            }

            for(i = 0; i < iBytesToCopy; i++)
            {
                sToken[i] = sString[i];
            }
            sToken[iBytesToCopy] = 0;
        }

        return pe + strlen(sSeparator);
    }
}

char *
OI_Str_Trim(char *s)
{
    char *pb;
    char *pe;
    char *ps;
    char *pd;

    if(strcmp(s, "") == 0)
        return s;

    pb = s;

    while(((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
    {
        pb++;
    }

    pe = s;
    while(*pe != 0)
    {
        pe++;
    }
    pe--;
    while((pe >= s) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
    {
        pe--;
    }

    ps = pb;
    pd = s;
    while(ps <= pe)
    {
        *pd = *ps;
        ps++;
        pd++;
    }
    *pd = 0;

    return s;
}

void *
OI_my_bsearch(const void *key, const void *base, size_t nmemb, size_t size, int *piEqual, int (*compar) (const void *, const void *))
{
    size_t l, u, idx;
    const void *p, *p2;
    int comparison, comparison2;

    *piEqual = 0;
    l = 0;
    u = nmemb;

    while(l < u)
    {
        idx = (l + u) / 2;
        p = (void *) (((const char *) base) + (idx * size));
        comparison = (*compar) (key, p);

        if(comparison == 0)
        {
            *piEqual = 1;
            return (void *) p;
        }
        else if(comparison < 0)
        {
            if(idx == 0)
                return (void *) p;

            p2 = (void *) (((const char *) base) + ((idx - 1) * size));
            comparison2 = (*compar) (key, p2);

            if(comparison2 > 0)
                return (void *) p;

            u = idx;
        }
        else    /*if (comparison > 0) */
        {
            l = idx + 1;
        }
    }

    return NULL;
}


//DDWORD类型处理
#define swap_ddword(x)  \
	((((x) & 0xff00000000000000llu) >> 56) | \
	 (((x) & 0x00ff000000000000llu) >> 40) | \
	 (((x) & 0x0000ff0000000000llu) >> 24) | \
	 (((x) & 0x0000ff0000000000llu) >> 24) | \
	 (((x) & 0x000000ff00000000llu) >> 8) | \
	 (((x) & 0x00000000ff000000llu) << 8) | \
	 (((x) & 0x0000000000ff0000llu) << 24) | \
	 (((x) & 0x000000000000ff00llu) << 40) | \
	 (((x) & 0x00000000000000ffllu) << 56) )

uint64_t OI_Switch(uint64_t a)
{   
#if 0
	uint64_t iRet;
	int i;
	const unsigned char *psrc = NULL;
	unsigned char *pdst = NULL;
#endif

#if BYTE_ORDER == BIG_ENDIAN
	return a;
#elif BYTE_ORDER == LITTLE_ENDIAN
#if 0
	psrc = (const unsigned char *)&a;
	pdst = (unsigned char *)&iRet;
	for (i = 0; i < 8; ++i)
	{
		pdst[i] = psrc[7-i];
	}
	return iRet;
#else
	return swap_ddword(a);
#endif
#else
#error "What kind of system is this?"
#endif
}  

int OI_GetDDWord(char ** ppCur, int * piLeftLen, uint64_t * pval)
{
	if (ppCur == NULL || pval == NULL)
	{
		return -1;
	}

	if (piLeftLen != NULL && (*piLeftLen < 0 || *piLeftLen < sizeof(uint64_t)))
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
}

int OI_AddDDWord(char ** ppCur, int * piLeftLen, uint64_t val)
{
	if (ppCur == NULL )
	{
		return -1;
	}
	if (piLeftLen != NULL && (*piLeftLen < 0 || *piLeftLen < sizeof(uint64_t)))
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
}


int OI_GetTLV(char **p,int *piLen,int *piTag,int *piValueLen,void *pvalue,int iValueSize)  
{
   int iTag = 0 ;
   int iLen = 0 ;
   int offset = 0 ;
   int i = 0 ;
   unsigned char *pCur = (unsigned char *)*p ;

   if( *piLen < 0 )
       return(-1);

   for( i = 0 ; pCur[offset++] == 255ul  ; ++i )
       iTag += 255ul ;
   iTag += pCur[offset++] ;

   for( i = 0 ; pCur[offset++] == 255ul  ; ++i )
       iLen += 255ul ;
   iLen += pCur[offset++] ;

   if( *piLen - offset  < 0 )
       return(-2);

   if( iLen > iValueSize )
       return(-3);

   if( *piLen - offset - iLen < 0 )
       return(-4);

   *piTag = iTag ;
   *piValueLen = iLen ;

   memcpy(pvalue,pCur+offset,iLen) ;
   offset+=iLen ;

   *p += offset ;
   *piLen -= offset ;

   return(0);
}

int OI_AddTLV(char **p,int * piLeftLen,int iTag,int iLen,void *pvalue)
{
    char *pCur = *p ;
	int offset = 0 ;

	if( iTag >= 255 ) 
	{
		memset(pCur,255,iTag/255);
		offset += iTag/255 ;
	}

	pCur[offset++] = iTag%255 ;

	if( iLen >= 255 ) 
	{
		memset(pCur,255,iLen/255);
		offset += iLen/255 ;
	}

	pCur[offset++] = iLen%255 ;

	memcpy(pCur+offset,pvalue,iLen);
	offset+= iLen ;

	*p += offset ;
	*piLeftLen -= offset ;

	return(0);
}

void *OI_memcpy(void *dest, size_t dest_size,const void *src, size_t n)
{
    if( dest_size < n )
	     return(NULL) ;
	
	return(memcpy(dest,src,n));
}

void *OI_memmove(void *dest, size_t dest_size,const void *src, size_t n)
{
    if( dest_size < n )
	     return(NULL) ;
	
	return(memmove(dest,src,n));
}
