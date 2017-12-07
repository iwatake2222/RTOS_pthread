#include "libThreadThread.h"
#include "libThreadMsg.h"

/**
 * API for initialize / finalize
 */
void initializeLibThread()
{
	// do nothing because initialization is done by constructor
}

void finalizeLibThread()
{
	// do nothing because finalize is done by destructor
}

/**
 * API for thread
 */
RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle)
{
	return LibThreadThread::getInstance()->createThread(threadEntry, threadName, threadParams, arg, threadHandle);
}

RET joinThread(THREAD_HANDLE threadHandle)
{
	return LibThreadThread::getInstance()->joinThread(threadHandle);
}

RET deleteThread(THREAD_HANDLE threadHandle)
{
	return RET_OK;
}


/**
 * API for Message
 */
RET createDynamicQueue(QUEUE_ID *queueId)
{
	return LibThreadMsg::getInstance()->createDynamicQueue(queueId);
}

RET deleteDynamicQueue(QUEUE_ID queueId)
{
	return LibThreadMsg::getInstance()->deleteDynamicQueue(queueId);
}

void* allocMessage(int size)
{
	return LibThreadMsg::getInstance()->allocMessage(size);
}

void freeMessage(MSG_HANDLE msg)
{
	LibThreadMsg::getInstance()->freeMessage(msg);
}

RET sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size)
{
	return LibThreadMsg::getInstance()->sendMessage(queueId, msg, size);
}

RET recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs)
{
	return LibThreadMsg::getInstance()->recvMessage(queueId, msg, size, timeoutMs);
}

