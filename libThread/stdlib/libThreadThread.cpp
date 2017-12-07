#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <future>
#include "libThreadThread.h"


LibThreadThread LibThreadThread::m_instance;

LibThreadThread::LibThreadThread()
{
}

LibThreadThread::~LibThreadThread()
{
}

LibThreadThread* LibThreadThread::getInstance()
{
	return &m_instance;
}


RET LibThreadThread::createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle)
{
	*threadHandle = (THREAD_HANDLE)(new std::thread(threadEntry, arg));
	return RET_OK;
}

RET LibThreadThread::joinThread(THREAD_HANDLE threadHandle)
{
	std::thread* thread = (std::thread*)threadHandle;
	if (thread->joinable()) thread->join();
	return RET_OK;
}

