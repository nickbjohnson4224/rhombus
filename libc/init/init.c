/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <arch.h>
#include <stdlib.h>
#include <string.h>
#include <proc.h>
#include <ipc.h>
#include <stdio.h>
#include <mmap.h>

static void reject(uint32_t caller, struct packet *packet) {

	if (packet) {
		packet_free(packet);
	}

	send(PORT_REPLY, caller, NULL);
}

static void segfault(uint32_t caller, struct packet *packet) {
	printf("Segmentation Fault\n");

	exit(0);
}

static void fpufault(uint32_t caller, struct packet *packet) {
	printf("Floating Point Exception\n");

	exit(0);
}

void _init(void) {

	/* setup standard streams */
	stdin  = fload("stdin");
	stdout = fload("stdout");
	stderr = fload("stderr");
	stdvfs = fload("stdvfs");

	when(PORT_FAULT, segfault);
	when(PORT_READ,  reject);
	when(PORT_WRITE, reject);
	when(PORT_INFO,  reject);
	when(PORT_CTRL,  reject);
	when(PORT_QUERY, reject);

}
