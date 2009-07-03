#ifndef CONFIG_H
#define CONFIG_H

/***** TASKING SUBSYSTEM *****/

// Maximum number of tasks, must be less than 65536
#define MAX_TASKS 1024

// Scheduling algorithm
#define SCHED_ROUND_ROBIN 0
#define SCHED_VAR_QUANTUM 1
#define SCHED_ALG SCHED_ROUND_ROBIN

/***** MEMORY SUBSYSTEM *****/

// Maximum physical memory size in megabytes (2^20 bytes)
#define MAX_PHMEM 64

/***** INTERRUPT SUBSYSTEM *****/

// Processor features
//#define SAVE_MMX
//#define SAVE_SSE
//#define SAVE_FPU

#endif
