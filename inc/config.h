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

	#define ESPACE	0xFC000000	// Temporary space for exec data
	#define LSPACE	0xFD000000	// Space for libsys
	#define KSPACE 	0xFE000000	// Kernel space
	#define KMALLOC 0xFE200000	// Kernel allocator

	#define TMP_DST 0xFF000000
	#define TMP_SRC 0xFF010000

	#define TMP_MAP	0xFF800000
	#define PGE_MAP	0xFFC00000

	#define TASK_TBL (KSPACE + 0x400000)

	#define SIG_TBL (LSPACE - 0x1000)
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
