#ifndef CONFIG_H
#define CONFIG_H

/********** KERNEL OPTIONS **********/

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
	//#define MAX_PHMEM_MAX

	#define ESPACE	0xFC000000	// Temporary space for exec data
	#define LSPACE	0xFF000000	// Space for libsys
	#define KSPACE 	0xFF000000	// Kernel space
	#define TMP_MAP	0xFF800000
	#define PGE_MAP	0xFFC00000

	#define TASK_TBL (KSPACE + 0x400000)
	
	#define SIG_TBL (KSPACE - 0x1000)
	#define SIG_MAP (KSPACE - 0x0C00)

	#define SSTACK_BSE	(LSPACE - 0x4000)
	#define SSTACK_INI	(LSPACE - 0x2010)
	#define SSTACK_TOP	(LSPACE - 0x2000)
	#define USTACK_BSE	(LSPACE - 0x10000)
	#define USTACK_INI	(LSPACE - 0x5010)
	#define USTACK_TOP	(LSPACE - 0x5000)
	
	/***** INTERRUPT SUBSYSTEM *****/


/********** INIT OPTIONS **********/

	/***** INITRD *****/

	// Initrd base address
	#define INITRD_BASE 0x10000000

	// Initrd format
	#define INITRD_V7TAR

/********** DRIVER OPTIONS **********/

	/***** DEVICE DETECTION *****/

	// Bus scanning
	#define SCAN_PCI
//	#define SCAN_ISA
	#define SCAN_EISA
//	#define SCAN_AGP
//	#define SCAN_PCIE
	#define SCAN_ATA

	/***** VIDEO SETTINGS *****/
	
	// Video mode
	#define VIDEO_TEXT
//	#define VIDEO_VGA
//	#define VIDEO_VESA

#endif
