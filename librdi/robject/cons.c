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
#include <time.h>

#include <rho/struct.h>
#include <rho/mutex.h>
#include <rho/natio.h>

#include <rdi/robject.h>

struct robject *robject_cons(uint32_t index, struct robject *parent) {
	struct robject *robject;

	robject = malloc(sizeof(struct robject));
	robject->mutex = false;
	robject->driver_mutex = false;
	robject->index = index;
	robject->parent = parent;

	robject->call_table = NULL;
	robject->call_class = NULL;
	robject->data_table = NULL;
	robject->subs_table = NULL;
	robject->accs_table = NULL;

	// generate random keys
	// XXX SEC - this should be replaced with a secure RNG ASAP
	robject->key[0] = 0ULL;
	for (int i = 1; i < 8; i++) {
		robject->key[i] = (uint64_t) rand() | (uint64_t) rand() << 32ULL;
	}

	if (index) {
		robject_set(index, robject);
	}

	return robject;
}

void robject_free(struct robject *ro) {

	// remove from table (if in table)
	if (ro->index) {
		robject_set(ro->index, NULL);
	}

	mutex_spin(&ro->mutex);

	s_table_free(ro->call_table);
	s_table_free(ro->call_class);
	s_table_free(ro->data_table);
	s_table_free(ro->subs_table);
	s_table_free(ro->accs_table);

	free(ro);
}
