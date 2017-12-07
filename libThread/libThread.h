#ifndef LIB_THREAD_H_
#define LIB_THREAD_H_

#include "common.h"

/**
 * API for initialize / finalize
 */
void initializeLibThread();
void finalizeLibThread();

/**
 * API for thread
 */
typedef void* (*THREAD_ENTRY)(void *arg);
typedef void* THREAD_HANDLE;
typedef struct {
	int dummy;
} THREAD_PARAMS;

RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle);
RET joinThread(THREAD_HANDLE threadHandle);
RET deleteThread(THREAD_HANDLE threadHandle);


/**
 * API for Message
 */
#define MSG_WAIT_FOREVER -1
#define MSG_WAIT_TRY      0
typedef int QUEUE_ID;
typedef void* MSG_HANDLE;

RET createDynamicQueue(QUEUE_ID *queueId);
RET deleteDynamicQueue(QUEUE_ID queueId);
void* allocMessage(int size);	// sender will allocate memory
void freeMessage(MSG_HANDLE msg);	// receiver will free memory
RET sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size);
RET recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs);

/* the number of queue must be less than LIB_THREAD_QUEUE_STATIC_MAX */
typedef enum {
	QUEUE_MODULE_A = 0,
	QUEUE_MODULE_B,
} QUEUE_STATIC;

#endif /* LIB_THREAD_H_ */
