#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <stdlib.h>
#include "ids_basic_log.h"

#define HASH_MAX_ROW 100

typedef struct {
	LogFile *pstLog;
	int iLogLevel;

	void *pTable;
	size_t uNodeSize;
	size_t uRowNum;
	size_t auNodeNums[HASH_MAX_ROW];
	size_t auMods[HASH_MAX_ROW];

	int (*Compare)(const void *pKey, const void *pNode);
} HashTable;

size_t HashTableEvalTableSize(size_t uNodeSize, size_t uRowNum, unsigned auNodeNums[]);

int HashTableInit(HashTable *pstHashTable, LogFile *pstLog, int iLogLevel, void *pTable, size_t uTableSize, size_t uNodeSize, size_t uRowNum, unsigned auNodeNums[], unsigned auMods[], int (*Compare)(const void *pKey, const void *pNode));

void *HashTableSearch(HashTable *pstHashTable, const void *pKey, unsigned uShortKey);
void *HashTableSearchEx(HashTable *pstHashTable, const void *pSearchKey, const void *pEmptyKey, unsigned uShortKey, int *piExist);

int HashTableSearchAll(HashTable *pstHashTable, unsigned uShortKey, int * piCount, void * apAllNodes[]);
int HashTableSearchEmptyN(HashTable *pstHashTable, const void *pEmptyKey, unsigned uShortKey, int * piCount, void * apEmptyNodes[]);
int HashTableSearchN(HashTable *pstHashTable, const void *pSearchKey, unsigned uShortKey, int * piCount, void * apNodes[]);
#endif
