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

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <natio.h>

/*****************************************************************************
 * fs_recv
 *
 * Extract a filesystem command from the given message <msg>. If the message
 * contained is not a valid filesystem command, this function returns NULL.
 */

struct mp_fs *fs_recv(struct msg *msg) {
	struct mp_fs *mp;

	/* reject null messages and packets */
	if (!msg || !msg->packet || !msg->count) {
		return NULL;
	}

	/* get pointer to packet */
	mp = msg->packet;

	/* reject other architectures */
	if (mp->arch != MP_ARCH_NATIVE) {
		return NULL;
	}

	/* reject potential overflows */
	if (mp->length > msg->count * PAGESZ) {
		return NULL;
	}

	/* reject undersized packets */
	if (mp->length < sizeof(struct mp_fs)) {
		return NULL;
	}

	/* reject other protocols */
	if (mp->protocol != MP_PROT_FS) {
		return NULL;
	}

	return mp;
}
