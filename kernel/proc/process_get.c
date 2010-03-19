/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <mem.h>

/****************************************************************************
 * process_get
 *
 * returns a pointer to the process with the specified pid if the process
 * exists, and null otherwise.
 */

process_t *process_get(pid_t pid) {
	uintptr_t addr;

	if (pid >= MAX_PID) {
		return NULL;
	}

	addr = PROCESS_TABLE + (PAGESZ * pid);

	if (page_get(addr) & PF_PRES) {
		return (process_t*) addr;
	}

	return NULL;
}
