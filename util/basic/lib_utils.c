#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <regex.h>
#include <errno.h>
#include <sys/file.h>

#include "lib_net_const.h"
#include "lib_utils.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

link_warning(PacketReadString, "warning: the 'PacketReadString' is dangerous.")
     int OI_Atoi(char *str)
{
    if(str == NULL)
        return -1;
    else
        return atoi(str);
}

long
OI_Atol(char *str)
{
    if(str == NULL)
        return -1;
    else
        return atol(str);
}

char *
OI_nextword(char **str)
{
    char *head, *tail;
    int ch;

    head = *str;
    for(;;)
    {
        ch = *head;
        if(!ch)
        {
            *str = head;
            return head;
        }
        if(!OI_Isspace(ch))
            break;
        head++;
    }
    tail = head + 1;
    while((ch = *tail))
    {
        if(OI_Isspace(ch))
        {
            *tail++ = '\0';
            break;
        }
        tail++;
    }
    *str = tail;
    return head;
}

char *
OI_nextfield(char **str, char delim)
{
    char *head, *tail;
    int ch;

    head = *str;
    for(;;)
    {
        ch = *head;
        if(!ch)
        {
            *str = head;
            return head;
        }
        if(ch != delim)
            break;
        head++;
    }

    tail = head + 1;
    while((ch = *tail))
    {
        if(ch == delim)
        {
            *tail++ = '\0';
            break;
        }
        tail++;
    }
    *str = tail;

    return head;
}

static int
OI_Isspi(char ch)
{
    static char tmp[4] = { RS, US, STX, 0 };
    //return (int) strchr(tmp, ch); //by diegojiang at 2011-10-10
    if(strchr(tmp, ch) == NULL)
        return 0;
    else
        return 1;
}

char *
OI_GetNextField(char **str)
{
    char *head, *tail;
    int ch;

    head = *str;
    for(;;)
    {
        ch = *head;
        if(!ch)
        {
            *str = head;
            return head;
        }
        if(!OI_Isspi(ch))
            break;
        head++;
    }
    tail = head + 1;
    while((ch = *tail))
    {
        if(OI_Isspi(ch))
        {
            *tail++ = '\0';
            break;
        }
        tail++;
    }
    *str = tail;
    return head;
}

char *
OI_GetNextField2(char **str, char delim)
{
    char *head, *tail;
    int ch;

    head = *str;
    for(;;)
    {
        ch = *head;
        if(!ch)
        {
            *str = head;
            return head;
        }
        if(ch != delim)
            break;
        head++;
    }
    tail = head + 1;
    while((ch = *tail))
    {
        if(ch == delim)
        {
            *tail++ = '\0';
            break;
        }
        tail++;
    }
    *str = tail;
    return head;
}

#if 0
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
#endif
char *
OI_strrepl(char *Str, size_t BufSiz, char *OldStr, char *NewStr)
{
    int OldLen, NewLen;
    char *p, *q;

    if(NULL == (p = strstr(Str, OldStr)))
        return Str;
    OldLen = strlen(OldStr);
    NewLen = strlen(NewStr);
    if((strlen(Str) + NewLen - OldLen + 1) > BufSiz)
        return NULL;
    q = p + NewLen;
    memmove(q, p + OldLen, strlen(p + OldLen) + 1);
/*    memmove(q = p+NewLen, p+OldLen, strlen(p+OldLen)+1); */
    memcpy(p, NewStr, NewLen);
    return q;
}

int
OI_reformatstr(char buffer[], const int buflen, char *OldStr, char *NewStr)
{
    char *Start, *Str;
    size_t BufLeft;

    Start = buffer;

    for(Str = Start, BufLeft = buflen; Start != NULL;)
    {
        Start = OI_strrepl(Start, BufLeft, OldStr, NewStr);
//          printf("Str:%s OldStr:%s Start:%s NewStr:%s\n",Str,OldStr,Start,NewStr);
        if(Start == Str)
            break;
        BufLeft -= Start - Str;
        Str = Start;
    }

    if(NULL == Start)   /* buf not enough */
        return 0;
    return 1;
}

char *
OI_htmlfilter(const char *in_str, char *out_str, const int out_str_size)
{
    char *filter_str = out_str;

    memset(filter_str, 0, out_str_size);
    snprintf(filter_str, out_str_size, "%s", in_str);

    if(OI_reformatstr(filter_str, out_str_size, "\\", "\\\\") < 1)
    {
        sprintf(out_str, "-");
        return out_str;
    }
    if(OI_reformatstr(filter_str, out_str_size, "'", "\\'") < 1)
    {
        sprintf(out_str, "-");
        return out_str;
    }
    if(OI_reformatstr(filter_str, out_str_size, "\"", "&quot;") < 1)
    {
        sprintf(out_str, "-");
        return out_str;
    }
    return out_str;
}

static int
OI_patmat(const char *pat, const char *str)
{
    switch (*pat)
    {
        case '\0':
            return !*str;

        case '*':
            return OI_patmat(pat + 1, str) || (*str && OI_patmat(pat, str + 1));

        case '?':
            return *str && OI_patmat(pat + 1, str + 1);

        default:
            return (*pat == *str) && OI_patmat(pat + 1, str + 1);
    }
}

int
OI_xstrcmp(const char *pat, const char *str)
{
//      assert(str && pat);
    if(NULL == str || NULL == pat)
        return -1;
    else
        return (OI_patmat(pat, str));
}

static int
OI_patimat(const char *pat, const char *str)
{
    switch (*pat)
    {
        case '\0':
            return !*str;

        case '*':
            return OI_patimat(pat + 1, str) || (*str && OI_patimat(pat, str + 1));

        case '?':
            return *str && OI_patimat(pat + 1, str + 1);

        default:
            return (toupper(*pat) == toupper(*str)) && OI_patimat(pat + 1, str + 1);
    }
}

int
OI_xstricmp(const char *pat, const char *str)
{
//      assert(str && pat);
    if(NULL == str || NULL == pat)
        return -1;
    else
        return (OI_patimat(pat, str));
}

char *
OI_ExtractLeft(char *pszData, char **pptr, char ch)
{
    *pptr = 0;
    if(pszData && ch)
    {
        char *ps = pszData;
        char c = *ps;

        while(c)
        {
            if(c == ch)
            {
                *pptr = pszData;
                *ps = 0;
                return (char *) (ps + 1);
            }
            c = *(++ps);
        }
    }
    return 0;
}

unsigned int
OI_ExtraUin(const char *pszUserID)
{
    const char *ptr = NULL;
    unsigned int iUserID;

    if(strncasecmp(pszUserID, "QQ_", 3) == 0)
        ptr = pszUserID + 3;
    else if(strncasecmp(pszUserID, "OICQ_", 5) == 0)
        ptr = pszUserID + 5;
    else
        return -1;
    iUserID = strtoul(ptr, NULL, 10);
    return iUserID;
}

////////////////////////for icqlib , modify by hunter ///////////////////////////////

u_int32_t
OI_Chars_2_DW(unsigned char *buf)
{
//  return (u_int32_t)((buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0]);
    u_int32_t val;

    memcpy(&val, buf, sizeof(u_int32_t));
    return val;
}

u_int16_t
OI_Chars_2_Word(unsigned char *buf)
{
//  return (u_int16_t)((buf[1] << 8) + buf[0]);
    u_int16_t val;

    memcpy(&val, buf, sizeof(u_int16_t));
    return val;
}

void
OI_DW_2_Chars(unsigned char *buf, u_int32_t num)
{
/*	buf[3] = (u_int8_t)((num >> 24) & 0x000000FF);
	buf[2] = (u_int8_t)((num >> 16) & 0x000000FF);
	buf[1] = (u_int8_t)((num >> 8) & 0x000000FF);
	buf[0] = (u_int8_t)(num & 0x000000FF);*/
    memcpy(buf, &num, sizeof(u_int32_t));
}

void
OI_Word_2_Chars(unsigned char *buf, u_int16_t num)
{
/*	buf[1] = (u_int8_t)((num >> 8) & 0x00FF);
	buf[0] = (u_int8_t)(num & 0x00FF);*/
    memcpy(buf, &num, sizeof(u_int16_t));
}

void
OI_PacketAdvance(unsigned char **buf, int length)
{
    *buf += length;
}

void
OI_PacketAppend32(unsigned char **buf, u_int32_t i)
{
    u_int32_t val = htonl(i);

    OI_DW_2_Chars(*buf, val);
    OI_PacketAdvance(buf, sizeof(u_int32_t));
}

void
OI_PacketAppend32n(unsigned char **buf, u_int32_t i)
{
    u_int32_t val = i;

    OI_DW_2_Chars(*buf, val);
    OI_PacketAdvance(buf, sizeof(u_int32_t));
}

void
OI_PacketAppend16(unsigned char **buf, u_int16_t i)
{
    u_int16_t val = htons(i);

    OI_Word_2_Chars(*buf, val);
    OI_PacketAdvance(buf, sizeof(u_int16_t));
}

void
OI_PacketAppend16n(unsigned char **buf, u_int16_t i)
{
    u_int16_t val = i;

    OI_Word_2_Chars(*buf, val);
    OI_PacketAdvance(buf, sizeof(u_int16_t));
}

void
OI_PacketAppend8(unsigned char **buf, u_int8_t i)
{
    u_int8_t val = i;

    **buf = val;
    OI_PacketAdvance(buf, sizeof(u_int8_t));
}

u_int32_t
OI_PacketRead32(unsigned char **buf)
{
    u_int32_t val;

    val = OI_Chars_2_DW(*buf);
    OI_PacketAdvance(buf, sizeof(u_int32_t));

    return ntohl(val);
}

u_int32_t
OI_PacketRead32n(unsigned char **buf)
{
    u_int32_t val;

    val = OI_Chars_2_DW(*buf);
    OI_PacketAdvance(buf, sizeof(u_int32_t));

    return (val);
}


u_int16_t
OI_PacketRead16(unsigned char **buf)
{
    u_int16_t val;

    val = OI_Chars_2_Word(*buf);
    OI_PacketAdvance(buf, sizeof(u_int16_t));

    return htons(val);
}

u_int16_t
OI_PacketRead16n(unsigned char **buf)
{
    u_int16_t val;

    val = OI_Chars_2_Word(*buf);
    OI_PacketAdvance(buf, sizeof(u_int16_t));

    return val;
}


u_int8_t
OI_PacketRead8(unsigned char **buf)
{
    u_int8_t val;

    val = **buf;
    OI_PacketAdvance(buf, sizeof(u_int8_t));

    return val;
}

//************************************//
void
OI_PacketAppend(unsigned char **buf, const void *data, int length)
{
    memcpy(*buf, data, length);
    OI_PacketAdvance(buf, length);
}

int
OI_PacketAppendString(unsigned char **buf, const char *s)
{
    int length;

    if(s)
    {
        length = strlen(s) + 1;

        OI_PacketAppend16(buf, length);
        OI_PacketAppend(buf, s, length);
    }
    else
    {
        length = 1;
        OI_PacketAppend16(buf, 1);
        OI_PacketAppend8(buf, 0);
    }
    return length;
}

const void *
OI_PacketRead(unsigned char **buf, int length)
{
    const void *data = *buf;

    OI_PacketAdvance(buf, length);

    return data;
}

/* should copy returned string immedity */
const char *
OI_PacketReadString(unsigned char **buf)
{
    int length = OI_PacketRead16(buf);

    return (const char *) OI_PacketRead(buf, length);
}


int
OI_PacketReadStringEx(unsigned char **buf, unsigned char *outbuf, int outbuflen)
{
    int length = OI_PacketRead16(buf), truelength;
    const unsigned char *ptr = (const unsigned char *) OI_PacketRead(buf, length);

    truelength = length > (outbuflen - 1) ? (outbuflen - 1) : length;
    memcpy(outbuf, ptr, truelength);
    return truelength + 2;
}


char *
OI_PacketReadStringNew(unsigned char **buf)
{
    char *ptr;
    int length = OI_PacketRead16(buf);

    ptr = (char *) malloc(length);
    if(!ptr)
        return 0L;
    strncpy(ptr, (char *) OI_PacketRead(buf, length), length);
    return ptr;
}

int
OI_PacketAppendStringFE(unsigned char **buf, const char *s)
{
    int length = 0;

    if(s)
    {
        length = strlen(s);
        OI_PacketAppend(buf, s, length);
    }
    OI_PacketAppend8(buf, 0xFE);
    return length;
}

int
OI_PacketAppendStringCustom(unsigned char **buf, const char *s, char chSpec)
{
    int length = 0;

    assert(s);
    length = strlen(s);
    OI_PacketAppend(buf, s, length);
    OI_PacketAppend8(buf, chSpec);
    return length;
}

int
OI_PacketAppendString0(unsigned char **buf, const char *s)
{
    int length = 0;

    if(s)
    {
        length = strlen(s);
        OI_PacketAppend(buf, s, length);
    }
    OI_PacketAppend8(buf, 0);
    return length;
}

////////////////////////////////////////////////////////////
//有丢信息的嫌疑！！建议用HexDumpStr


char *
OI_HexToStr(const char *psStr, int iLength)
{
    static char szBuffer[255];
    int i;

    if(iLength > 80)
        return "";
    for(i = 0; i < iLength; i++)
        sprintf(szBuffer + i * 2, "%02x", (unsigned char) *(psStr + i));
    return szBuffer;
}

int
OI_StrToHex(const unsigned char *sHexSeed, unsigned char *sSeed, int iSeedLen)
{
    int i;

    bzero(sSeed, iSeedLen);
    for(i = 0; i < iSeedLen; i++)
    {
        if(sHexSeed[2 * i] == 0 || sHexSeed[2 * i + 1] == 0)    // sHexSeed 不够长度
            return -1;

        if(sHexSeed[2 * i] >= '0' && sHexSeed[2 * i] <= '9')
            sSeed[i] += 16 * (sHexSeed[2 * i] - '0');
        else if(sHexSeed[2 * i] >= 'a' && sHexSeed[2 * i] <= 'f')
            sSeed[i] += 16 * (sHexSeed[2 * i] - 'a' + 10);
        else if(sHexSeed[2 * i] >= 'A' && sHexSeed[2 * i] <= 'F')
            sSeed[i] += 16 * (sHexSeed[2 * i] - 'A' + 10);
        else
            return -2;  // sHexSeed 某偶数位出错误

        if(sHexSeed[2 * i + 1] >= '0' && sHexSeed[2 * i + 1] <= '9')
            sSeed[i] += (sHexSeed[2 * i + 1] - '0');
        else if(sHexSeed[2 * i + 1] >= 'a' && sHexSeed[2 * i + 1] <= 'f')
            sSeed[i] += (sHexSeed[2 * i + 1] - 'a' + 10);
        else if(sHexSeed[2 * i + 1] >= 'A' && sHexSeed[2 * i + 1] <= 'F')
            sSeed[i] += (sHexSeed[2 * i + 1] - 'A' + 10);
        else
            return -3;  // sHexSeed 某奇数位出错误
    }
    return 0;
}

void
OI_DropRootPriv(int iUserID, int iGroupID)
{
    if(getgid() == 0)
    {
        if(setgid(iGroupID) < 0)
            perror("can't drop root priv");
    }
    if(getuid() == 0)
    {
        if(setuid(iUserID) < 0)
            perror("can't drop root priv");
    }
    return;
}

const char *
OI_IntToStr(int iValue, char *psBuffer, int iBufferLength)
{
    snprintf(psBuffer, iBufferLength, "%d", iValue);
    return psBuffer;
}

const char *
OI_UIntToStr(int iValue, char *psBuffer, int iBufferLength)
{
    snprintf(psBuffer, iBufferLength, "%u", iValue);
    return psBuffer;
}

char *
OI_GetBlurTimeStr(time_t * mytime)
{
    static char s[50];
    struct tm curr;

    curr = *localtime(mytime);

    sprintf(s, "%02d%02d%02d", curr.tm_mday, curr.tm_hour, curr.tm_min);

    return s;
}

int
OI_CreateFile(char *pszFilePath)
{
    int fd;

    if((fd = open(pszFilePath, O_RDWR | O_CREAT | O_TRUNC, 0644)) == -1)
        return -1;
    close(fd);
    return 0;
}

int
OI_OpenFile(RES_HANDLE * hHandle, char *sFilePath)
{
    hHandle->fp = fopen(sFilePath, "r");
    if(hHandle->fp == NULL)
        return -1;
    return 0;
}

int
OI_LoadString(RES_HANDLE * hHandle, char *sTag, char *sBuffer, int iLength)
{
    char sLine[255], *head, *tail;

    fseek(hHandle->fp, 0L, SEEK_SET);
    while(fgets(sLine, 255, hHandle->fp) != NULL)
    {
        tail = sLine;
        head = OI_nextword(&tail);
        if(strcmp(sTag, head) == 0)
        {
            OI_strtrimcr(tail, " \t\n\r\f\v\"");
            OI_strtrimcl(tail, " \t\n\r\f\v\"");
            snprintf(sBuffer, iLength, "%s", tail);
            return 0;
        }
    }
    printf("no found:[%s]\n", sTag);
    return -1;
}

void
OI_CloseFile(RES_HANDLE * hHandle)
{
    if(hHandle->fp != NULL)
    {
        fclose(hHandle->fp);
        hHandle->fp = NULL;
    }
}



static int iPidHandle;

int OI_CreateFileLock(char *sLockFIle, int *piLockFd)
{
    struct flock lock;
	char sPid[128];
	char sMessage[256] ;
    snprintf(sPid,sizeof(sPid),"   pid:%-20d\n",(int) getpid());

    *piLockFd = open(sLockFIle, O_RDWR | O_CREAT, 0640);
    if(*piLockFd < 0)  /* error occured */
    {
        perror("can't open pid file");
	    return(-1);
    }

    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; /* lock the whole file */

    if(fcntl(*piLockFd , F_SETLK, &lock) < 0)
    {
	    snprintf(sMessage,sizeof(sMessage),"fcntl(.., F_SETLCK, ...) file[%s]   fail: another instant running, I quit.\n",sLockFIle);
        perror(sMessage);
		return(-2);
    }

    /* write pid to the lock file */
    ftruncate(*piLockFd, 0);
    write(*piLockFd, sLockFIle, strlen(sLockFIle));
    write(*piLockFd, sPid, strlen(sPid));

	return(0);
}

void
OI_CreatePIDFile(char *sPIDPath, char *sPIDFile)
{
    char sFilePath[255];
    struct flock lock;

    //int  fd;

    snprintf(sFilePath, sizeof(sFilePath), "%s%s", sPIDPath, sPIDFile);
/*
    FILE *pstFile;
	pstFile = fopen(sFilePath, "w");
    if (pstFile == NULL)
    {
        fprintf(stderr,"Fail to open pid file:%s!\n", sFilePath);
    }
    else
    {
        fprintf(pstFile,"%d",getpid());
        fclose(pstFile);
    }
    */
    /*** prevent other instance from running simultaneously */
    iPidHandle = open(sFilePath, O_RDWR | O_CREAT, 0640);
    if(iPidHandle < 0)  /* error occured */
    {
        perror("can't open pid file");
        exit(1);
    }
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0; /* lock the whole file */

    if(fcntl(iPidHandle, F_SETLK, &lock) < 0)
    {
        perror("fcntl(.., F_SETLCK, ...) fail: another instant running, I quit.\n");
        exit(0);
    }

    /* write pid to the lock file */
    sprintf(sFilePath, "%-20d\n", (int) getpid());
    ftruncate(iPidHandle, 0);
    write(iPidHandle, sFilePath, strlen(sFilePath));
}

void
OI_RemovePIDFile(char *sPIDPath, char *sPIDFile)
{
    char sFilePath[255];

    close(iPidHandle);
    snprintf(sFilePath, sizeof(sFilePath), "%s%s", sPIDPath, sPIDFile);
    unlink(sFilePath);
}

int
OI_HexDumpStr(const char *data, long size, unsigned char *sOutStr)
{
    long i;
    long x;
    int r = 1, offset = 0, len;
    char d[32];
    unsigned char bfr[64];

    for(i = 0, x = 0;; i++)
    {
        if(i < size)
        {
            if(x == 0)
            {
                len = sprintf((char *) &(sOutStr[offset]), "%04lx: ", i);
                offset += len;
            }
            snprintf(d, 9, "%08x", data[i]);
            len = sprintf((char *) &(sOutStr[offset]), "%c%c ", d[6], d[7]);
            offset += len;
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
                len = sprintf((char *) &(sOutStr[offset]), "   ");
                offset += len;
                bfr[x] = ' ';
                r = 0;
            }
        }
        x++;
        if(!(x < 16))
        {
            bfr[x] = 0;
            len = sprintf((char *) &(sOutStr[offset]), "%s\n", bfr);
            offset += len;
            x = 0;
            if(!r)
                break;
        }
    }
    return offset;
}

int
OI_HexDumpStr2(const char *data, long size, unsigned char *sOutStr, int iMaxOutStrLen)
{
    long i;
    long x;
    int r = 1, offset = 0, len;
    char d[32];
    unsigned char bfr[64];

    for(i = 0, x = 0;; i++)
    {
        if(i < size)
        {
            if(x == 0)
            {
                len = snprintf((char *) &(sOutStr[offset]), iMaxOutStrLen - offset - 1, "%04lx: ", i);
                if(len <= 0)
                    break;
                offset += len;
            }
            snprintf(d, 9, "%08x", data[i]);
            len = snprintf((char *) &(sOutStr[offset]), iMaxOutStrLen - offset - 1, "%c%c ", d[6], d[7]);
            if(len <= 0)
                break;
            offset += len;
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
                len = snprintf((char *) &(sOutStr[offset]), iMaxOutStrLen - offset - 1, "   ");
                if(len <= 0)
                    break;
                offset += len;
                bfr[x] = ' ';
                r = 0;
            }
        }
        x++;
        if(!(x < 16))
        {
            bfr[x] = 0;
            len = snprintf((char *) &(sOutStr[offset]), iMaxOutStrLen - offset - 1, "%s\n", bfr);
            if(len <= 0)
                break;
            offset += len;
            x = 0;
            if(!r)
                break;
        }
    }
    return offset;
}

long
OI_genSeed(void)
{
    int fd;
    long value = (long) (time(NULL) + getpid() * 7);
    char buffer[4];

    fd = open("/dev/urandom", O_RDONLY);
    if(fd == -1)
        return value;
    if(read(fd, buffer, 4) != 4)
        goto err;
    close(fd);
    memcpy(&value, buffer, 4);
    return value;
  err:
    close(fd);
    return value;
}

#if 0
char *
OI_randstr(char *buffer, int len)
{
    char *chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
    int i, chars_len;

    chars_len = strlen(chars);
    for(i = 0; i < len; i++)
        buffer[i] = chars[rand() % (chars_len - 1)];
    buffer[len] = '\0';
    return buffer;
}

char *
OI_randstr2(char *buffer, int len)
{
    static char *chars = "abcdefghijklmnopqrstuvwxyz1234567890";
    int i, chars_len;

    chars_len = strlen(chars);
    for(i = 0; i < len; i++)
        buffer[i] = chars[rand() % (chars_len - 1)];
    buffer[len] = '\0';
    return buffer;
}
#endif
static int hash_mult[] = {
    173, 179, 181, 191, 193, 197,
    199, 211, 223, 227, 229, 233,
    239, 241, 251, 257, 263, 269,
    271, 277, 281, 293, 307, 311,
    401, 409, 419, 421, 431, 433,
    439, 443, 449, 457, 461, 463
};

int
OI_hash_name(char *nname)
{
    u_char *name = (u_char *) nname;
    u_char ch;
    int hash = 1, *tab;

    for(tab = hash_mult; (ch = *name); name++, tab++)
        hash += tolower(ch) + *tab + hash;
    if(hash < 0)
        hash = -hash;
    return (hash);
}

char *
OI_transtr(char *instr, char *outstr, int len)
{
    static char buffer[17];
    int hash, chars_len;

    memset(outstr, '0', len);
    hash = OI_hash_name(instr);
    snprintf(buffer, 17, "%d", hash);
    chars_len = strlen(buffer);
    chars_len = MIN(chars_len, len);
    strncpy(outstr, buffer, chars_len);

    outstr[len] = 0;
    return outstr;
}

char *
OI_strtrim(char *str)
{
    char *ibuf, *obuf;

    if(str)
    {
        for(ibuf = obuf = str; *ibuf;)
        {
            while(*ibuf && (isspace(*ibuf)))
                ibuf++;
            if(*ibuf && (obuf != str))
                *(obuf++) = ' ';
            while(*ibuf && (!isspace(*ibuf)))
                *(obuf++) = *(ibuf++);
        }
        *obuf = 0;
    }
    return (str);
}

/* trim the Set char at the string's end */
int
OI_strtrimcr(char *szStr, char *szSet)
{
    int i, j;                   /* Locale counters */

    j = i = strlen(szStr) - 1;  /* Find length of string */

    while(strrchr(szSet, szStr[i]) && (0 <= i))
        szStr[i--] = '\0';                /*- Replace character with '\0' */

    return (j - i); /* Return the difference between old and new length */
}

/* trim the Set char at the string's begin */
int
OI_strtrimcl(char *szStr, char *szSet)
{
    int i = 0, j;

    j = strlen(szStr) - 1;  /* Find length of string */

    while(strrchr(szSet, szStr[i]) && (i <= j))
        i++;                        /*- Count no of removed chars */

    if(0 < i)   /* IF there were matches */
        strcpy(szStr, &szStr[i]);               /*- shift string to the left */

    return (i); /* Return no of matching chars */
}

// only trim the tail char
char *
OI_s_trim(char *str)
{
    char *tail;

    if(!*str)
        return str;
    tail = str + strlen(str) - 1;
    while(tail != str)
    {
        if(isspace(*tail))
            *tail = 0;
        else
            break;
        tail--;
    }
    return str;
}

int g_iStopFlag;
void
OI_sig_handle(int signo)
{
    g_iStopFlag = 1;
    signal(SIGTERM, OI_sig_handle);
}

#ifdef __FreeBSD__
void
OI_sig_handle2(int signo)
{
    waitpid(-1, NULL, WNOHANG);
}
#endif

void
OI_InitDaemon(void)
{
    pid_t pid;

    pid = fork();
    if(pid > 0)
        exit(0);
    else if(pid < 0)
    {
        perror("fail to fork.");
        exit(1);
    }

    /* now the process is the leader of the session, the leader of its
       process grp, and have no controlling tty */
    if(setsid() == -1)
    {
        perror("omail: setsid() fail. It seems impossible.");
        exit(1);
    }
    setpgid(0, 0);

    /*** change umask */
    umask(0027);

    /* ignore the SIGHUP, in case the session leader sending it when terminating.  */
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGURG, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGALRM, SIG_IGN);

#ifdef __FreeBSD__
    signal(SIGCHLD, OI_sig_handle2);
#else
    signal(SIGCHLD, SIG_IGN);
#endif
    signal(SIGTERM, OI_sig_handle);

    pid = fork();
    if(pid > 0)
        exit(0);
    else if(pid < 0)
    {
        perror("fail to fork.");
        exit(1);
    }
    /*** close all inherited file/socket descriptors */
    //for(i = 0; i < getdtablesize(); i ++)
    //  close(i);   
    close(0);
    g_iStopFlag = 0;
}

#if 0
int
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
            return (c > 0xa0 && c < 0xa5);
        case HZ_TJPN:
            return ((c == 0xa4) || (c == 0xa5));
    }
    return 0;
}

int
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
#endif
typedef struct
{
    unsigned char id[3];        // "QC1"
    unsigned char rand[2];
    unsigned char timestamp[4]; // 
    unsigned char port[2];
    unsigned char sum[2];
    unsigned char ip[4];
} QC1;

int
OI_SimpleCrypt(u_int32_t uiIpAddress, u_int16_t unPort, const char *pszChatName, const char *pszRoomName, char *pOutput, int *piOutLength)
{
    char szKey[17] = "48fe90f41d59394d";
    short shSum = 0, shLength, shRand;
    QC1 stInfo;
    time_t now;
    int i;

    shLength = strlen(pszChatName);
    for(i = 0; i < shLength; i++)
        shSum += (unsigned char) pszChatName[i];
    shLength = strlen(pszRoomName);
    for(i = 0; i < shLength; i++)
        shSum += (unsigned char) pszRoomName[i];
    shLength += strlen(pszChatName);

    if(*piOutLength <= (int) sizeof(QC1) * 2 + shLength)
        return -1;

    time(&now);
    shRand = rand() % 0x7fff;
    now += shRand;
    shRand = htons(shRand);
    now = htonl(now);
    shSum = htons(shSum);
    unPort = htons(unPort);

    memcpy(stInfo.rand, &shRand, 2);
    memcpy(stInfo.timestamp, &now, 4);
    memcpy(stInfo.port, &unPort, 2);
    memcpy(stInfo.sum, &shSum, 2);
    memcpy(stInfo.ip, &uiIpAddress, 4);

    stInfo.id[0] = 'Q';
    stInfo.id[1] = 'C';
    stInfo.id[2] = '1';

    stInfo.port[0] = stInfo.port[0] ^ (unsigned char) (stInfo.timestamp[0] + szKey[0]);
    stInfo.port[1] = stInfo.port[1] ^ (unsigned char) (stInfo.timestamp[1] + szKey[1]);
    stInfo.sum[0] = stInfo.sum[0] ^ (unsigned char) (stInfo.timestamp[2] + szKey[2]);
    stInfo.sum[1] = stInfo.sum[1] ^ (unsigned char) (stInfo.timestamp[3] + szKey[3]);
    stInfo.ip[0] = stInfo.ip[0] ^ (unsigned char) (stInfo.timestamp[0] + szKey[4]);
    stInfo.ip[1] = stInfo.ip[1] ^ (unsigned char) (stInfo.timestamp[1] + szKey[5]);
    stInfo.ip[2] = stInfo.ip[2] ^ (unsigned char) (stInfo.timestamp[2] + szKey[6]);
    stInfo.ip[3] = stInfo.ip[3] ^ (unsigned char) (stInfo.timestamp[3] + szKey[7]);

    stInfo.id[0] = stInfo.id[0] ^ (szKey[8] + stInfo.timestamp[0]);
    stInfo.id[1] = stInfo.id[1] ^ (szKey[9] + stInfo.timestamp[2]);
    stInfo.id[2] = stInfo.id[2] ^ (szKey[10] + stInfo.timestamp[3]);
    stInfo.rand[0] = stInfo.rand[0] ^ szKey[11];
    stInfo.rand[1] = stInfo.rand[1] ^ szKey[12];

    for(i = 0; i < 4; i++)
    {
        stInfo.timestamp[i] ^= szKey[i * 4 + 0];
        stInfo.timestamp[i] ^= szKey[i * 4 + 1];
        stInfo.timestamp[i] ^= szKey[i * 4 + 2];
        stInfo.timestamp[i] ^= szKey[i * 4 + 3];
    }

    snprintf(pOutput, *piOutLength, "%s:%s:%s", pszChatName, pszRoomName, OI_HexToStr((char *) &stInfo, sizeof(QC1)));
    *piOutLength = strlen(pOutput);
    return 0;
}

time_t
OI_GetFileTimeStamp(const char *pszFilePath)
{
    struct stat stat_p;

    if(-1 == stat(pszFilePath, &stat_p))
        return 0;
    return stat_p.st_mtime;
}

#if 0
int
OI_GetFileSize(const char *pszFilePath)
{
    struct stat stat_p;

    if(-1 == stat(pszFilePath, &stat_p))
        return -1;
    return stat_p.st_size;
}
#endif
// iInitFlag == 1 : unblock ; iInitFlag == 0 : block ; iInitFlag == 0x2 : create
int
OI_ex_lock_init(EX_LOCK * pstLockInfo, const char *pszLockFilePath, int iInitFlag)
{
    int crflag = (iInitFlag & 0x2) ? O_CREAT : 0;

    snprintf(pstLockInfo->szLockFilePath, sizeof(pstLockInfo->szLockFilePath), "%s", pszLockFilePath);
    pstLockInfo->hHandle = -1;
    pstLockInfo->hHandle = open(pstLockInfo->szLockFilePath, crflag | O_RDONLY, 0644);
    pstLockInfo->iOpFlag = (iInitFlag & 0x1) ? LOCK_NB : 0;
    if(pstLockInfo->hHandle == -1)
        return -1;
    return 0;
}

// operation == 1 : lock ; operation == 0: unlock
int
OI_ex_lock(EX_LOCK * pstLockInfo, int operation)
{
    if(pstLockInfo->hHandle == -1)
    {
        errno = EBADF;
        return -1;
    }
    if(operation)
        return flock(pstLockInfo->hHandle, LOCK_EX | pstLockInfo->iOpFlag);

    return flock(pstLockInfo->hHandle, LOCK_UN | pstLockInfo->iOpFlag);
}

int
OI_initRegModule(REGINFO * pstRegInfo, const char *pszRegexMask)
{
    int err;

    err = regcomp(&(pstRegInfo->stRegEx), pszRegexMask, REG_EXTENDED);
    if(err)
        pstRegInfo->used = 0;
    else
        pstRegInfo->used = 1;

    return pstRegInfo->used;
}

int
OI_isMatchRegMask(REGINFO * pstRegInfo, const char *pszMatchString)
{
    int err;
    regmatch_t subs[10];
    char errbuf[128];

    if(pstRegInfo->used == 0)
        return 0;

    err = regexec(&(pstRegInfo->stRegEx), pszMatchString, 10, subs, 0);
    if(err == REG_NOMATCH)
        return 0;
    else if(err)
    {
        regerror(err, &(pstRegInfo->stRegEx), errbuf, sizeof(errbuf));
        //TLib_Log_LogMsg("reg error:%s[%s]",pszMatchString,errbuf);
        return 0;
    }
    return 1;
}

void
OI_ToupperStr(char *str)
{
    int len = strlen(str);
    int i;

    for(i = 0; i < len; i++)
        str[i] = toupper(str[i]);
}

void
OI_TolowerStr(char *str)
{
    int len = strlen(str);
    int i;

    for(i = 0; i < len; i++)
        str[i] = tolower(str[i]);
}

int
OI_isDigitStr(char *str)
{
    while(*str)
    {
        if(isdigit(*str) == 0)
            return -1;
        str++;
    }
    return 1;
}


// 利用随机抽牌的原理
int
OI_randperm(void *parrMem, size_t nmemb, size_t size)
{
    int i, j;
    char sTemUin[1024];

    if(size > 1024)
        return (-1);

    if(nmemb <= 2)
        return (0);

    for(i = 1 ; i < nmemb ; ++i)
	{
        j = 0 + (int) ((1.0 * ((double) (i+1))) * (rand() / (RAND_MAX + 1.0)));
	}

    for(i = nmemb - 2; i >= 0; --i)
    {
        j = 0 + (int) ((1.0 * ((double) i)) * (rand() / (RAND_MAX + 1.0)));
        memcpy(sTemUin, parrMem + j * size, size);
        memcpy(parrMem + j * size, parrMem + (i + 1) * size, size);
        memcpy(parrMem + (i + 1) * size, sTemUin, size);
    }

    return (0);
}
