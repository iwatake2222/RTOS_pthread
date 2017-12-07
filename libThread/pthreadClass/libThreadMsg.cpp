#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "libThreadMsg.h"

LibThreadMsg LibThreadMsg::m_instance;

LibThreadMsg::LibThreadMsg()
{
	pthread_mutex_init(&m_mutex, NULL);
	// m_queueStatus = new QUEUE_STATUS[LIB_THREAD_QUEUE_MAX];
	for (int i = 0; i < LIB_THREAD_QUEUE_MAX; i++) memset(&m_queueStatus[i], 0, sizeof(QUEUE_STATUS));
}

LibThreadMsg::~LibThreadMsg()
{
	// delete[] m_queueStatus;
	pthread_mutex_destroy(&m_mutex);
}

LibThreadMsg* LibThreadMsg::getInstance()
{
	return &m_instance;
}

RET LibThreadMsg::createDynamicQueue(QUEUE_ID *queueId)
{
	for(int i = LIB_THREAD_QUEUE_STATIC_MAX; i < LIB_THREAD_QUEUE_MAX; i++) {
		pthread_mutex_lock(&m_mutex);
		if(m_queueStatus[i].isAssignedForDynamic == false) {
			m_queueStatus[i].isAssignedForDynamic = true;
			*queueId = i;
			pthread_mutex_unlock(&m_mutex);
			return RET_OK;
		}
		pthread_mutex_unlock(&m_mutex);
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

	pthread_mutex_lock(&m_mutex);
	int nextWp = (m_queueStatus[queueId].wp + 1) % QUEUE_SIZE;
	if (nextWp == m_queueStatus[queueId].rp) {
		pthread_mutex_unlock(&m_mutex);
		return RET_ERR_OF;
	}
	m_queueStatus[queueId].message[m_queueStatus[queueId].wp] = msg;
	m_queueStatus[queueId].size[m_queueStatus[queueId].wp] = size;
	m_queueStatus[queueId].wp  = nextWp;
	pthread_mutex_unlock(&m_mutex);
	return RET_OK;
}

RET LibThreadMsg::recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs)
{
	if(queueId < 0 || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error\n");
		return RET_ERR;
	}

	clock_t timeStart;
	timeStart = clock();
	double timeElapsed = 1000 * (double)(clock() - timeStart) / CLOCKS_PER_SEC;
	pthread_mutex_lock(&m_mutex);
	do {
		if (m_queueStatus[queueId].rp != m_queueStatus[queueId].wp) break;
		pthread_mutex_unlock(&m_mutex);
		sched_yield();
		timeElapsed = 1000 * (double)(clock() - timeStart) / CLOCKS_PER_SEC;
		pthread_mutex_lock(&m_mutex);
	} while (timeoutMs == MSG_WAIT_FOREVER || timeElapsed < timeoutMs);

	if (m_queueStatus[queueId].rp == m_queueStatus[queueId].wp) {
		pthread_mutex_unlock(&m_mutex);
		return RET_NO_DATA;
	}
	
	*msg  = m_queueStatus[queueId].message[m_queueStatus[queueId].rp];
	*size = m_queueStatus[queueId].size[m_queueStatus[queueId].rp];
	int nextRp = (m_queueStatus[queueId].rp + 1) % QUEUE_SIZE;
	m_queueStatus[queueId].rp  = nextRp;
	pthread_mutex_unlock(&m_mutex);
	return RET_OK;
}
