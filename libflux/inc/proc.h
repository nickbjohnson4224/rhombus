#ifndef PROC_H
#define PROC_H

#include <flux/abi.h>

#define fork _fork		/* Spawn new process */
#define exit _exit		/* Exit current process */
#define pinfo _info		/* Get process information */
#define pctrl _ctrl		/* Set process information */

void block(bool v);		/* (Dis)allow scheduling */
void sleep(void);		/* Relinquish timeslice */

#endif
