#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

/* common return code */
typedef int RET;
#define RET_OK       0x00000000
#define RET_NO_DATA  0x00000001
#define RET_TIMEOUT  0x00000002
#define RET_ERR      0x80000001
#define RET_ERR_OF   0x80000002

/* LOG macros */
#define LOG(str, ...) {printf("\x1b[39m"); printf("[%s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__);}
#define LOG_W(str, ...) {printf("\x1b[33m"); printf("[WARNING %s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__);}
#define LOG_E(str, ...) {printf("\x1b[31m"); printf("[ERROR %s:%d] " str, __FILE__, __LINE__, ##__VA_ARGS__);}


#endif /* COMMON_H_ */
