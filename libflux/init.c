/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/proc.h>
#include <flux/ipc.h>
#include <flux/io.h>
#include <flux/info.h>
#include <flux/mmap.h>

static void reject(uint32_t caller, struct packet *packet) {

	if (packet) {
		packet_free(packet);
	}

	send(PORT_REPLY, caller, NULL);
}

static void segfault(uint32_t caller, struct packet *packet) {
	write(FD_STDOUT, "Segmentation Fault\n", 19, 0);

	exit(0);
}

static void fpufault(uint32_t caller, struct packet *packet) {
	write(FD_STDOUT, "Floating Point Exception\n", 25, 0);

	exit(0);
}

void _fini(void) {
	when(PORT_FAULT, segfault);
	when(PORT_READ,  reject);
	when(PORT_WRITE, reject);
	when(PORT_INFO,  reject);
	when(PORT_CTRL,  reject);
	when(PORT_QUERY, reject);

	mmap(infospace, sizeof(struct info_node), PROT_READ | PROT_WRITE);
	
	if (infospace[0].value[0] != '!') {
		arch_memclr(infospace, sizeof(struct info_node));
		infospace[0].value[0] = '!';
	}
}
