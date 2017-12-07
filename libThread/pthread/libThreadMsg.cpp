#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "common.h"
#include "../libThreadConfig.h"
#include "../libThread.h"

typedef struct {
	void* message[QUEUE_SIZE];
	int size[QUEUE_SIZE];
	int wp;		// pointer to be written next time
	int rp;		// pointer to be read next time
	bool isAssignedForDynamic;
} QUEUE_STATUS;

static QUEUE_STATUS s_queueStatus[LIB_THREAD_QUEUE_MAX] = {0};

/* should create one mutex to check isAssignedForDynamic, and mutexes for each QUEUE to check wp and rp */
/* share the same mutex for now */
static pthread_mutex_t s_mutex;

void initializeLibThreadMsg()
{
	pthread_mutex_init(&s_mutex, NULL);
}

void finalizeLibThreadMsg()
{
	pthread_mutex_destroy(&s_mutex);
}



RET createDynamicQueue(QUEUE_ID *queueId)
{
	for(int i = LIB_THREAD_QUEUE_STATIC_MAX; i < LIB_THREAD_QUEUE_MAX; i++) {
		pthread_mutex_lock(&s_mutex);
		if(s_queueStatus[i].isAssignedForDynamic == false) {
			s_queueStatus[i].isAssignedForDynamic = true;
			*queueId = i;
			pthread_mutex_unlock(&s_mutex);
			return RET_OK;
		}
		pthread_mutex_unlock(&s_mutex);
	}
	LOG_E("cannot create a dynamic queue\n");
	return RET_ERR_OF;
}

RET deleteDynamicQueue(QUEUE_ID queueId)
{
	if(queueId < LIB_THREAD_QUEUE_STATIC_MAX || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error: queueId = %d\n", queueId);
		return RET_ERR;
	}
	s_queueStatus[queueId].isAssignedForDynamic = false;
	return RET_OK;
}

void* allocMessage(int size)
{
	return malloc(size);
}

void freeMessage(MSG_HANDLE msg)
{
	free(msg);
}

RET sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size)
{
	if(queueId < 0 || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error\n");
		return RET_ERR;
	}

	pthread_mutex_lock(&s_mutex);
	int nextWp = (s_queueStatus[queueId].wp + 1) % QUEUE_SIZE;
	if (nextWp == s_queueStatus[queueId].rp) {
		pthread_mutex_unlock(&s_mutex);
		return RET_ERR_OF;
	}
	s_queueStatus[queueId].message[s_queueStatus[queueId].wp] = msg;
	s_queueStatus[queueId].size[s_queueStatus[queueId].wp] = size;
	s_queueStatus[queueId].wp  = nextWp;
	pthread_mutex_unlock(&s_mutex);
	return RET_OK;
}

RET recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs)
{
	if(queueId < 0 || queueId >= LIB_THREAD_QUEUE_MAX) {
		LOG_E("implementation error\n");
		return RET_ERR;
	}

	clock_t timeStart;
	timeStart = clock();
	double timeElapsed = 1000 * (double)(clock() - timeStart) / CLOCKS_PER_SEC;
	pthread_mutex_lock(&s_mutex);
	do {
		if (s_queueStatus[queueId].rp != s_queueStatus[queueId].wp) break;
		pthread_mutex_unlock(&s_mutex);
		sched_yield();
		timeElapsed = 1000 * (double)(clock() - timeStart) / CLOCKS_PER_SEC;
		pthread_mutex_lock(&s_mutex);
	} while (timeoutMs == MSG_WAIT_FOREVER || timeElapsed < timeoutMs);

	if (s_queueStatus[queueId].rp == s_queueStatus[queueId].wp) {
		pthread_mutex_unlock(&s_mutex);
		return RET_NO_DATA;
	}
	
	*msg  = s_queueStatus[queueId].message[s_queueStatus[queueId].rp];
	*size = s_queueStatus[queueId].size[s_queueStatus[queueId].rp];
	int nextRp = (s_queueStatus[queueId].rp + 1) % QUEUE_SIZE;
	s_queueStatus[queueId].rp  = nextRp;
	pthread_mutex_unlock(&s_mutex);
	return RET_OK;
}
