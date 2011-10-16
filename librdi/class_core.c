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
#include <abi.h>

#include <rdi/core.h>

rdi_cons_hook rdi_global_cons_file_hook;
rdi_cons_hook rdi_global_cons_dir_hook;
rdi_cons_hook rdi_global_cons_link_hook;

static char *_cons(struct robject *self, rp_t src, int argc, char **argv) {
	struct robject *new_robject = NULL;
	char *type;

	if (argc >= 2) {
		type = argv[1];

		if (!strcmp(type, "file")) {
			if (rdi_global_cons_file_hook) {
				new_robject = rdi_global_cons_file_hook(src, argc, argv);
			}
		}
		else if (!strcmp(type, "link")) {
			if (rdi_global_cons_link_hook) {
				new_robject = rdi_global_cons_link_hook(src, argc, argv);
			}
		}
		else if (!strcmp(type, "dir")) {
			if (rdi_global_cons_dir_hook) {
				new_robject = rdi_global_cons_dir_hook(src, argc, argv);
			}
		}

		if (new_robject) {
			rtab_open(src, RP_CONS(getpid(), new_robject->index));
			robject_open(new_robject, src, STAT_OPEN | robject_stat(new_robject, src));
			return rtoa(RP_CONS(getpid(), new_robject->index));
		}
	}

	return strdup("! arg");
}

struct robject *rdi_class_core;

void __rdi_class_core_setup() {
	
	rdi_class_core = robject_cons(0, robject_class_basic);

	robject_set_call(rdi_class_core, "cons", _cons, 0);

	robject_set_data(rdi_class_core, "type", (void*) "driver");
	robject_set_data(rdi_class_core, "name", (void*) "RDI-class-core");
}

struct robject *rdi_core_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_core);
	robject_set_default_access(r, access);

	return r;
}

void rdi_core_free(struct robject *r) {
	robject_free(r);
}
