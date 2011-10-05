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
#include <mutex.h>

#include <robject.h>

struct robject *robject_cons(uint32_t index, struct robject *parent) {
	struct robject *robject;

	robject = malloc(sizeof(struct robject));
	robject->mutex = false;
	robject->index = index;
	robject->parent = parent;

	robject->call_table = NULL;
	robject->data_table = NULL;
	robject->evnt_table = NULL;
	robject->evnt_subs  = NULL;

	if (index) {
		robject_set(index, parent);
	}

	return robject;
}

void robject_free(struct robject *ro) {

	// remove from table (if in table)
	if (ro->index) {
		robject_set(ro->index, NULL);
	}

	mutex_spin(&ro->mutex);

	// XXX - should also free tables here

	free(ro);
}
