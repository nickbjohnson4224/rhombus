/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

size_t io_send(uint64_t rp, void *r, void *s, size_t size, uint64_t off, uint8_t port) {
	struct mp_io *cmd;
	struct mp_error *err;
	struct msg *reply;

	/* format I/O command */
	cmd = malloc(sizeof(struct mp_io) + size);
	cmd->length   = sizeof(struct mp_io) + size;
	cmd->size     = size;
	cmd->offset   = off;
	cmd->protocol = MP_PROT_IO;

	/* copy from sending buffer */
	if (s) memcpy(cmd->data, s, size);

	reply = rp_send(rp, port, (struct mp_basic*) cmd);

	/* attempt to interpret as I/O command */
	cmd = io_recv(reply);
	if (cmd) {
		free(reply);

		/* copy to receiving buffer */
		if (r) memcpy(r, cmd->data, cmd->size);

		return cmd->size;
	}

	/* attempt to interpret as error */
	err = error_recv(reply);
	if (err) {
		free(err);
		free(reply);
		return 0;
	}

	/* free message */
	if (reply) {
		if (reply->packet) {
			free(reply->packet);
		}
		free(reply);
	}

	return 0;
}
