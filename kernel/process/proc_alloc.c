/*
 * Copyrught 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/proc.h>
#include <kernel/thread.h>

/* general process space mutex */
uint32_t m_proc = MUTEX_OPEN;

/****************************************************************************
 * proc_alloc
 *
 * This function creates a new process and returns it. The process has a
 * unique identification number. The process structure is within the fixed
 * segment SEGMENT_ADDR_PROC in the kernel address space, at the page that is
 * the lower 16 bits of its process identification number.
 */

proc_t *proc_alloc(void) {
	size_t id;
	proc_t *process;
	frame_t frame;

	/* lock process space, spin */
	while (!mutex_aquire(&m_proc, 1));

	/* locate first empty process space */
	for (id = 0; id < SEGMENT_PAGES; id++) {
		if (!page_check(SEGMENT_ADDR_PROC + PAGESZ * id)) {
			process = (proc_t*) (SEGMENT_ADDR_PROC + PAGESZ * id);
			break;
		}
	}

	/* allocate process structure memory */
	frame = frame_alloc();
	page_set(KSPACE, process, frame | FRAME_READ | FRAME_WRITE | FRAME_SUPER);

	/* set id number */
	process->ctrl.id = id;

	/* clear mutexen */
	process->m_core    = MUTEX_OPEN;
	process->m_threads = MUTEX_OPEN;

	/* unlock process space */
	mutex_release(&m_proc, 1);

	return process;
}
