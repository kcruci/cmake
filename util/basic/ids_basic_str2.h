#ifndef	_IDS_BASIC_STR2_H_
#define	_IDS_BASIC_STR2_H_

//[in/out]*piLenLeft ��ʾ��p�л�ʣ��Ŀռ䣬����ΪNULL
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif


#define AddChar(p,piLenLeft,cValue)		OI_AddChar(p,piLenLeft,cValue)
#define AddWord(p,piLenLeft,shValue)		OI_AddWord(p,piLenLeft,shValue)
#define AddDWord(p,piLenLeft,lValue)		OI_AddDWord(p,piLenLeft,lValue)
#define AddString(p,piLenLeft,pBuf,shBufLen)	OI_AddString(p,piLenLeft,pBuf,shBufLen)
#define AddString2(p,piLenLeft,pBuf,shBufLen)	OI_AddString2(p,piLenLeft,pBuf,shBufLen)
#define GetBuffer(p,piLen,pBuf,piBufLen)		OI_GetBuffer(p,piLen,pBuf,piBufLen)
#define GetChar(p,piLen,cValue) 				OI_GetChar(p,piLen,cValue)
#define GetDWord(p,piLen,plValue)				OI_GetDWord(p,piLen,plValue)
#define GetString(p,piLen,pBuf,iBufLen)		OI_GetString(p,piLen,pBuf,iBufLen)
#define GetString2(p,piLen,pBuf,iBufLen)		OI_GetString2(p,piLen,pBuf,iBufLen)
#define GetWord(p, piLen,pshValue)				OI_GetWord(p, piLen,pshValue)
#define MyGetToken(sToken,iTokenSize,sString,sSeparator)	OI_MyGetToken(sToken,iTokenSize,sString,sSeparator)
#define Str_Trim(s)							OI_Str_Trim(s)
#define my_bsearch(key,base,nmemb,size,piEqual,compar)		OI_my_bsearch(key,base,nmemb,size,piEqual,compar)
#define AddDDWord(p,piLenLeft,lValue)		OI_AddDDWord(p,piLenLeft,lValue)
#define GetDDWord(p,piLen,plValue)			OI_GetDDWord(p,piLen,plValue)


    int OI_AddChar(char **p, int *piLenLeft, char cValue);
    int OI_AddWord(char **p, int *piLenLeft, unsigned short shValue);
    int OI_AddDWord(char **p, int *piLenLeft, unsigned int iValue);

    // ע��Ϊʲô��֧��һ���ṹ�� ����Ϊ�ṹ�л��� WORD,DWORD��������Ҫʹ�����Լ����ֽ���,Ϊ�˷�ֹ�������Ǹ��ֽ���
    // �ַ�������Ϊ WORD
    int OI_AddString(char **p, int *piLenLeft, char *pBuf, unsigned short shBufLen);
    // �ַ�������Ϊ UChar 
    int OI_AddString2(char **p, int *piLenLeft, char *pBuf, unsigned short shBufLen);

    int OI_GetChar(char **p, int *piLen, char *cValue);
    int OI_GetWord(char **p, int *piLen, unsigned short *pshValue);
    int OI_GetDWord(char **p, int *piLen, unsigned int *piValue);

    //ע������������������������һ������ֵҪ����󳤶ȴ�1(���ڴ�'\0') ,�����ڴ�һ������ȡһ���ṹ
    // �ַ�������Ϊ WORD �����в�'\0'
    int OI_GetString(char **p, int *piLen, char *pBuf, int iBufLen);
    // �ַ�������Ϊ UChar  �����в�'\0'
    int OI_GetString2(char **p, int *piLen, char *pBuf, int iBufLen);
	// ȡbuff   ������, Ҳ�����˺��油 '\0'
    int OI_GetBuffer(char **p, int *piLen, char *pBuf, int *piBufLen);

    char *OI_MyGetToken(char *sToken, int iTokenSize, char *sString, char *sSeparator);
    char *OI_Str_Trim(char *s);
    void *OI_my_bsearch(const void *key, const void *base, size_t nmemb, size_t size, int *piEqual, int (*compar) (const void *, const void *));

	int OI_GetTLV(char **p,int *piLen,int *piTag,int *piValueLen,void *pvalue,int iValueSize);
	int OI_AddTLV(char **p,int * piLeftLen,int iTag,int iLen,void *pvalue);

	//DDWORD���ʹ���
	uint64_t OI_Switch(uint64_t a);

#define htonll(x) OI_Switch((x))
#define ntohll(x) OI_Switch((x))
	int OI_GetDDWord(char ** ppCur, int * piLeftLen, uint64_t * pval);
	int OI_AddDDWord(char ** ppCur, int * piLeftLen, uint64_t val);

    //ʹ�� OI_memcpy ���� memcpy
	void *OI_memcpy(void *dest, size_t dest_size,const void *src, size_t n);
	void *OI_memmove(void *dest, size_t dest_size,const void *src, size_t n);

#ifdef  __cplusplus
}
#endif

#endif
