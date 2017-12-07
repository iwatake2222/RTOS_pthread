#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "libThreadMsg.h"

LibThreadMsg LibThreadMsg::m_instance;

LibThreadMsg::LibThreadMsg()
{
	for (int i = 0; i < LIB_THREAD_QUEUE_MAX; i++) memset(&m_queueStatus[i], 0, sizeof(QUEUE_STATUS));
}

LibThreadMsg::~LibThreadMsg()
{
}

LibThreadMsg* LibThreadMsg::getInstance()
{
	return &m_instance;
}


RET LibThreadMsg::createDynamicQueue(QUEUE_ID *queueId)
{
	for(int i = LIB_THREAD_QUEUE_STATIC_MAX; i < LIB_THREAD_QUEUE_MAX; i++) {
		m_mutex.lock();
		if(m_queueStatus[i].isAssignedForDynamic == false) {
			m_queueStatus[i].isAssignedForDynamic = true;
			*queueId = i;
			m_mutex.unlock();
			return RET_OK;
		}
		m_mutex.unlock();
	}
	LOG_E("cannot create a dynamic queue\n");
	return RET_ERR_OF;
}

RET LibThreadMsg::deleteDynamicQueue(QUEUE_ID queueId)
{
	if(queueId < LIB_THREAD_QUEUE_STATIC_MAX || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error: queueId = %d\n", queueId);
		return RET_ERR;
	}
	m_queueStatus[queueId].isAssignedForDynamic = false;
	return RET_OK;
}

void* LibThreadMsg::allocMessage(int size)
{
	return malloc(size);
}

void LibThreadMsg::freeMessage(MSG_HANDLE msg)
{
	free(msg);
}

RET LibThreadMsg::sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size)
{
	if(queueId < 0 || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error\n");
		return RET_ERR;
	}

	m_mutex.lock();
	int nextWp = (m_queueStatus[queueId].wp + 1) % QUEUE_SIZE;
	if (nextWp == m_queueStatus[queueId].rp) {
		m_mutex.unlock();
		return RET_ERR_OF;
	}
	m_queueStatus[queueId].message[m_queueStatus[queueId].wp] = msg;
	m_queueStatus[queueId].size[m_queueStatus[queueId].wp] = size;
	m_queueStatus[queueId].wp  = nextWp;
	m_mutex.unlock();
	return RET_OK;
}

RET LibThreadMsg::recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs)
{
	if(queueId < 0 || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error\n");
		return RET_ERR;
	}

	std::chrono::system_clock::time_point  timeStart;
	timeStart = std::chrono::system_clock::now();
	int timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timeStart).count();
	m_mutex.lock();
	do {
		if (m_queueStatus[queueId].rp != m_queueStatus[queueId].wp) break;
		m_mutex.unlock();
		std::this_thread::yield();
		timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timeStart).count();
		m_mutex.lock();
	} while (timeoutMs == MSG_WAIT_FOREVER || timeElapsed < timeoutMs);

	if (m_queueStatus[queueId].rp == m_queueStatus[queueId].wp) {
		m_mutex.unlock();
		return RET_NO_DATA;
	}
	
	*msg  = m_queueStatus[queueId].message[m_queueStatus[queueId].rp];
	*size = m_queueStatus[queueId].size[m_queueStatus[queueId].rp];
	int nextRp = (m_queueStatus[queueId].rp + 1) % QUEUE_SIZE;
	m_queueStatus[queueId].rp  = nextRp;
	m_mutex.unlock();
	return RET_OK;
}
