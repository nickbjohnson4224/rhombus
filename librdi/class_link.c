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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>

// XXX SEC - does not check for read access
static char *_find(struct robject *r, rp_t src, int argc, char **argv) {
	char *link;

	link = robject_get_data(r, "link");

	if (argc == 2) {
		if (link) {
			return saprintf(">> %s", link);
		}
		else {
			return rtoa(RP_CONS(getpid(), r->index));
		}
	}
	else if (argc == 3 && !strcmp(argv[1], "-L")) {
		return rtoa(RP_CONS(getpid(), r->index));
	}

	return strdup("! arg");
}

// XXX SEC - does not check for write access
static char *_set_link(struct robject *r, rp_t src, int argc, char **argv) {
	char *link;
	char *old;

	if (argc == 2) {
		link = argv[1];

		old = robject_get_data(r, "link");
		robject_set_data(r, "link", strdup(link));
		free(old);

		return strdup("T");
	}

	return strdup("! arg");
}

// XXX SEC - does not check for read access
static char *_get_link(struct robject *r, rp_t src, int argc, char **argv) {
	return strdup(robject_get_data(r, "link"));
}

struct robject *rdi_class_link;

void __rdi_class_link_setup() {
	
	rdi_class_link = robject_cons(0, rdi_class_core);

	robject_set_call(rdi_class_link, "find", _find);
	robject_set_call(rdi_class_link, "set-link", _set_link);
	robject_set_call(rdi_class_link, "get-link", _get_link);

	// XXX DEP - legacy interface
	robject_set_call(rdi_class_link, "symlink", _set_link);

	robject_set_data(rdi_class_link, "type", (void*) "link");
	robject_set_data(rdi_class_link, "name", (void*) "RDI-class-link");
}

struct robject *rdi_link_cons(uint32_t index, uint32_t access, const char *link) {
	struct robject *r;

	r = robject_cons(index, rdi_class_link);

	robject_set_data(r, "access-default", (void*) access);
	if (link) robject_set_data(r, "link", strdup(link));

	return r;
}

void rdi_link_free(struct robject *r) {
	char *link;

	link = robject_get_data(r, "link");

	if (link) {
		free(link);
	}

	robject_free(r);
}
