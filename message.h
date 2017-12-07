#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "common.h"

/**
 * this is the sample of message payload and command
 * each module(thread) should have its own header file to define its API
 * You may use the common message.h for all modules if working small/medium project
 */

/**
 * Command
 */
#define CMD_ACT_PRINT 0
#define CMD_ACT_XXX   1
#define CMD_ACT_YYY   2
#define CMD_EXIT     -1


/**
 * Payload (Message structure)
 */

/* payloads for each command defined */
typedef struct {
	uint32_t cmd;
	uint32_t param0;
	uint32_t param1;
	uint32_t param2;
	uint32_t param3;
} MESSAGE_PAYLOAD_PRINT;

/* common type */
typedef struct {
	uint32_t cmd;
} MESSAGE_PAYLOAD_HEADER;

typedef union {
	MESSAGE_PAYLOAD_HEADER header;
	MESSAGE_PAYLOAD_PRINT print;
} MESSAGE_PAYLOAD;

#endif /* MESSAGE_H_ */
