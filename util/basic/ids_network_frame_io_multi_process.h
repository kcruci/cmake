#ifndef __IDS_NETWORK_FRAME_IO_MULTI_PROCESS_H_
#define __IDS_NETWORK_FRAME_IO_MULTI_PROCESS_H_

#include "ids_basic_log.h"

#define IDS_IOMP_FLAG_READ 0x1
#define IDS_IOMP_FLAG_WRITE 0x2
#define IDS_IOMP_FLAG_ERROR 0x4

struct _ids_io_multi_process_mng;
typedef struct _ids_io_multi_process_mng ids_io_multi_process_mng;

ids_io_multi_process_mng *ids_iomp_mng_init(LogFile * pstLog, int iLogLevel, int iMaxSocketNum);

int ids_iomp_mng_add_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd, int iFlag);

int ids_iomp_mng_mod_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd, int iFlag);

int ids_iomp_mng_del_fd(ids_io_multi_process_mng * pstIOMPMng, int iFd);

int ids_iomp_mng_wait_for_fd(ids_io_multi_process_mng * pstIOMPMng, int iTimeoutMSec);

int ids_iomp_mng_get_ready_fd(ids_io_multi_process_mng * pstIOMPMng, int *piPos, int *piFlag);

#endif
