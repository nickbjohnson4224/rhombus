#ifndef PROC_H
#define PROC_H

#include <abi.h>

#define fork _fork		/* Spawn new process */

void sleep(void);

uint32_t getpid(void);
uint32_t getppid(void);

#endif
