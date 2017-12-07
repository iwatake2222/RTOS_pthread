#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "../libThreadConfig.h"
#include "../libThread.h"

static pthread_mutex_t s_mutex;
static pthread_t s_thread[LIB_THREAD_THREAD_MAX] = {0};

static int allocateThread()
{
	for (int index = 0; index < LIB_THREAD_THREAD_MAX; index++) {
		pthread_mutex_lock(&s_mutex);
		if(s_thread[index] == 0) {
			s_thread[index] = 1;	// reserve to use
			pthread_mutex_unlock(&s_mutex);
			return index;
		}
		pthread_mutex_unlock(&s_mutex);
	}
	LOG_E("cannot create thread");
	return -1;
}

static RET freeThread(THREAD_HANDLE threadHandle)
{
	for (int index = 0; index < LIB_THREAD_THREAD_MAX; index++) {
		if(&s_thread[index] == threadHandle) {
			s_thread[index] = 0;
			return RET_OK;
		}
	}

	LOG_E("implementation error");
	return RET_ERR;
}

void initializeLibThread()
{
	pthread_mutex_init(&s_mutex, 0);
	extern void initializeLibThreadMsg();
	initializeLibThreadMsg();
}

void finalizeLibThread()
{
	pthread_mutex_destroy(&s_mutex);
	extern void finalizeLibThreadMsg();
	finalizeLibThreadMsg();
}

RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle)
{
	int threadIndex = allocateThread();
	if (threadIndex < 0) return RET_ERR;
	
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_create(&s_thread[threadIndex], &tattr, threadEntry, arg);
	*threadHandle = &s_thread[threadIndex];
	return RET_OK;
}

RET joinThread(THREAD_HANDLE threadHandle)
{
	pthread_join(*(pthread_t*)threadHandle, NULL);
	return RET_OK;
}

RET deleteThread(THREAD_HANDLE threadHandle)
{
	return freeThread(threadHandle);
}

