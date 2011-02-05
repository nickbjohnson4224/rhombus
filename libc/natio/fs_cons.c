/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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
#include <stdlib.h>
#include <natio.h>
#include <errno.h>

/*****************************************************************************
 * fs_cons
 *
 * Attempts to create a new filesystem object of type <type> and name <name> 
 * in directory <dir>. Returns the new object on success, NULL on failure.
 *
 * protocol:
 *   port: PORT_CONS
 *
 *   request:
 *     uint32_t type
 *     char name[]
 *
 *   reply:
 *     uint64_t rp
 */

uint64_t fs_cons(uint64_t dir, const char *name, int type) {
	struct msg *msg;
	uint64_t rp;

	msg = aalloc(sizeof(struct msg) + sizeof(uint32_t) + strlen(name) + 1, PAGESZ);
	msg->source = RP_CONS(getpid(), 0);
	msg->target = dir;
	msg->length = sizeof(uint32_t) + strlen(name) + 1;
	msg->port   = PORT_CONS;
	msg->arch   = ARCH_NAT;

	((uint32_t*) msg->data)[0] = type;
	strcpy((char*) &msg->data[sizeof(uint32_t)], name);

	if (msend(msg)) return 0;
	msg = mwait(PORT_REPLY, dir);

	if (msg->length < sizeof(uint64_t)) {
		free(msg);
		return 0;
	}

	rp = ((uint64_t*) msg->data)[0];

	free(msg);
	return rp;
}
