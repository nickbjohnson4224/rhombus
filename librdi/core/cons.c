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

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/access.h>

#include <stdlib.h>
#include <string.h>
#include <natio.h>
#include <proc.h>

/*****************************************************************************
 * _rdi_callback_cons
 *
 * Cons callback currently registered by the driver.
 */

struct resource *(*__rdi_callback_cons)(uint64_t src, int type);

/*****************************************************************************
 * rdi_set_read
 *
 * Set the cons callback.
 */

void rdi_set_cons(struct resource *(*_cons)(uint64_t src, int type)) {	
	__rdi_callback_cons = _cons;
}

/*****************************************************************************
 * __rdi_cons_handler
 */

char *__rdi_cons_handler(uint64_t src, uint32_t idx, int argc, char **argv) {
	struct resource *new;
	int type;

	if (argc != 2) return NULL;
	if (!__rdi_callback_cons) return strdup("! nosys");

	type = typeflag(argv[1][0]);
	new = __rdi_callback_cons(src, type);

	if (!new) return strdup("! construct");

	index_set(new->index, new);

	return rtoa(RP_CONS(getpid(), new->index));
}
