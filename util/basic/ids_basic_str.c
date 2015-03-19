#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <netinet/in.h>

#include "common.h"
#include "ids_basic_str.h"

#define HZ_GB 0
#define HZ_BIG5 1
#define HZ_JPN 2
#define HZ_GBK 3
#define HZ_TGB 4
#define HZ_TSIGN 5
#define HZ_TJPN 6

#define BOOL int

BOOL
OI_ISDBCSLEAD(int CodeType, unsigned char c)
{
    switch (CodeType)
    {
        case HZ_GB:
            return (c > 0xa0 && c < 0xff);
        case HZ_BIG5:
            return (c > 0xa0 && c < 0xff);
        case HZ_JPN:
            return ((c > 0x80 && c < 0xa0) || (c > 0xdf && c < 0xfd));
        case HZ_GBK:
            return (c > 0x80 && c < 0xff);
        case HZ_TGB:
            return (c > 0xaf && c < 0xf8);
        case HZ_TSIGN:
            return ((c == 0xa1) || (c == 0xa3));
        case HZ_TJPN:
            return ((c == 0xa4) || (c == 0xa5));
    }
    return 0;
}

BOOL
OI_ISDBCSNEXT(int CodeType, unsigned char c)
{
    switch (CodeType)
    {
        case HZ_GB:
            return (c > 0xa0 && c < 0xff);
        case HZ_BIG5:
            return ((c > 0x3f && c < 0x7f) || (c > 0xa0 && c < 0xff));
        case HZ_JPN:
            return (c > 0x3f && c < 0xfd);
        case HZ_GBK:
            return ((c > 0x3f && c < 0x7f) || (c > 0x7f && c < 0xff));
        case HZ_TGB:
            return (c > 0xa0 && c < 0xff);
        case HZ_TSIGN:
            return (c > 0xa0 && c < 0xff);
        case HZ_TJPN:
            return (c > 0xa0 && c < 0xf4);
    }
    return 0;
}

int
OI_is_bad_string(char *buf)
{
    int i;
    int j = 0, t = 0, u = 0;
    char cflt[256];
    char eflt[256];
    char *p, *q, *r;

    p = buf;
    q = buf;
    r = buf;

    i = strlen(buf);
    if(i > 255)
        i = 255;
    for(j = 0; j < i; j++)
    {
        if(isprint(*(buf + j)))
        {
            eflt[u++] = (char) tolower((int) *(buf + j));;
            continue;
        }
        if(OI_ISDBCSLEAD(HZ_TGB, *(buf + j)))
        {
            if(OI_ISDBCSNEXT(HZ_TGB, *(buf + j + 1)))
            {
                cflt[t++] = *(buf + j);
                cflt[t++] = *(buf + j + 1);
                j++;
                continue;
            }
        }
        if(OI_ISDBCSLEAD(HZ_TSIGN, *(buf + j)))
        {
            if(OI_ISDBCSNEXT(HZ_TSIGN, *(buf + j + 1)))
            {
                cflt[t++] = *(buf + j);
                cflt[t++] = *(buf + j + 1);
                j++;
                continue;
            }
        }

        if(OI_ISDBCSLEAD(HZ_TJPN, *(buf + j)))
        {
            if(OI_ISDBCSNEXT(HZ_TJPN, *(buf + j + 1)))
            {
                cflt[t++] = *(buf + j);
                cflt[t++] = *(buf + j + 1);
                j++;
                continue;
            }
        }
        return 1;   /* 不可识别字符 */
    }
    cflt[t] = 0;
    eflt[u] = 0;
    p = cflt;
    q = eflt;

    return 0;
}

int
OI_is_simple_str(char *sBuf)
{

    int digits = 0;
    int uppers = 0;
    int lowers = 0;
    int others = 0;
    size_t i;

    if(strlen(sBuf) < 7)
        return 1;

    for(i = 0; i < strlen(sBuf); i++)
    {
        if(isdigit(sBuf[i]))
            digits++;
        else if(isupper(sBuf[i]))
            uppers++;
        else if(islower(sBuf[i]))
            lowers++;
        else
            others++;
    }

    if(strlen(sBuf) < 10)
    {
        if((size_t) digits == strlen(sBuf))
        {
            return 1;
        }
        else if((size_t) lowers == strlen(sBuf))
        {
            return 1;
        }
        else if((size_t) uppers == strlen(sBuf))
        {
            return 1;
        }
    }

    return 0;
}

char *
OI_get_val(char *desc, char *src)
{
    char *descp = desc, *srcp = src;
    int mtime = 0, space = 0;

    while(mtime != 2 && *srcp != '\0')
    {
        switch (*srcp)
        {
            case ' ':
            case '\t':
            case '\0':
            case '\n':
            case US:
                space = 1;
                srcp++;
                break;
            default:
                if(space || srcp == src)
                    mtime++;
                space = 0;
                if(mtime == 2)
                    break;
                *descp = *srcp;
                descp++;
                srcp++;
        }
    }
    *descp = '\0';
    strcpy(src, srcp);
    return desc;
}

char *
OI_get_unit(char *desc, char *src)
{
    char *descp = desc, *srcp = src;

    if(*srcp == ETX)
    {
        *descp = '\0';
        return desc;
    }
    while(*srcp != ETX && *srcp != US)
    {
        if(*srcp == '\'')
            *descp++ = '\\';
        *descp++ = *srcp++;
    }
    *descp = '\0';
    if(*srcp == ETX)
    {
        *src = *srcp;
        return desc;
    }
    do
    {
        *src++ = *(++srcp);
    } while(*srcp && *srcp != ETX);
    *src = '\0';
    return desc;
}

char *
OI_getunit(char *desc, char *src)
{
    char *descp = desc, *srcp = src;

    if(*srcp == ETX)
    {
        *descp = '\0';
        return desc;
    }
    while(*srcp != ETX && *srcp != US)
    {
        *descp++ = *srcp++;
    }
    *descp = '\0';
    if(*srcp == ETX)
    {
        *src = *srcp;
        return desc;
    }
    do
    {
        *src++ = *(++srcp);
    } while(*srcp != ETX);
    *src = '\0';
    return desc;
}

char *
OI_nGetVal(char *desc, char *src, int iLen)
{
    char *descp = desc, *srcp = src;
    int mtime = 0, space = 0;
    int iCurrLen = 0;

    while(mtime != 2 && *srcp != '\0' && iCurrLen < iLen - 1)
    {
        switch (*srcp)
        {
            case ' ':
            case '\t':
            case '\0':
            case '\n':
            case US:
                space = 1;
                srcp++;
                break;
            default:
                if(space || srcp == src)
                    mtime++;
                space = 0;
                if(mtime == 2)
                    break;
                *descp = *srcp;
                descp++;
                iCurrLen++;
                srcp++;
        }
    }
    *descp = '\0';
    strcpy(src, srcp);
    return desc;
}

char *
OI_nGetUnit(char *sDesc, char *sSrc, int iLen)
{
    char *sDescp = sDesc, *sSrcp = sSrc;
    int iCurrLen = 0;

    if(*sSrcp == ETX)
    {
        *sDescp = '\0';
        return sDesc;
    }
    while(*sSrcp != ETX && *sSrcp != US && *sSrcp != RS && iCurrLen++ < iLen)
    {
        *sDescp++ = *sSrcp++;
    }
    *sDescp = '\0';
    if(*sSrcp == ETX)
    {
        *sSrc = *sSrcp;
        return sDesc;
    }
    do
    {
        *sSrc++ = *(++sSrcp);
    } while(*sSrcp != ETX);
    *sSrc = '\0';
    return sDesc;
}

char *
OI_nGetUnit2(char *sDesc, char *sSrc, int iLen)
{
    char *sDescp = sDesc, *sSrcp = sSrc, cLen;
    int iCurrentLen = 0;

    if(*sSrcp == ETX)
    {
        *sDescp = '\0';
        return sDesc;
    }
    cLen = *sSrcp;
    while(*sSrcp != ETX && iCurrentLen <= cLen && iCurrentLen <= iLen)
    {
        *sDescp++ = *(++sSrcp);
        ++iCurrentLen;
    }
    *sDescp = '\0';
    if(*sSrcp == ETX)
    {
        *sSrc = *sSrcp;
        return sDesc;
    }
    do
    {
        *sSrc++ = *(++sSrcp);
    } while(*sSrcp != ETX);
    *sSrc = '\0';
    return sDesc;
}

/*
int AddUnit(char **sDest, unsigned short shLen, char *sSrc)
{
unsigned short shNlen;
int iLen;

	if (shLen>0)
		iLen=shLen;		
	else
	{
		iLen=strlen(sSrc);
		shNlen=htons(iLen);
		memcpy(*sDest, &shNlen, sizeof(short));
		(*sDest)+=sizeof(short);
	}

	memcpy(*sDest, sSrc, iLen);

	(*sDest)+=iLen;
	
	return 0;
}
*/

char *
OI_nFetchUnit(char *sDesc, char *sSrc, int iLen)
{
    static char *sSrcp;
    char *sDescp = sDesc;
    int iCurrLen = 0;

    if(sSrc)
        sSrcp = sSrc;
    if(*sSrcp == ETX)
    {
        *sDescp = '\0';
        return sDesc;
    }
    while(*sSrcp != ETX && *sSrcp != US && iCurrLen++ < iLen)
    {
        *sDescp++ = *sSrcp++;
    }
    *sDescp = '\0';
    return sDesc;
}

char *
OI_encode(char *buffer, char *toencode)
{
    size_t i;

    buffer[0] = '\0';
    for(i = 0; i < strlen(toencode); i++)
        switch (toencode[i])
        {
            case '&':
                strcat(buffer, "%26");
                break;
            case '=':
                strcat(buffer, "%3D");
                break;
            case '%':
                strcat(buffer, "%25");
                break;
            case '+':
                strcat(buffer, "%2B");
                break;
            case '\n':
                strcat(buffer, "%0A");
                break;
            case '\r':
                strcat(buffer, "%0D");
                break;
            case ' ':
                strcat(buffer, "+");
                break;
            default:
                snprintf(buffer, sizeof(buffer), "%s%c", buffer, toencode[i]);
        }
    return buffer;
}



char *
OI_randstr(char *buffer, int len)
{
    //char *chars="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
    char *chars = "ABCDEFGHIJKMNPQRSTUVWXYZabcdefghijkmnpqrstuvwxyz23456789";   // 为减少误判， 去掉0、1、O、o、L、l
    int i, chars_len;

    chars_len = strlen(chars);
    for(i = 0; i < len; i++)
        buffer[i] = chars[(int) ((float) strlen(chars) * rand() / (RAND_MAX + 1.0))];
    buffer[len] = '\0';
    return buffer;
}


char *
OI_randstr2(char *buffer, int len)
{
    char *chars = "abcdefghijkmnpqrstuvwxyz23456789";   // 为减少误判， 去掉0、1、O、o、L、l
    int i, chars_len;

    chars_len = strlen(chars);
    for(i = 0; i < len; i++)
        buffer[i] = chars[(int) ((float) strlen(chars) * rand() / (RAND_MAX + 1.0))];
    buffer[len] = '\0';
    return buffer;
}

char *
OI_randstr_number(char *buffer, int len)
{
    char *chars = "0123456789";
    int i, chars_len;

    chars_len = strlen(chars);
    for(i = 0; i < len; i++)
        buffer[i] = chars[(int) ((float) strlen(chars) * rand() / (RAND_MAX + 1.0))];
    buffer[len] = '\0';
    return buffer;
}


char *
OI_MyQuote(char *dest, char *src)
{
    register int i, j;

    i = j = 0;

    while(src[i])
    {
        switch ((unsigned char) src[i])
        {
            case '\n':
                dest[j++] = '\\';
                dest[j++] = 'n';
                break;
            case '\t':
                dest[j++] = '\\';
                dest[j++] = 't';
                break;
            case '\r':
                dest[j++] = '\\';
                dest[j++] = 'r';
                break;
            case '\b':
                dest[j++] = '\\';
                dest[j++] = 'b';
                break;
            case '\'':
                dest[j++] = '\\';
                dest[j++] = '\'';
                break;
            case '\"':
                dest[j++] = '\\';
                dest[j++] = '\"';
                break;
            case '\\':
                dest[j++] = '\\';
                dest[j++] = '\\';
                break;
            default:
                dest[j++] = src[i];
        }
        i++;
    }
    dest[j] = 0;
    return dest;
}


char *
OI_MyQuoteWild(char *dest, char *src)
{
    register int i, j;

    i = j = 0;
    while(src[i])
    {
        switch ((unsigned char) src[i])
        {
            case '\n':
                dest[j++] = '\\';
                dest[j++] = 'n';
                break;
            case '\t':
                dest[j++] = '\\';
                dest[j++] = 't';
                break;
            case '\r':
                dest[j++] = '\\';
                dest[j++] = 'r';
                break;
            case '\b':
                dest[j++] = '\\';
                dest[j++] = 'b';
                break;
            case '\'':
                dest[j++] = '\\';
                dest[j++] = '\'';
                break;
            case '\"':
                dest[j++] = '\\';
                dest[j++] = '\"';
                break;
            case '\\':
                dest[j++] = '\\';
                dest[j++] = '\\';
                break;
            case '%':
                dest[j++] = '\\';
                dest[j++] = '%';
                break;
            case '_':
                dest[j++] = '\\';
                dest[j++] = '_';
                break;
            default:
                dest[j++] = src[i];
        }
        i++;
    }
    dest[j] = 0;
    return dest;
}

/* Show Hex val of a string, if iFlag = 1, show printable character as char */
void
OI_HexShow(unsigned char *sStr, int iLen, int iFlag)
{
    register int iCount;

    for(iCount = 0; iCount < iLen; iCount++)
    {
        if(iCount % 25 == 0)
            printf("\n");
        if(iFlag && sStr[iCount] > 0x1f)
            printf("%2c ", sStr[iCount]);
        else
            printf("%.2x ", sStr[iCount]);
    }
    if((iCount - 1) % 25)
        printf("\n");
}

char *
OI_HexString(unsigned char *sStr, int iLen)
{
    int iCount;
    static char sBuffer[1024];
    char sTmp[10];

    strcpy(sBuffer, "");
    for(iCount = 0; iCount < iLen; iCount++)
    {
        if(strlen(sBuffer) > 1000)
            break;

        if(iCount % 25 == 0)
            strcat(sBuffer, "\n");
        snprintf(sTmp, sizeof(sTmp), "%.2x ", sStr[iCount]);
        strcat(sBuffer, sTmp);
    }
    if((iCount - 1) % 25)
        strcat(sBuffer, "\n");

    return sBuffer;
}

char *
OI_SkipSpace(register char *sStr)
{
    while(*sStr == ' ' || *sStr == '\t' || *sStr == '\r' || *sStr == '\n')
        sStr++;
    return sStr;
}

int
OI_IsGsmNo(char *sGsmNo)
{
    int iCount, iNoLen;

    iNoLen = strlen(sGsmNo);
    if(iNoLen != 11 && iNoLen != 10)
        return 0;
    for(iCount = 0; iCount < iNoLen; iCount++)
        if(!isdigit(sGsmNo[iCount]))
            return 0;
    if(strncmp(sGsmNo, "13", 2) && strncmp(sGsmNo, "15", 2) && strncmp(sGsmNo, "852", 3) && strncmp(sGsmNo, "853", 3))
        return 0;
    return 1;
}

char *
OI_IsEmailAddr(char *sEmail)
{
    char *sPtr, *sPtr1;
    int iLen;

    if((iLen = strlen(sEmail)) < 7)
        return NULL;
    if(!isalpha(sEmail[iLen - 1]))
        return NULL;
    if(!(sPtr = strchr(sEmail, '@')) || sPtr == sEmail)
        return NULL;
    if(!(sPtr1 = strchr(sPtr, '.')) || sPtr1 - sPtr < 3)
        return NULL;
    return sEmail;
}

char
OI_StrHashByte(char *sStr, unsigned char cModule)
{
    unsigned long lNumerator;
    int iLen;
    unsigned char cRemainder = 0;
    register int i;

    iLen = strlen(sStr);
    for(i = 0; i <= iLen - 3; i += 3)
    {
        cRemainder = ((cRemainder << 24) + (sStr[i] << 16) + (sStr[i + 1] << 8) + sStr[i + 2]) % cModule;
    }
    lNumerator = cRemainder;
    for(i = iLen - i; i > 0; i--)
    {
        lNumerator = (lNumerator << 8) + sStr[iLen - i];
    }
    return lNumerator % cModule;
}

unsigned short
OI_StrHashWord(char *sStr, unsigned short shModule)
{
    unsigned long lNumerator;
    int iLen;
    unsigned short shRemainder = 0;
    register int i;

    iLen = strlen(sStr);
    for(i = 0; i <= iLen - 2; i += 2)
    {
        shRemainder = ((shRemainder << 16) + (sStr[i] << 8) + sStr[i + 1]) % shModule;
    }
    lNumerator = shRemainder;
    for(i = iLen - i; i > 0; i--)
    {
        lNumerator = (lNumerator << 8) + sStr[iLen - i];
    }
    return lNumerator % shModule;
}

char *
OI_tolower_string(char *str)
{
    int i;
    int len = strlen(str);

    for(i = 0; i < len; i++)
        str[i] = tolower(str[i]);

    return str;
}

char *
OI_toupper_string(char *str)
{
    int i;
    int len = strlen(str);

    for(i = 0; i < len; i++)
        str[i] = toupper(str[i]);
    return str;
}

void
OI_hex_dump(const char *data, long size)
{
    long i;
    long x;
    int r = 1;
    char d[32];
    unsigned char bfr[64];

    for(i = 0, x = 0;; i++)
    {
        if(i < size)
        {
            if(x == 0)
                printf("%04lx: ", i);
            snprintf(d, 9, "%08x", data[i]);
            printf("%c%c ", d[6], d[7]);
            bfr[x] = data[i];
            if(bfr[x] < 0x20)
                bfr[x] = '.';
            if(bfr[x] > 0x7f)
                bfr[x] = '.';
        }
        else
        {
            if(x == 0)
                break;
            else
            {
                printf("   ");
                bfr[x] = ' ';
                r = 0;
            }
        }
        x++;
        if(!(x < 16))
        {
            bfr[x] = 0;
            printf("%s\n", bfr);
            x = 0;
            if(!r)
                break;
        }
    }
}

void
OI_hex_dump_tofile(FILE * pfile, const char *data, long size)
{
    OI_hex_dump_toFd(fileno(pfile), data, size);
}

void
OI_hex_dump_toFd(int iFd, const char *data, long size)
{
    long i;
    long x;
    int r = 1;
    char d[32];
    unsigned char bfr[64];

    for(i = 0, x = 0;; i++)
    {
        if(i < size)
        {
            if(x == 0)
            {
                snprintf(d, sizeof(d), "%04lx: ", i);
                write(iFd, d, strlen(d));
            }
            snprintf(d, 9, "%08x", data[i]);
            write(iFd, &d[6], 2);

            bfr[x] = data[i];
            if(bfr[x] < 0x20)
                bfr[x] = '.';
            if(bfr[x] > 0x7f)
                bfr[x] = '.';
        }
        else
        {
            if(x == 0)
                break;
            else
            {
                write(iFd, "   ", strlen("   "));
                bfr[x] = ' ';
                r = 0;
            }
        }
        x++;
        if(!(x < 16))
        {
            bfr[x] = 0;
            write(iFd, bfr, strlen((const char *) bfr));
            write(iFd, "\n", 1);
            x = 0;
            if(!r)
                break;
        }
    }
}


const char *
OI_DumpPackage(void *pPkg, int iPkgLen)
{
    int i, inc;
    static char sLogBuf[65536 * 4];
    char *pDst = sLogBuf, *pSrc = pPkg;

    sLogBuf[0] = '\0';
    for(i = 0; i < iPkgLen; i++, pSrc++)
    {
        char *sSep, sAddr[20];

        switch (i % 16)
        {
            case 3:
            case 11:
                sSep = " - ";
                break;
            case 7:
                sSep = " | ";
                break;
            case 15:
                sSep = "\n";
                break;
            default:
                sSep = " ";
                break;
        }
        switch (i % 16)
        {
            case 0:
                snprintf(sAddr, sizeof(sAddr), "%04hX: ", i);
                break;
            default:
                sAddr[0] = '\0';
                break;
        }
        inc = snprintf(pDst, sLogBuf + sizeof(sLogBuf) - pDst, "%s%02X%s", sAddr, (unsigned char) *pSrc, sSep);
        if(inc < 0)
        {
            break;
        }
        pDst += inc;
        if(pDst >= sLogBuf + sizeof(sLogBuf))
        {
            break;
        }
    }

    return sLogBuf;
}

const char *
OI_DumpHex(void *pMem, size_t uDumpOffset, size_t uDumpLen)
{
    int inc;
    size_t i;
    static char sLogBuf[65536 * 4];
    char *pDst = sLogBuf, *pSrc = pMem + uDumpOffset;

    sLogBuf[0] = '\0';
    for(i = uDumpOffset; i < uDumpOffset + uDumpLen; i++, pSrc++)
    {
        inc = snprintf(pDst, sLogBuf + sizeof(sLogBuf) - pDst, "%02X", (unsigned char) *pSrc);
        if(inc < 0)
        {
            break;
        }
        pDst += inc;
        if(pDst >= sLogBuf + sizeof(sLogBuf))
        {
            break;
        }
    }

    return sLogBuf;
}
