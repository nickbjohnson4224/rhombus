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
 * fs_list
 *
 * Gives the name of the entry of number <entry> in the directory <dir>.
 * Returns a copy of that string on success, NULL on failure.
 *
 * protocol:
 *   port: PORT_LIST
 *
 *   request:
 *     uint32_t entry
 *
 *   reply:
 *     char name[]
 */

char *fs_list(uint64_t dir, int entry) {
	struct msg *msg;
	char *name;

	msg = aalloc(sizeof(struct msg) + sizeof(uint32_t), PAGESZ);
	if (!msg) return NULL;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = dir;
	msg->length = sizeof(uint32_t);
	msg->port   = PORT_LIST;
	msg->arch   = ARCH_NAT;

	((uint32_t*) msg->data)[0] = entry;

	if (msend(msg)) return NULL;
	msg = mwait(PORT_REPLY, dir);

	if (msg->length == 0) {
		free(msg);
		return NULL;
	}

	name = strdup((const char*) msg->data);

	free(msg);
	return name;
}
