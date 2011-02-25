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
#include <mutex.h>
#include <natio.h>
#include <proc.h>
#include <ipc.h>

#include <driver/vfs.h>
#include <driver/io.h>

/*****************************************************************************
 * __rcall_wrapper
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

void __rcall_wrapper(struct msg *msg) {
	struct vfs_obj *file;
	struct msg *reply;
	char *args;
	char *rets;

	if (!_di_rcall) {
		merror(msg);
		return;
	}

	file = vfs_get_index(RP_INDEX(msg->target));

	if (!file) {
		merror(msg);
		return;
	}

	args = (char*) msg->data;

	rets = _di_rcall(msg->source, file, args);

	if (!rets) {
		merror(msg);
		return;
	}

	reply = aalloc(sizeof(struct msg) + strlen(rets) + 1, PAGESZ);
	reply->source = msg->target;
	reply->target = msg->source;
	reply->length = strlen(rets) + 1;
	reply->port   = PORT_REPLY;
	reply->arch   = ARCH_NAT;
	strcpy((char*) reply->data, rets);
	free(rets);

	free(msg);
	msend(reply);
}
