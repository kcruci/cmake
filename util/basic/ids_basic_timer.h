#ifndef __IDS_BASIC_TIMER_H__
#define __IDS_BASIC_TIMER_H__

#include "ids_basic_log.h"

#define CONST_DATA_LEN_TIMER	1
#define NON_CONST_DATA_LEN_TIMER	2

//#define SESSION_KEY 	0x10070555

#ifdef __cplusplus
extern "C" {
#endif
/*
 * get some stat data for operation supporting
 */
//��õ�ǰtimer��������ֵ����ӡ�ɾ����ʱ�䵽��̬���£�Ϊ��Ƭ����
unsigned GetCurTimerNode(void);
//������й��������timer���������ں�������hash�׺�ģ������һ����첽������񣬸�ֵ���������й�
unsigned GetMaxTimerNode(void);
//������й���������session���ݴ�С�����ں�������MaxUserDataֵ��
unsigned GetMaxDataLen(void);
//hash�Ľ׺�ģ���Լ�MaxUserData��ֵ��ֱ��Ӱ�쵽timer������ڴ��С

/*
 * the expire callback function type
 */
typedef void(*ExpireFunc)(unsigned uiDataLen, char * sData);

/*
 * uMaxUserDataSize: max data size for timer
 * uRowNum: number of hash rows
 * auNodeNum: hash base
 * uiCheckIntervalUs: units of uiTimeOut, in usec
 *
 * iLogLevel:
 * 0	silent
 * 1	ERROR_LOG
 * 2	WARN_LOG
 * 3	DEBUG_LOG
 * 4	INFO_LOG
 * 5	TRACE_LOG
 *
 * -NOTE-: iLogLevel is ignored when pstLog is NULL, that is silent
 */
//��ʼ��Timer�������ú����ᶯ̬�����ڴ�
//����ֵ
//0 -- �ɹ���
//С��0 -- ʧ�ܣ� 
//-10 �ڴ治��
//-1 uRowNum��auNodeNum �����Ƿ���uRowNum����Ϊ0��
//-2 uiCheckIntervalUs �����Ƿ�������Ϊ0��
int InitTimer(LogFile * pstLog, int iLogLevel, 
		unsigned uMaxUserDataSize, unsigned uRowNum, unsigned auNodeNum[], 
		unsigned uiCheckIntervalUs, unsigned uiType, int isessionKey);

/*
 * set a timer
 * it will expire after uiCheckIntervalUs * uiTimeOut usecs, if it's not be deleted
 * and it can be modifer the expire time
 *
 * on success, that is ,0 returned, a unique seq output by puiSeq if it's present
 */
//���Timer������
//����ֵ
//0 -- �ɹ�
//-1 -- Timerδ��ʼ����û����InitTimer��
//-2 -- Timer���� -NOTE-: ��Ҫ�ϱ�һ���쳣ID
//-3 -- session����̫�������Ҳ�ϱ�һ���쳣ID
int AddTimer(unsigned *puiSeq, unsigned uiTimeOut, 
		ExpireFunc OnExpire, unsigned uiDataLen, char * sData);

/*
 * change the expire time
 */
//����Timer��ʱʱ�亯��
//����ֵ
//0 -- �ɹ�
//-1 -- Timerδ��ʼ����û����InitTimer��
//-2 -- δ�ҵ�Timer��û������Ѿ���ʱ����
int ModTimer(unsigned uiSeq, unsigned uiTimeOut);

/*
 * get the timer data
 */
//��Timer���session���ݡ�-NOTE-���ú�������ɾ��Timer
//����ֵ
//0 -- �ɹ�
//-1 -- Timerδ��ʼ����û����InitTimer��
//-2 -- δ�ҵ�Timer��û������Ѿ���ʱ����
int GetTimer(unsigned uiSeq, unsigned * puiDataLen, char ** psData);

/*
 * unset a timer
 */
//ɾ��Timer
//����ֵ
//0 -- �ɹ�
//-1 -- Timerδ��ʼ����û����InitTimer��
//1 -- δ�ҵ�Timer��û������Ѿ���ʱ����
int DelTimer(unsigned uiSeq);

/*
 * check timer routine, just called in your main loop
 */
//Timer��������������ѭ���е��ü���
void CheckTimer(void);
#ifdef __cplusplus
}
#endif
#endif/*__OI_TIMER_H__*/
