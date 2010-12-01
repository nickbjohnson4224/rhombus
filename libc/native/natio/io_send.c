/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <natio.h>
#include <stdio.h>
#include <arch.h>
#include <ipc.h>

/***************************************************************************
 * io_send
 *
 * Send a whole buffer <s> of size <size> to file <fd> at port <port> at
 * offset <off>, and recieve a whole buffer <r> of size <size> as a reply.
 * If <s> is NULL, nothing is sent; if <r> is NULL, nothing is recieved.
 * This function is used internally by most native I/O functions. Returns 
 * the number of bytes sent.
 */

size_t io_send(uint64_t fd, void *r, void *s, size_t size, uint64_t off, uint8_t port) {
	struct io_cmd *cmd;
	size_t packet_size;
	struct msg *msg;
	event_t old_handler;

	/* save old PORT_REPLY handler */
	old_handler = when(PORT_REPLY, NULL);

	/* allocate message */
	msg = malloc(sizeof(struct msg));

	/* figure out size of whole packet */
	packet_size = sizeof(struct io_cmd) + size;
	
	/* allocate packet */
	msg->count  = (packet_size % PAGESZ) ? (size / PAGESZ) + 1 : size / PAGESZ;
	msg->packet = aalloc(msg->count * PAGESZ, PAGESZ);

	/* set up I/O command header */
	cmd = msg->packet;
	cmd->length = size;
	cmd->offset = off;

	/* copy data from source */
	if (s) memcpy(cmd->data, s, size);

	/* send message */
	msg->value = fd & 0xFFFFFFFF;
	msend(port, fd >> 32, msg);

	/* recieve response */
	msg = mwaits(PORT_REPLY, fd >> 32);
	cmd = msg->packet;

	if (!cmd) {
		size = 0;
	}
	else {
		size = cmd->length;

		/* copy data to destination */
		if (r) memcpy(r, cmd->data, size);
	}

	/* free recieved message */
	if (msg->packet) free(msg->packet);
	free(msg);

	/* restore old PORT_REPLY handler */
	when(PORT_REPLY, old_handler);

	return size;
}
