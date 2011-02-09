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
 * fs_move
 *
 * Attempts to move the file <file> into directory <dir> with name <name>. If 
 * <file> and <dir> are not in the same driver, this will likely fail, and the 
 * file will have to be manually copied. Returns a pointer to the new 
 * filsystem object (which may be the same as the old filesystem object) on 
 * success, NULL on failure.
 *
 * protocol:
 *   port: PORT_MOVE
 *
 *   request:
 *     uint64_t fobj
 *     char name[]
 *
 *   reply:
 *     uint64_t fobj
 */

uint64_t fs_move(uint64_t dir, const char *name, uint64_t fobj) {
	struct msg *msg;

	msg = aalloc(sizeof(struct msg) + sizeof(uint64_t) + strlen(name) + 1, PAGESZ);
	if (!msg) return 0;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = dir;
	msg->length = sizeof(uint64_t) + strlen(name) + 1;
	msg->port   = PORT_MOVE;
	msg->arch   = ARCH_NAT;

	((uint64_t*) msg->data)[0] = fobj;
	strcpy((char*) &msg->data[8], name);

	if (msend(msg)) return 0;
	msg = mwait(PORT_REPLY, dir);

	if (msg->length < sizeof(uint64_t)) {
		free(msg);
		return 0;
	}

	fobj = ((uint64_t*) msg->data)[0];

	free(msg);
	return fobj;
}
