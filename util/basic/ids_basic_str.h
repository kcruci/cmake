#ifndef _IDS_BASIC_STR_H_
#define _IDS_BASIC_STR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define get_val(desc,src)			OI_get_val(desc,src)
#define get_unit(desc,src) 			OI_get_unit(desc,src)
#define getunit(desc,src) 			OI_getunit(desc,src)
#define nGetUnit(sDesc,sSrc,iLen)	OI_nGetUnit(sDesc,sSrc,iLen)
#define nGetVal(sDesc,sSrc,iLen)	OI_nGetVal(sDesc,sSrc,iLen)
#define encode(buffer,toencode)		OI_encode(buffer,toencode)
#define randstr(buffer,len)			OI_randstr(buffer,len)
#define MyQuote(dest,src)			OI_MyQuote(dest,src)
#define HexShow(sStr,iLen,iFlag)	OI_HexShow(sStr,iLen,iFlag)
#define tolower_string(str)			OI_tolower_string(str)
#define toupper_string(str)			OI_toupper_string(str)
#define StrHashByte(sStr,cModule)	OI_StrHashByte(sStr,cModule)
#define IsEmailAddr(sEmail)			OI_IsEmailAddr(sEmail)
#define SkipSpace(sStr)				OI_SkipSpace(sStr)
#define hex_dump(data,size)			OI_hex_dump(data,size)
#define hex_dump_toFd(iFd,data,size)		OI_hex_dump_toFd(iFd,data,size)
#define hex_dump_tofile(pfile,data,size)	OI_hex_dump_tofile(pfile,data,size)
#define DumpPackage(pPkg,iPkgLen) OI_DumpPackage(pPkg,iPkgLen)
#define DumpHex(pMem,uDumpOffset,uDumpLen) OI_DumpHex(pMem,uDumpOffset,uDumpLen)





    char *OI_get_val(char *desc, char *src);
    char *OI_get_unit(char *desc, char *src);
    char *OI_getunit(char *desc, char *src);
    char *OI_nGetUnit(char *sDesc, char *sSrc, int iLen);
    char *OI_nGetVal(char *sDesc, char *sSrc, int iLen);
    char *OI_encode(char *buffer, char *toencode);
    char *OI_randstr(char *buffer, int len);
    char *OI_MyQuote(char *dest, char *src);
    void OI_HexShow(unsigned char *sStr, int iLen, int iFlag);

    char *OI_tolower_string(char *str);
    char *OI_toupper_string(char *str);
//void shiftfiles(char *basename);

    char OI_StrHashByte(char *sStr, unsigned char cModule);

    char *OI_IsEmailAddr(char *sEmail);

    char *OI_SkipSpace(register char *sStr);

    void OI_hex_dump(const char *data, long size);
    void OI_hex_dump_toFd(int iFd, const char *data, long size);
    void OI_hex_dump_tofile(FILE * pfile, const char *data, long size);
    const char *OI_DumpPackage(void *pPkg, int iPkgLen);
    const char *OI_DumpHex(void *pMem, size_t uDumpOffset, size_t uDumpLen);

#ifdef __cplusplus
}
#endif

#endif
