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

#include <rdi/core.h>
#include <rdi/access.h>

#include <string.h>
#include <mutex.h>

/*****************************************************************************
 * _rdi_callback_cons
 *
 * Cons callback currently registered by the driver.
 */

int (*__rdi_callback_open)(uint64_t src, struct resource *r);

/*****************************************************************************
 * rdi_set_read
 *
 * Set the cons callback.
 */

void rdi_set_open(int (*_open)(uint64_t src, struct resource *r)) {	
	__rdi_callback_open = _open;
}

/*****************************************************************************
 * __rdi_open_handler
 */

char *__rdi_open_handler(uint64_t src, uint32_t idx, int argc, char **argv) {
	struct resource *r;

	r = index_get(idx);
	if (!r) {
		/* file not found */
		return strdup("! nfound");
	}

	mutex_spin(&r->mutex);

	if (__rdi_callback_open) {
		/* let driver verify/enact open request */
		if (__rdi_callback_open(src, r)) {
			mutex_free(&r->mutex);
			return strdup("! denied");
		}
	}
	else {
		/* increment open count for source PID */
		id_hash_set(&r->open, RP_PID(src), id_hash_get(&r->open, RP_PID(src)) + 1);
	}

	mutex_free(&r->mutex);

	return strdup("T");
}
