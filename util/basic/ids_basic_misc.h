#ifndef _IDS_BASIC_MISC_H
#define _IDS_BASIC_MISC_H
#include <stdlib.h>
#include <sys/procfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/param.h>
#include "common.h"

#define OI_MAKEWORD(a, b)      ((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))
#define OI_MAKELONG(a, b)      ((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define OI_LOWORD(l)           ((WORD)(l))
#define OI_HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define OI_LOBYTE(w)           ((BYTE)(w))
#define OI_HIBYTE(w)           ((BYTE)(((WORD)(w) >> 8) & 0xFF))
//���num������[lower, upper]�򷵻�lower ���� upper
#define OI_CHECKBOUND(a, b ,c) MIN(MAX((a),(b)), c)


#ifdef  __cplusplus
extern "C"
{
#endif

#define MsSleep(lMs)						OI_MsSleep(lMs)
#define DaemonInit()			 			OI_DaemonInit()
#define FileCopy(sFile1, sFile2) 			OI_FileCopy(sFile1,sFile2)
#define GetPrimeNumber(iNum) 				OI_GetPrimeNumber(iNum)
#define WritePid(sPidFilePath)	 			OI_WritePid(sPidFilePath)
//#define GetPrpsinfo(pr_pid, prpsinfo) 		OI_GetPrpsinfo(pr_pid,prpsinfo)
#define KillPrevious(sPidFilePath) 			OI_KillPrevious(sPidFilePath)
#define HashToIndexTable(sBaseTableName,c)  OI_HashToIndexTable(sBaseTableName,c)
#define GetUlRandom(lMin, lMax) 		    OI_GetUlRandom(lMin,lMax)

#define  MarkTime(iFlag,fmt,args...)  OI_MarkTime(iFlag,fmt,## args)

// ע�⣺���ɵ�������ķ�ΧΪ [lMin, lMax]��lMax������ڵ���lMin���������޶��塣
//�������� ��ȷ��20ms����select 30ms����usleep
    int OI_MsSleep(unsigned int lMs);

    // ��ǰĿ¼��Ϊ / , ���ҹر�������fd,��������֮ǰ��Ҫ��fd,socket 
	//�����������źŴ��� ���յ�SIGTERM  g_iStopFlag == 1  , ����ѭ�����ж�g_iStopFlag �Ƿ�Ϊ��
    void OI_DaemonInit();  
    int OI_FileCopy(char *sFile1, char *sFile2);    //bitcopy

	//��ʹ�ýű���for i in `seq 100 -1 10` ; do factor $i|awk '{if(NF==2) print $2}' ; done
    unsigned int OI_GetPrimeNumber(unsigned int iNum);    //>= iNum ������

    int OI_WritePid(char *sPidFilePath);    //log pid trival
  //  int OI_GetPrpsinfo(pid_t pr_pid, prpsinfo_t * prpsinfo);    //get ���̵�prpsinfo_t �ṹ
    int OI_KillPrevious(char *sPidFilePath);    //�˲����� WrtitePid���ļ�����

    char *OI_MarkTime(int iFlag, char *sFormat, ...);
    char *OI_HashToIndexTable(char *sBaseTableName, unsigned char c);

// ע�⣺���ɵ�������ķ�ΧΪ [lMin, lMax]��lMax������ڵ���lMin���������޶��塣
    static inline unsigned int OI_GetUlRandom(unsigned int lMin, unsigned int lMax)
    {
        return ((unsigned int) ((double) (lMax - lMin + 1) * rand() / (RAND_MAX + 1.0)) + lMin);
    }

#ifdef  __cplusplus
}
#endif

#endif
