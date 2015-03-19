#ifndef __endecode_base_H_
#define __endecode_base_H_

#include <stdint.h>

int GetINT8(char **p, int *piLen, uint8_t *Value);
int AddINT8(char **p, int *piLenLeft, uint8_t Value);
int GetINT16(char **p, int *piLen, uint16_t *Value);
int AddINT16(char **p, int *piLenLeft, uint16_t Value);
int GetINT32(char **p, int *piLen, uint32_t *Value);
int AddINT32(char **p, int *piLenLeft, uint32_t Value);
int GetINT64(char **p, int *piLen, uint64_t *Value);
int AddINT64(char **p, int *piLenLeft, uint64_t Value);

int GetINT8Array_zero(char **p, int *piLen, uint8_t *Buf, int iBufLen);
int AddINT8Array_zero(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen);
int GetINT16Array_zero(char **p, int *piLen, uint16_t *Buf, int iBufLen);
int AddINT16Array_zero(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen);
int GetINT32Array_zero(char **p, int *piLen, uint32_t *Buf, int iBufLen);
int AddINT32Array_zero(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen);
int GetINT64Array_zero(char **p, int *piLen, uint64_t *Buf, int iBufLen);
int AddINT64Array_zero(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen);

int GetINT8Array_split(char **p, int *piLen, uint8_t *Buf, int iBufLen, char spliter);
int AddINT8Array_split(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen, char spliter);
int GetINT16Array_split(char **p, int *piLen, uint16_t *Buf, int iBufLen, char spliter);
int AddINT16Array_split(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen, char spliter);
int GetINT32Array_split(char **p, int *piLen, uint32_t *Buf, int iBufLen, char spliter);
int AddINT32Array_split(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen, char spliter);
int GetINT64Array_split(char **p, int *piLen, uint64_t *Buf, int iBufLen, char spliter);
int AddINT64Array_split(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen, char spliter);

int GetINT8Array_end(char **p, int *piLen, uint8_t *Buf, int iBufLen);
int AddINT8Array_end(char **p, int *piLenLeft, uint8_t *Buf, int iBufLen);
int GetINT16Array_end(char **p, int *piLen, uint16_t *Buf, int iBufLen);
int AddINT16Array_end(char **p, int *piLenLeft, uint16_t *Buf, int iBufLen);
int GetINT32Array_end(char **p, int *piLen, uint32_t *Buf, int iBufLen);
int AddINT32Array_end(char **p, int *piLenLeft, uint32_t *Buf, int iBufLen);
int GetINT64Array_end(char **p, int *piLen, uint64_t *Buf, int iBufLen);
int AddINT64Array_end(char **p, int *piLenLeft, uint64_t *Buf, int iBufLen);

int Set8AtoIBuf(uint8_t *stTo, char *stFrom);
int Set16AtoIBuf(uint16_t *stTo, char *stFrom);
int Set32AtoIBuf(uint32_t *stTo, char *stFrom);
int Set64AtoIBuf(uint64_t *stTo, char *stFrom);

int StringTo32Array(char *sList, uint32_t adwList[], uint32_t *pdwListLen);


#endif 
