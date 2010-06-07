/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/proc.h>
#include <flux/ipc.h>

static void reject(uint32_t caller, struct packet *packet) {

	if (packet) {
		packet_free(packet);
	}

	send(PORT_REPLY, caller, NULL);
}

static void segfault(uint32_t caller, struct packet *packet) {
	exit(0);
}

void _fini(void) {

	when(PORT_FAULT, segfault);
	when(PORT_READ,  reject);
	when(PORT_WRITE, reject);
	when(PORT_INFO,  reject);
	when(PORT_CTRL,  reject);
	when(PORT_QUERY, reject);
}
