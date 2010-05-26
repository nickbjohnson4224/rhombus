#ifndef PROC_H
#define PROC_H

#include <flux/abi.h>

#define fork _fork		/* Spawn new process */
#define exit _exit		/* Exit current process */

void block(bool v);		/* (Dis)allow scheduling */
void sleep(void);		/* Relinquish timeslice */

uint32_t getpid(void);

#endif
