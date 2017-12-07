#ifndef LIB_THREAD_THREAD_H_
#define LIB_THREAD_THREAD_H_

#include <pthread.h>
#include "common.h"
#include "../libThreadConfig.h"
#include "../libThread.h"

class LibThreadThread
{
private:
	static LibThreadThread m_instance;
	pthread_mutex_t m_mutex;
	pthread_t       m_thread[LIB_THREAD_THREAD_MAX];

private:
	LibThreadThread();
	~LibThreadThread();
	int allocateThread();
	RET freeThread(THREAD_HANDLE threadHandle);

public:
	static LibThreadThread* getInstance();
	RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle);
	RET joinThread(THREAD_HANDLE threadHandle);
	RET deleteThread(THREAD_HANDLE threadHandle);
};

#endif /* LIB_THREAD_THREAD_H_ */
