/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * rcall
 *
 * Generic remote procedure call protocol. Sends a string to the given
 * resource, and recieves a string in return. This function can be used to
 * implement any sort of ad-hoc textual protocol, and is a cover-all for any
 * things that cannot be done with the standard I/O and filesystem routines.
 * Returns NULL on error or empty return string.
 *
 * protocol:
 *   port: PORT_RCALL
 *
 *   request:
 *     char args[]
 *
 *   reply:
 *     char rets[]
 */

char *rcall(uint64_t rp, const char *args) {
	struct msg *msg;
	char *rets;

	msg = aalloc(sizeof(struct msg) + strlen(args) + 1, PAGESZ);
	if (!msg) return NULL;
	msg->source = RP_CONS(getpid(), 0);
	msg->target = rp;
	msg->length = strlen(args) + 1;
	msg->port   = PORT_RCALL;
	msg->arch   = ARCH_NAT;
	strcpy((char*) msg->data, args);

	if (msend(msg)) return NULL;
	msg = mwait(PORT_REPLY, rp);

	if (msg->length) {
		rets = strdup((char*) msg->data);
	}
	else {
		rets = NULL;
	}

	free(msg);
	return rets;
}
