/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <kernel/proc.h>
#include <kernel/memory.h>

/****************************************************************************
 * proc_free
 *
 * It is guaranteed that the process "process" is not
 */

void proc_free(proc_t *process) {
	frame_t frame;

	/* free 
