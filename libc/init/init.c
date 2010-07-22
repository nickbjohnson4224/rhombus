/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <arch.h>
#include <stdlib.h>
#include <string.h>
#include <proc.h>
#include <ipc.h>
#include <io.h>
#include <info.h>
#include <stdio.h>
#include <mmap.h>

static void reject(uint32_t caller, struct packet *packet) {

	if (packet) {
		packet_free(packet);
	}

	send(PORT_REPLY, caller, NULL);
}

static void segfault(uint32_t caller, struct packet *packet) {
	write(STDOUT, "Segmentation Fault\n", 19, 0);

	exit(0);
}

static void fpufault(uint32_t caller, struct packet *packet) {
	write(STDOUT, "Floating Point Exception\n", 25, 0);

	exit(0);
}

void _init(void) {

	/* setup standard streams */
	stdin  = fdopen(STDIN,  "w");
	stdout = fdopen(STDOUT, "r");
	stderr = fdopen(STDERR, "w");
	stdvfs = fdopen(STDVFS, "rw");
	extin  = fdopen(EXTIN,  "r");
	extout = fdopen(EXTOUT, "w");

	when(PORT_FAULT, segfault);
	when(PORT_READ,  reject);
	when(PORT_WRITE, reject);
	when(PORT_INFO,  reject);
	when(PORT_CTRL,  reject);
	when(PORT_QUERY, reject);

	mmap(infospace, sizeof(struct info_node), PROT_READ | PROT_WRITE);
	
	if (infospace[0].value[0] != '!') {
		memclr(infospace, sizeof(struct info_node));
		infospace[0].value[0] = '!';
	}
}
