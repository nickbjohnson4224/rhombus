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

#include <robject.h>
#include <string.h>

static char *__rcall_type(struct robject *self, rp_t src, int argc, char **argv) {
	const char *type;

	type = robject_get_data(self, "type");
	
	if (type) {
		return strdup(type);
	}
	else {
		return NULL;
	}
}

static char *__rcall_ping(struct robject *self, rp_t src, int argc, char **argv) {
	return strdup("T");
}

struct robject *robject_root;

void __robject_init(void) {

	// allocate root object
	robject_root = robject_cons(0, NULL);

	// set up basic data fields
//	robject_set_data(robject_root, "type", "basic");

	// set up basic rcall handlers
//	robject_set_call(robject_root, "type", __rcall_type);
//	robject_set_call(robject_root, "ping", __rcall_ping);
}
