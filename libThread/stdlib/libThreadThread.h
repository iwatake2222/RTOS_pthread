#ifndef LIB_THREAD_THREAD_H_
#define LIB_THREAD_THREAD_H_

#include "common.h"
#include "../libThreadConfig.h"
#include "../libThread.h"

class LibThreadThread
{
private:
	static LibThreadThread m_instance;

private:
	LibThreadThread();
	~LibThreadThread();

public:
	static LibThreadThread* getInstance();
	RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle);
	RET joinThread(THREAD_HANDLE threadHandle);
};

#endif /* LIB_THREAD_THREAD_H_ */
