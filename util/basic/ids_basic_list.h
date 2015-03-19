#ifndef _IDS_BASIC_LIST_H__
#define _IDS_BASIC_LIST_H__

typedef struct _ListHead
{
	struct _ListHead * pstNext;
	struct _ListHead * pstPrev;
}ListHead;

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->pstNext = (ptr); (ptr)->pstPrev = (ptr); \
} while (0)

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
	ListHead name = LIST_HEAD_INIT(name)

#define _LIST_ADD(node, prev, next) do { \
	(next)->pstPrev = (node); \
		(node)->pstNext = (next); \
		(node)->pstPrev = (prev); \
		(prev)->pstNext = (node); \
} while (0)

#define LIST_ADD(node, head) do { \
	ListHead * pstNew = (node); \
		ListHead * pstPrev = (head); \
		ListHead * pstNext = (head)->pstNext; \
		_LIST_ADD(pstNew, pstPrev, pstNext); \
} while (0)

#define LIST_ADD_TAIL(node, head) do { \
	ListHead * pstNew = (node); \
		ListHead * pstPrev = (head)->pstPrev; \
		ListHead * pstNext = (head); \
		_LIST_ADD(pstNew, pstPrev, pstNext); \
} while (0)

#define _LIST_DEL(prev, next) do { \
	(next)->pstPrev = (prev); \
		(prev)->pstNext = (next); \
} while (0)

#define LIST_DEL(entry) do { \
	ListHead * pstPrev = (entry)->pstPrev; \
		ListHead * pstNext = (entry)->pstNext; \
		_LIST_DEL(pstPrev, pstNext); \
} while (0)

#define LIST_DEL_INIT(entry) do { \
	LIST_DEL(entry); \
		INIT_LIST_HEAD(entry); \
} while (0)

#define LIST_EMPTY(head) ((head)->pstNext == (head))

#define LIST_SPLICE(list, head) do { \
	ListHead * first = (list)->pstNext; \
		if (first != (list)) { \
			ListHead * last = (list)->pstPrev; \
				ListHead * at = (head)->pstNext; \
				first->pstPrev = (head); \
				last->pstNext = at; \
				st->pstPrev = last; \
		} \
} while (0)

#define LIST_ENTRY(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define LIST_FOR_EACH(pos, head) \
	for ((pos) = (head)->pstNext; (pos) != (head); pos = (pos)->pstNext)
        	
#define LIST_FOR_EACH_SAFE(pos, n, head) \
	for ((pos) = (head)->pstNext, (n) = (pos)->pstNext; (pos) != (head); \
		(pos) = (n), (n) = (pos)->pstNext)
		
#define LIST_FOR_EACH_PREV(pos, head) \
	for ((pos) = (head)->pstPrev; (pos) != (head); (pos) = (pos)->pstPrev) 	

#endif
