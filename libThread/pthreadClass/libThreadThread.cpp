#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "libThreadThread.h"


LibThreadThread LibThreadThread::m_instance;

LibThreadThread::LibThreadThread()
{
	pthread_mutex_init(&m_mutex, NULL);
	for (int i = 0; i < LIB_THREAD_THREAD_MAX; i++) m_thread[i] = 0;
}

LibThreadThread::~LibThreadThread()
{
	pthread_mutex_destroy(&m_mutex);
}

LibThreadThread* LibThreadThread::getInstance()
{
	return &m_instance;
}

int LibThreadThread::allocateThread()
{
	for (int index = 0; index < LIB_THREAD_THREAD_MAX; index++) {
		pthread_mutex_lock(&m_mutex);
		if(m_thread[index] == 0) {
			m_thread[index] = 1;	// reserve to use
			pthread_mutex_unlock(&m_mutex);
			return index;
		}
		pthread_mutex_unlock(&m_mutex);
	}
	LOG_E("cannot create thread");
	return -1;
}

RET LibThreadThread::freeThread(THREAD_HANDLE threadHandle)
{
	for (int index = 0; index < LIB_THREAD_THREAD_MAX; index++) {
		if(&m_thread[index] == threadHandle) {
			m_thread[index] = 0;
			return RET_OK;
		}
	}

	LOG_E("implementation error");
	return RET_ERR;
}

RET LibThreadThread::createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle)
{
	int threadIndex = allocateThread();
	if (threadIndex < 0) return RET_ERR;
	
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_create(&m_thread[threadIndex], &tattr, threadEntry, arg);
	*threadHandle = &m_thread[threadIndex];
	return RET_OK;
}

RET LibThreadThread::joinThread(THREAD_HANDLE threadHandle)
{
	pthread_join(*(pthread_t*)threadHandle, NULL);
	return RET_OK;
}

RET LibThreadThread::deleteThread(THREAD_HANDLE threadHandle)
{
	return freeThread(threadHandle);
}
