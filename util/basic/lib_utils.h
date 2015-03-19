#ifndef _LIB_UTILS_H_
#define _LIB_UTILS_H_

#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifdef  __cplusplus
extern "C"
{
#endif


#define __make_section_unallocated(section_string)   \
 __asm__ (".pushsection " section_string "\n\t.popsection");

#define link_warning(symbol, msg) \
 __make_section_unallocated (".gnu.warning." #symbol) \
 static const char __evoke_link_warning_##symbol[]     \
   __attribute__ ((section (".gnu.warning." #symbol "\n\t#"))) = msg;



#define HZ_GB 0
#define HZ_BIG5 1
#define HZ_JPN 2
#define HZ_GBK 3
#define HZ_TGB 4
#define HZ_TSIGN 5
#define HZ_TJPN 7


//#define MIN(a,b) (((a)<(b))?(a):(b))
//#define MAX(a,b) (((a)>(b))?(a):(b))


#define dim(x) (sizeof(x)/sizeof(x[0]))

    typedef struct
    {
        regex_t stRegEx;
        int used;
    } REGINFO;

    typedef struct
    {
        char szLockFilePath[255];
        int hHandle;
        int iOpFlag;

    } EX_LOCK;

    typedef struct
    {
        FILE *fp;
    } RES_HANDLE;

#define SimpleCrypt(uiIpAddress,unPort,pszChatName,pszRoomName,pOutput,piOutLength) \
	    OI_SimpleCrypt(uiIpAddress,unPort,pszChatName,pszRoomName,pOutput,piOutLength)
#define ExtractLeft(pszData,pptr,ch)                        OI_ExtractLeft(pszData,pptr,ch)
#define GetBlurTimeStr(mytime)                              OI_GetBlurTimeStr(mytime)
#define GetNextField(str)                                   OI_GetNextField(str)
#define GetNextField2(str,delim)                            OI_GetNextField2(str,delim)
#define HexToStr(psStr,iLength)                             OI_HexToStr(psStr,iLength)
#define PacketReadStringNew(buf)                            OI_PacketReadStringNew(buf)
#define IntToStr(iValue,psBuffer,iBufferLength)             OI_IntToStr(iValue,psBuffer,iBufferLength)
#define UIntToStr(iValue,psBuffer,iBufferLength)            OI_UIntToStr(iValue,psBuffer,iBufferLength)
#define nextfield(str,delim)                                OI_nextfield(str,delim)
#define nextword(str)                                       OI_nextword(str)
#define randstr(buffer,len)                                 OI_randstr(buffer,len)
#define randstr2(buffer,len)                                OI_randstr2(buffer,len)
#define s_trim(str)                                         OI_s_trim(str)
#define strrepl(Str,BufSiz,OldStr,NewStr)                   OI_strrepl(Str,BufSiz,OldStr,NewStr)
#define strtrim(str)                                        OI_strtrim(str)
#define transtr(instr,outstr,len)                           OI_transtr(instr,outstr,len)
#define PacketReadString(buf)                               OI_PacketReadString(buf)
#define PacketRead(buf,length)                              OI_PacketRead(buf,length)
#define Atoi(str)                                           OI_Atoi(str)
#define CreateFile(pszFilePath)                             OI_CreateFile(pszFilePath)
#define ExtraUin(pszUserID)                                 OI_ExtraUin(pszUserID)
//#define GetInterFaceIP(puiIPAddr)                           OI_GetInterFaceIP(puiIPAddr)
#define HexDumpStr(data,size,sOutStr)                       OI_HexDumpStr(data,size,sOutStr)
#define ISDBCSLEAD(CodeType,c)                              OI_ISDBCSLEAD(CodeType,c)
#define ISDBCSNEXT(CodeType,c)                              OI_ISDBCSNEXT(CodeType,c)
#define LoadString(hHandle,sTag,sBuffer,iLength)            OI_LoadString(hHandle,sTag,sBuffer,iLength)
#define OpenFile(hHandle,sFilePath)                         OI_OpenFile(hHandle,sFilePath)
#define PacketAppendString(buf,s)                           OI_PacketAppendString(buf,s)
#define PacketAppendString0(buf,s)                          OI_PacketAppendString0(buf,s)
#define PacketAppendStringFE(buf,s)                         OI_PacketAppendStringFE(buf,s)
#define PacketAppendStringCustom(buf,s,chSpec)              OI_PacketAppendStringCustom(buf,s,chSpec)
#define PacketReadStringEx(buf,outbuf,outbuflen)            OI_PacketReadStringEx(buf,outbuf,outbuflen)
#define ex_lock(pstLockInfo,operation)                      OI_ex_lock(pstLockInfo,operation)
#define ex_lock_init(pstLockInfo,pszLockFilePath,iInitFlag) OI_ex_lock_init(pstLockInfo,pszLockFilePath,iInitFlag)
#define hash_name(nname)                                    OI_hash_name(nname)
#define initRegModule(pstRegInfo,pszRegexMask)              OI_initRegModule(pstRegInfo,pszRegexMask)
#define isDigitStr(str)                                     OI_isDigitStr(str)
#define isMatchRegMask(pstRegInfo,pszMatchString)           OI_isMatchRegMask(pstRegInfo,pszMatchString)
#define reformatstr(buffer,buflen,OldStr,NewStr)            OI_reformatstr(buffer,buflen,OldStr,NewStr)
#define htmlfilter(in_str,out_str,out_str_size)             OI_htmlfilter(in_str,out_str,out_str_size)
#define strtrimcl(szStr,szSet)                              OI_strtrimcl(szStr,szSet)
#define strtrimcr(szStr,szSet)                              OI_strtrimcr(szStr,szSet)
#define xstrcmp(pat,str)                                    OI_xstrcmp(pat,str)
#define xstricmp(pat,str)                                   OI_xstricmp(pat,str)
#define Atol(str)                                           OI_Atol(str)
#define genSeed()                                           OI_genSeed()
#define GetFileTimeStamp(pszFilePath)                       OI_GetFileTimeStamp(pszFilePath)
#define GetFileSize(pszFilePath)                            OI_GetFileSize(pszFilePath)
#define Chars_2_Word(buf)                                   OI_Chars_2_Word(buf)
#define PacketRead16(buf)                                   OI_PacketRead16(buf)
#define PacketRead16n(buf)                                  OI_PacketRead16n(buf)
#define Chars_2_DW(buf)                                     OI_Chars_2_DW(buf)
#define PacketRead32(buf)                                   OI_PacketRead32(buf)
#define PacketRead32n(buf)                                  OI_PacketRead32n(buf)
#define PacketRead8(buf)                                    OI_PacketRead8(buf)
#define CloseFile(hHandle)                                  OI_CloseFile(hHandle)
#define CreatePIDFile(sPIDPath,sPIDFile)                    OI_CreatePIDFile(sPIDPath,sPIDFile)
#define DW_2_Chars(buf,num)                                 OI_DW_2_Chars(buf,num)
#define DropRootPriv(iUserID,iGroupID)                      OI_DropRootPriv(iUserID,iGroupID)
#define InitDaemon()                                        OI_InitDaemon()
#define PacketAdvance(buf,length)                           OI_PacketAdvance(buf,length)
#define PacketAppend(buf,data,length)                       OI_PacketAppend(buf,data,length)
#define PacketAppend16(buf,i)                               OI_PacketAppend16(buf,i)
#define PacketAppend16n(buf,i)                              OI_PacketAppend16n(buf,i)
#define PacketAppend32(buf,i)                               OI_PacketAppend32(buf,i)
#define PacketAppend32n(buf,i)                              OI_PacketAppend32n(buf,i)
#define PacketAppend8(buf,i)                                OI_PacketAppend8(buf,i)
#define RemovePIDFile(sPIDPath,sPIDFile)                    OI_RemovePIDFile(sPIDPath,sPIDFile)
#define ToupperStr(str)                                     OI_ToupperStr(str)
#define TolowerStr(str)                                     OI_TolowerStr(str)
#define Word_2_Chars(buf,num)                               OI_Word_2_Chars(buf,num)
#define sig_handle(signo)                                   OI_sig_handle(signo)
#define hightbit(c)                                         OI_hightbit(c)
#define lowbit(c)                                           OI_lowbit(c)
#define Isspace(ch)                                         OI_Isspace(ch)
#define ishtmlalpha(ch)                                     OI_ishtmlalpha(ch)
#define isUnPrintableChnChar(first,next)                    OI_isUnPrintableChnChar(first,next)
#define convGbAlphaToEng(first,next)                        OI_convGbAlphaToEng(first,next)
#define isGbAlpha(first,next)                               OI_isGbAlpha(first,next)
#define isGbDigit(first,next)                               OI_isGbDigit(first,next)
#define convGbDigitToEng(first,next)                        OI_convGbDigitToEng(first,next)
#define convGbUrlCharToEng(first,next)                      OI_convGbUrlCharToEng(first,next)
#define isGbUrlChar(first,next)                             OI_isGbUrlChar(first,next)
#define isUrlChar(ch)                                       OI_isUrlChar(ch)
#define getIntParam(str)                                    OI_getIntParam(str)
#define getUIntParam(str)                                   OI_getUIntParam(str)

    char *OI_ExtractLeft(char *pszData, char **pptr, char ch);
    char *OI_GetBlurTimeStr(time_t * mytime);
    char *OI_GetNextField(char **str);
    char *OI_GetNextField2(char **str, char delim);
    //输入二进制，返回ascii，最大只返回前80字节的信息
    char *OI_HexToStr(const char *psStr, int iLength);
    //输入ascii字符串，返回二进制串
    int OI_StrToHex(const unsigned char *sHexSeed, unsigned char *sSeed, int iSeedLen);
    char *OI_PacketReadStringNew(unsigned char **buf);
    const char *OI_IntToStr(int iValue, char *psBuffer, int iBufferLength);
    const char *OI_UIntToStr(int iValue, char *psBuffer, int iBufferLength);
    char *OI_nextfield(char **str, char delim);
    char *OI_nextword(char **str);
    char *OI_randstr(char *buffer, int len);
    char *OI_randstr2(char *buffer, int len);
    char *OI_s_trim(char *str);
    char *OI_strrepl(char *Str, size_t BufSiz, char *OldStr, char *NewStr);
    char *OI_strtrim(char *str);
    char *OI_transtr(char *instr, char *outstr, int len);
    const char *OI_PacketReadString(unsigned char **buf);
    const void *OI_PacketRead(unsigned char **buf, int length);
    int OI_Atoi(char *str);
    int OI_CreateFile(char *pszFilePath);
    unsigned int OI_ExtraUin(const char *pszUserID);
    //int OI_GetInterFaceIP(u_int32_t * puiIPAddr); //请使用 lib_net.h的  OI_getlocalIPList
//OutStr起码是输入的4倍!
    int OI_HexDumpStr(const char *data, long size, unsigned char *sOutStr);
    int OI_HexDumpStr2(const char *data, long size, unsigned char *sOutStr, int iMaxOutStrLen);
    int OI_ISDBCSLEAD(int CodeType, unsigned char c);
    int OI_ISDBCSNEXT(int CodeType, unsigned char c);
    int OI_LoadString(RES_HANDLE * hHandle, char *sTag, char *sBuffer, int iLength);
    int OI_OpenFile(RES_HANDLE * hHandle, char *sFilePath);
    int OI_PacketAppendString(unsigned char **buf, const char *s);
    int OI_PacketAppendString0(unsigned char **buf, const char *s);
    int OI_PacketAppendStringFE(unsigned char **buf, const char *s);
    int OI_PacketAppendStringCustom(unsigned char **buf, const char *s, char chSpec);
    int OI_PacketReadStringEx(unsigned char **buf, unsigned char *outbuf, int outbuflen);
    int OI_SimpleCrypt(u_int32_t uiIpAddress, u_int16_t unPort, const char *pszChatName, const char *pszRoomName, char *pOutput, int *piOutLength);
    int OI_ex_lock(EX_LOCK * pstLockInfo, int operation);
    int OI_ex_lock_init(EX_LOCK * pstLockInfo, const char *pszLockFilePath, int iInitFlag);
    int OI_hash_name(char *nname);
    int OI_initRegModule(REGINFO * pstRegInfo, const char *pszRegexMask);
    int OI_isDigitStr(char *str);
    int OI_isMatchRegMask(REGINFO * pstRegInfo, const char *pszMatchString);
    int OI_reformatstr(char buffer[], const int buflen, char *OldStr, char *NewStr);
    char *OI_htmlfilter(const char *in_str, char *out_str, const int out_str_size);
    int OI_strtrimcl(char *szStr, char *szSet);
    int OI_strtrimcr(char *szStr, char *szSet);
    int OI_xstrcmp(const char *pat, const char *str);
    int OI_xstricmp(const char *pat, const char *str);
    long OI_Atol(char *str);
    long OI_genSeed(void);
    time_t OI_GetFileTimeStamp(const char *pszFilePath);
//不能拿来stat > 2G的文件
    int OI_GetFileSize(const char *pszFilePath);
    u_int16_t OI_Chars_2_Word(unsigned char *buf);
    u_int16_t OI_PacketRead16(unsigned char **buf);
    u_int16_t OI_PacketRead16n(unsigned char **buf);
    u_int32_t OI_Chars_2_DW(unsigned char *buf);
    u_int32_t OI_PacketRead32(unsigned char **buf);
    u_int32_t OI_PacketRead32n(unsigned char **buf);
    u_int8_t OI_PacketRead8(unsigned char **buf);
    void OI_CloseFile(RES_HANDLE * hHandle);
    void OI_CreatePIDFile(char *sPIDPath, char *sPIDFile);
	int  OI_CreateFileLock(char *sLockFIle, int *piLockFd)  ;
    void OI_DW_2_Chars(unsigned char *buf, u_int32_t num);
    void OI_DropRootPriv(int iUserID, int iGroupID);
//有丢信息的嫌疑！！建议用HexDumpStr
//sizeof OutStr > 3*iLen
    void OI_InitDaemon(void);   // 设置了信号处理 接收到SIGTERM  g_iStopFlag == 1  , 在主循环中判断g_iStopFlag 是否为真
    void OI_PacketAdvance(unsigned char **buf, int length);
    void OI_PacketAppend(unsigned char **buf, const void *data, int length);
    void OI_PacketAppend16(unsigned char **buf, u_int16_t i);
    void OI_PacketAppend16n(unsigned char **buf, u_int16_t i);
    void OI_PacketAppend32(unsigned char **buf, u_int32_t i);
    void OI_PacketAppend32n(unsigned char **buf, u_int32_t i);
    void OI_PacketAppend8(unsigned char **buf, u_int8_t i);
    void OI_RemovePIDFile(char *sPIDPath, char *sPIDFile);
    void OI_ToupperStr(char *str);
    void OI_TolowerStr(char *str);
    void OI_Word_2_Chars(unsigned char *buf, u_int16_t num);
//void hex_dump(const char *data, long size);
    void OI_sig_handle(int signo);

    int OI_randperm(void *parrMem, size_t nmemb, size_t size);

    static inline char OI_hightbit(char c)
    {
        return ((c >> 4) & 0x0f);
    }

    static inline char OI_lowbit(char c)
    {
        return (c & 0x0f);
    }

    static inline int OI_Isspace(unsigned char ch)
    {
        return (int) (memchr(" \t\n\r", ch, 4) != NULL);
    }

    static inline int OI_ishtmlalpha(unsigned char ch)
    {
        return (int) (memchr("<>&:/\\", ch, 6) != NULL);
    }

    static inline int OI_isUnPrintableChnChar(unsigned char first, unsigned char next)
    {
        if((first == 0xd7) && (next > 0xf9))
            return 1;
        return 0;
    }

    static inline unsigned char OI_convGbAlphaToEng(unsigned char first, unsigned char next)
    {
        return (next - 0x80);
    }

    static inline int OI_isGbAlpha(unsigned char first, unsigned char next)
    {
        if((first == 0xa3) && ((next >= 0xc1 && next <= 0xda) || (next >= 0xe1 && next <= 0xfa)))
            return 1;
        return 0;
    }

    static inline int OI_isGbDigit(unsigned char first, unsigned char next)
    {
        if((first == 0xa3) && (next >= 0xb0 && next <= 0xb9))
            return 1;
        return 0;
    }

    static inline unsigned char OI_convGbDigitToEng(unsigned char first, unsigned char next)
    {
        return (next - 0x80);
    }

    static inline unsigned char OI_convGbUrlCharToEng(unsigned char first, unsigned char next)
    {
        unsigned char ret = 0;

        if(first == 0xa1 && next == 0xa3)
            ret = '.';
        else if(first == 0xa1 && next == 0xc3)
            ret = ':';
        else if(first == 0xa1 && next == 0xaf)
            ret = '/';
        return ret;
    }

    static inline int OI_isGbUrlChar(unsigned char first, unsigned char next)
    {
        if(first == 0xa1 && next == 0xa3)
            return 1;
        else if(first == 0xa1 && next == 0xc3)
            return 1;
        else if(first == 0xa1 && next == 0xaf)
            return 1;
        return 0;
    }

    static inline int OI_isUrlChar(unsigned char ch)
    {
        if(ch == '.' || ch == ':' || ch == '/')
            return 1;
        return 0;
    }

    static inline int OI_getIntParam(char **str)
    {
        char *psTmp = OI_nextword(str);

        if(!*psTmp)
            return -1;
        return atoi(psTmp);
    }

    static inline u_int32_t OI_getUIntParam(char **str)
    {
        char *psTmp = OI_nextword(str);

        if(!*psTmp)
            return (u_int32_t) - 1;
        return strtoul(psTmp, NULL, 10);
    }

#ifdef  __cplusplus
}
#endif


#endif
