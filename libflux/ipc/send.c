/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/abi.h>
#include <flux/ipc.h>
#include <flux/heap.h>

/****************************************************************************
 * send
 *
 * Sends a packet to the specified port of the specified target process.
 * The given packet is freed regardless of whether the call is successful.
 */

int send(uint8_t port, uint32_t target, struct packet *packet) {
	uint32_t err;

	_svpr((uintptr_t) packet, 0);
	packet_free(packet);
	err = _send(target, port);

	return err;
}
