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

#define LSPACE	0xF8000000
#define KSPACE 	0xFF000000
#define TMP_MAP	0xFF800000
#define PGE_MAP	0xFFC00000

#define TASK_TBL (KSPACE + 0x400000)

#define SIG_TBL (LSPACE - 0x1000)
#define SOV_TBL (LSPACE + 0x0000)
#define SSTACK_BSE	(LSPACE - 0x4000)
#define SSTACK_INI	(LSPACE - 0x2004)
#define SSTACK_TOP	(LSPACE - 0x2000)
#define USTACK_BSE	(LSPACE - 0x10000)
#define USTACK_INI	(LSPACE - 0x5004)
#define USTACK_TOP	(LSPACE - 0x5000)

/***** INTERRUPT SUBSYSTEM *****/

// Processor features

//*	// Erase first slash to toggle
//#define ENABLE_FXSAVE
///*/
//#define ENABLE_FSAVE
//*/

#endif
