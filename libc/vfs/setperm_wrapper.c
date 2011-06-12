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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <vfs.h>

char *__setperm_rcall_wrapper(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *r;
	uint32_t user;
	uint8_t perm;

	if (argc < 3) {
		return NULL;
	}

	user = atoi(argv[1]);
	perm = atoi(argv[2]);

	r = index_get(index);

	if (!r) {
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);

	if ((acl_get(r->acl, gettuser()) & PERM_ALTER) == 0) {
		mutex_free(&r->mutex);
		return strdup("! denied");
	}

	r->acl = acl_set(r->acl, user, perm);

	mutex_free(&r->mutex);

	return strdup("T");
}
