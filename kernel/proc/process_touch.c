/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <mem.h>
#include <task.h>

/****************************************************************************
 * process_touch
 *
 * creates a process at the specified pid.
 */

void process_touch(pid_t pid) {
	uintptr_t addr;

	if (pid >= MAX_PID) {
		return;
	}

	addr = PROCESS_TABLE + (PAGESZ * pid);

	if (page_get(addr) & PF_PRES) {
		return;
	}

	page_set(addr, page_fmt(frame_new(), PF_PRES | PF_RW));
}
