#ifndef LIB_THREAD_CONFIG_H_
#define LIB_THREAD_CONFIG_H_

/**
 * Configuration for thread
 */
#define LIB_THREAD_THREAD_MAX 16

/**
 * Configuration for message
 */
/* the number of queued message (not in Byte) */
/* a queue can contain QUEUE_SIZE message */
/* (each message is pointer(void*), which is allocated by sender using allocMessage()) */
#define QUEUE_SIZE 16	

/* total number of queues */
#define LIB_THREAD_QUEUE_STATIC_MAX 16
#define LIB_THREAD_QUEUE_DYNAMIC_MAX 4
#define LIB_THREAD_QUEUE_MAX (LIB_THREAD_QUEUE_STATIC_MAX + LIB_THREAD_QUEUE_DYNAMIC_MAX)

#endif /* LIB_THREAD_CONFIG_H_ */
