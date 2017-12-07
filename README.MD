# Simple RTOS / uITRON like system call APIs

## APIs
### Thread
- RET createThread(THREAD_ENTRY threadEntry, const char* const threadName, const THREAD_PARAMS * const threadParams, void* arg, THREAD_HANDLE *threadHandle);
- RET joinThread(THREAD_HANDLE threadHandle);
- RET deleteThread(THREAD_HANDLE threadHandle);

### Message (Queue)
- RET createDynamicQueue(QUEUE_ID *queueId);
- RET deleteDynamicQueue(QUEUE_ID queueId);
- void* allocMessage(int size);
- void freeMessage(MSG_HANDLE msg);
- RET sendMessage(QUEUE_ID queueId, MSG_HANDLE msg, int size);
- RET recvMessage(QUEUE_ID queueId, MSG_HANDLE* msg, int *size, int timeoutMs);

1. Sender module allocates memory for message (allocMessage)
	- Message(e.g. 12 Byte): [OOOOOOOOOOOO]

2. Sender module writes message to the allocated memory
	- Message(e.g. 12 Byte): [XXXXXXXXXXXX]	<- the same address as 1.

3. Sender module sends the message(sendMessage)
	- the pointer to the message(void*) is set to the receiver's Queue

```
Queue
|        |
|        |
|        |
|void* p |  ---> Message(e.g. 12 Byte): [XXXXXXXXXXXX]
---------
```

4. Receiver module gets the pointer from the queue, and refers the message (recvMessage)
5. Receiver module free the memory (freeMessage)

## Options
Three types of implementation
- pthread and C
- pthread and C++ class
- std::thread (C++11(future)) and C++ class
