#ifndef LIB_THREAD_MSG_H_
#define LIB_THREAD_MSG_H_

#include <future>
#include "common.h"
#include "../libThreadConfig.h"
#include "../libThread.h"

class LibThreadMsg
{
private:
	typedef struct {
		void* message[QUEUE_SIZE];
		int size[QUEUE_SIZE];
		int wp;		// pointer to be written next time
		int rp;		// pointer to be read next time
		bool isAssignedForDynamic;
	} QUEUE_STATUS;

private:
	static LibThreadMsg m_instance;
	QUEUE_STATUS        m_queueStatus[LIB_THREAD_QUEUE_MAX];

	/* should create one mutex to check isAssignedForDynamic, and mutexes for each QUEUE to check wp and rp */
	/* share the same mutex for now */
	std::mutex     m_mutex;

private:
	LibThreadMsg();
	~LibThreadMsg();

public:
	static LibThreadMsg* getInstance();
	RET createDynamicQueue(QUEUE_ID *queueId);
	RET deleteDynamicQueue(QUEUE_ID queueId);
	void* allocMessage(int size);
	void freeMessage(MSG_HANDLE msg);
	RET sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size);
	RET recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs);
};

#endif /* LIB_THREAD_MSG_H_ */
