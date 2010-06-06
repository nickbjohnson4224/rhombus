/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/proc.h>
#include <flux/ipc.h>

static void segfault(uint32_t caller, struct packet *packet) {

	exit(0);
}

void _fini(void) {

	when(PORT_FAULT, segfault);

}
