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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <arch.h>
#include <ipc.h>

/***************************************************************************
 * ssend - "stream send"
 *
 * Send a whole buffer <s> of size <size> through a stream at port <port> to
 * offset <off>, and recieve a whole buffer <r> of size <size> as a reply.
 *
 * If <s> is NULL, nothing is sent; if <r> is NULL, nothing is recieved.
 *
 * This function is used internally by most native I/O functions.
 *
 * Returns the number of bytes sent.
 */

size_t ssend(uint64_t fd, void *re, void *se, size_t size, uint64_t off, uint8_t port) {
	struct packet *p_out;
	struct packet *p_in;
	uint8_t *r, *s;
	size_t datasize, oldsize, frag;
	size_t spos, rpos;
	event_t old_handler;

	old_handler = when(PORT_REPLY, NULL);
	oldsize     = size;
	p_out       = palloc(0);
	p_in        = NULL;
	frag        = 0;
	spos        = 0;
	rpos        = 0;
	r           = (uint8_t*) re;
	s           = (uint8_t*) se;

	while (size) {
		datasize = (size > PACKET_MAXDATA) ? PACKET_MAXDATA : size;

		psetbuf(&p_out, datasize);

		p_out->identity = 0;
		p_out->protocol = PACKET_PROTOCOL;

		p_out->fragment_index = frag;
		p_out->fragment_count = ((size - 1) / PACKET_MAXDATA + 1);
		p_out->target_pid     = fd >> 32;
		p_out->target_inode   = fd & 0xFFFFFFFF;
		p_out->offset         = off;

		if (s) {
			memcpy(pgetbuf(p_out), &s[spos], datasize);
			spos += datasize;
		}

		psend(port, fd >> 32, p_out);
		p_in = pwaits(PORT_REPLY, fd >> 32);

		if (!p_in || p_in->data_length == 0) {
			if (p_in) pfree(p_in);
			break;
		}

		if (r) {
			memcpy(&r[rpos], pgetbuf(p_in), p_in->data_length);
			rpos += p_in->data_length;
		}

		size -= p_in->data_length;
		off  += p_in->data_length;

		pfree(p_in);

		frag++;
	}

	pfree(p_out);
	when(PORT_REPLY, old_handler);

	return (oldsize - size);
}
