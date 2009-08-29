#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/***** SYSTEM CALLS *****/

// Process creation and control
#define FORK_DEMOTE 1

int32_t fork_call(uint32_t flags);
void exit_call(int32_t value);

// Interprocess communication
#define SINT_BLOCK 1
#define SRET_UNBLOCK_SELF 1
#define SRET_UNBLOCK_CALLER 2

int32_t sint_call(uint32_t target, uint8_t signal, uint32_t arg0,
	uint32_t arg1, uint32_t arg2, uint32_t arg3, uint8_t flags);
void sret_call(uint8_t flags);
uint32_t rsig_call(uint8_t signal, uint32_t handler);
uint32_t lsig_call(uint8_t signal);

// Memory management
#define MMAP_RW 2
#define MMAP_EXEC 0

int32_t mmap_call(uint32_t address, uint32_t size, uint8_t flags);
int32_t umap_call(uint32_t address, uint32_t size);
int32_t mmgc_call(void);

/***** EVENT HANDLING *****/

void waitfor(uint32_t signal);
void block();

#endif
