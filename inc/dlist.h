/*
 * double list operation
 */

#ifndef _DLIST_H_
#define _DLIST_H_

/*
 * If you include this file to use double list, your structure must
 * define two points and must be named as "prev" and "next".
 */

#define DLIST_EMPTY(HEAD) ((HEAD)->next == HEAD)

/*
 * This macro can not be used for DLIST_REMOVE.
 * But except when break the 'for' loop after DLIST_REMOVE call.
 */
#define DLIST_FOR_EACH(HEAD, NODE) \
	for (NODE = (HEAD)->next; NODE != HEAD; NODE = (NODE)->next)
		
#define DLIST_INIT(DLIST) \
	do { \
		(DLIST)->next = DLIST; \
		(DLIST)->prev = DLIST; \
	}while(0)
	
#define DLIST_ADD_TAIL(HEAD, NODE) \
	do { \
		(HEAD)->prev->next = NODE; \
		(NODE)->prev = (HEAD)->prev; \
		(NODE)->next = HEAD; \
		(HEAD)->prev = NODE; \
	}while(0)

#define DLIST_ADD_HEAD(HEAD, NODE) \
	do { \
		(HEAD)->next->prev = NODE; \
		(NODE)->next = (HEAD)->next; \
		(NODE)->prev = HEAD; \
		(HEAD)->next = NODE; \
	}while(0)

#define DLIST_INSERT_BEFORE(BASE, NODE) \
	do { \
		(NODE)->next = BASE; \
		(NODE)->prev = (BASE)->prev; \
		(BASE)->prev->next = NODE; \
		(BASE)->prev = NODE; \
	}while(0)

#define DLIST_INSERT_AFTER(BASE, NODE) \
	do { \
		(NODE)->prev = BASE; \
		(NODE)->next = (BASE)->next; \
		(BASE)->next->prev = NODE; \
		(BASE)->next = NODE; \
	}while(0)

#define DLIST_REMOVE_NODE(NODE) \
	do { \
		(NODE)->prev->next = (NODE)->next; \
		(NODE)->next->prev = (NODE)->prev; \
		(NODE)->next = NULL; \
		(NODE)->prev = NULL; \
	}while(0)

#endif
