#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "message.h"
#include "libThread.h"

#ifdef IS_WINDOWS
#include <future>
#endif

void sleepMs(int ms)
{
#ifdef IS_WINDOWS
	// Sleep(ms);
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#else
	usleep(ms*1000);
#endif
}


void* threadFunc1(void* arg)
{
	LOG("enter th1\n");
	bool isExit = false;
	while(!isExit)
	{
		MSG_HANDLE msgHandle;
		int size;
		if(recvMessage(QUEUE_MODULE_A, &msgHandle, &size, MSG_WAIT_FOREVER) == RET_OK){
			LOG("th1: received message\n");
			MESSAGE_PAYLOAD* msg = (MESSAGE_PAYLOAD*)msgHandle;
			switch(msg->header.cmd) {
			case CMD_EXIT:
				isExit = true;
				break;
			case CMD_ACT_PRINT:
				LOG("data = %d %d %d %d\n", msg->print.param0, msg->print.param1, msg->print.param2, msg->print.param3);
				break;
			default:
				LOG_W("unsupported command\n");
				break;
			}
			freeMessage(msgHandle);	// receiver must free message
		} else {
			LOG("th1: timeout\n");
		}
	}
	LOG("exit th1\n");
	return 0;
}

void* threadFunc2(void* arg)
{
	LOG("enter th2\n");
	int queueId = *(int*)arg;
	LOG("thread2 uses queue id : %d\n", queueId);
	bool isExit = false;
	while(!isExit)
	{
		MSG_HANDLE msgHandle;
		int size;
		if(recvMessage(queueId, &msgHandle, &size, MSG_WAIT_FOREVER) == RET_OK){
			LOG("th2: received message\n");
			MESSAGE_PAYLOAD* msg = (MESSAGE_PAYLOAD*)msgHandle;
			switch(msg->header.cmd) {
			case CMD_EXIT:
				isExit = true;
				break;
			case CMD_ACT_PRINT:
				LOG("data = %d %d %d %d\n", msg->print.param0, msg->print.param1, msg->print.param2, msg->print.param3);
				break;
			default:
				LOG_W("unsupported command\n");
				break;
			}
			freeMessage(msgHandle);	// receiver must free message
		} else {
			LOG("th2: timeout\n");
		}
	}
	LOG("exit th2\n");
	return 0;
}

int main()
{
	setbuf(stdout, NULL);
	LOG("Hello\n");

	/* initialize lib */
	initializeLibThread();

	/* Create Dynamic Queue for thread2 */
	int queieId2;
	createDynamicQueue(&queieId2);

	/* Create thread1 and thread2*/
	THREAD_HANDLE th1;
	createThread(threadFunc1, "TH1", NULL, NULL, &th1);
	THREAD_HANDLE th2;
	createThread(threadFunc2, "TH2", NULL, &queieId2, &th2);

	/* Send messages to thread 1 */
	for(int i = 0; i < 10; i++) {
		MESSAGE_PAYLOAD_PRINT* msg = (MESSAGE_PAYLOAD_PRINT*)allocMessage(sizeof(MESSAGE_PAYLOAD_PRINT));
		memset(msg, 0, sizeof(MESSAGE_PAYLOAD_PRINT));
		msg->cmd = CMD_ACT_PRINT;
		msg->param0 = i + 100;
		if (sendMessage(QUEUE_MODULE_A, (MSG_HANDLE)msg, sizeof(MESSAGE_PAYLOAD_PRINT)) != RET_OK) {
			LOG_W("failed to send message\n");
			freeMessage((MSG_HANDLE)msg);	// free message by my self when send failed
		}
		sleepMs(100);
	}

	/* Send messages to thread 2 */
	for(int i = 0; i < 10; i++) {
		MESSAGE_PAYLOAD_PRINT* msg = (MESSAGE_PAYLOAD_PRINT*)allocMessage(sizeof(MESSAGE_PAYLOAD_PRINT));
		memset(msg, 0, sizeof(MESSAGE_PAYLOAD_PRINT));
		msg->cmd = CMD_ACT_PRINT;
		msg->param0 = i;
		if (sendMessage(queieId2, (MSG_HANDLE)msg, sizeof(MESSAGE_PAYLOAD_PRINT)) != RET_OK) {
			LOG_W("failed to send message\n");
			freeMessage((MSG_HANDLE)msg);	// free message by my self when send failed
		}
		sleepMs(100);
	}

	/* exit threads */
	MESSAGE_PAYLOAD_HEADER* msg = (MESSAGE_PAYLOAD_HEADER*)allocMessage(sizeof(MESSAGE_PAYLOAD_HEADER));
	msg->cmd = CMD_EXIT;
	sendMessage(QUEUE_MODULE_A, (MSG_HANDLE)msg, sizeof(MESSAGE_PAYLOAD_HEADER));
	msg = (MESSAGE_PAYLOAD_HEADER*)allocMessage(sizeof(MESSAGE_PAYLOAD_HEADER));
	msg->cmd = CMD_EXIT;
	sendMessage(queieId2, (MSG_HANDLE)msg, sizeof(MESSAGE_PAYLOAD_HEADER));
	
	/* wait until thread finished */
	joinThread(th1);
	joinThread(th2);

	/* release resources */
	deleteThread(th1);
	deleteThread(th2);
	deleteDynamicQueue(queieId2);

	/* finalize lib */
	finalizeLibThread();

	return 0;
}
