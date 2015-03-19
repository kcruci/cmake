#ifndef _COMM_H_
#define _COMM_H_

#define STX     0x02
#define ETX     0x03
#define RS      0x1e
#define US      0x1f

#define BUFFER_LEN   6000
#define CURR_VERSION_MAIN    01
#define CURR_VERSION_SUB     00



typedef unsigned int WORD32;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned char UCHAR;
typedef int BOOL;

typedef enum
{
	DT_UINT8_T = 1,
	DT_UINT16_T = 2,
	DT_UINT32_T = 3,
	DT_UINT64_T = 4, 
	DT_STRING = 5
}basic_data_type;

#define true 1
#define false 0

#define TRUE 1
#define FALSE 0

#endif
