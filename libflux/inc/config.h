#ifndef CONFIG_H
#define CONFIG_H

#include <flux/arch.h>

/********** GLOBAL OPTIONS **********/

	/* Enable checks on privileged code */
	#define PARANOID

/********** KERNEL OPTIONS **********/

	/***** TASKING SUBSYSTEM *****/

	/* Maximum number of tasks, must be less than 65536 */
	#define MAX_TASKS 1024
	#define MAX_PID   1024

	/***** MEMORY SUBSYSTEM *****/

	/* Two areas of 16 pages for temporary mappings */
	#define TMP_DST 0xFF000000
	#define TMP_SRC 0xFF010000

	#define TMP_MAP	0xFF800000
	#define PGE_MAP	0xFFC00000

	/* Address of task table */
	#define PROCESS_TABLE (KSPACE + 0x400000)
	#define THREAD_TABLE  (KSPACE - 0x400000)

	/* Task image stack */
	#define SSTACK_BSE	(LSPACE - 0x7000)
	#define SSTACK_INI	(LSPACE - 0x2000)
	#define SSTACK_TOP	(LSPACE - 0x2000)

	/* User stack */
	#define USTACK_BSE	(LSPACE - 0x10000)
	#define USTACK_INI	(LSPACE - 0x8010)
	#define USTACK_TOP	(LSPACE - 0x8000)

	/* Exec bootstrap location */
	#define EXEC_STRAP	(SSPACE - 0x100000)

/********** DRIVER OPTIONS **********/

	/***** DEVICE DETECTION *****/

	/* Bus scanning */
	#define SCAN_PCI

	/***** VIDEO SETTINGS *****/
	
	/* Video mode */
	#define VIDEO_TEXT

#endif
