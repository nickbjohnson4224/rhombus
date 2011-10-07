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

static struct robject *__find(struct robject *root, const char *path_str, const char **tail) {
	struct path *path;
	char *name;
	char *lookup;

	path = path_cons(path_str);

	while (root) {
		name = path_next(path);

		if (!name) {
			if (tail) *tail = NULL;
			free(path);
			return root;
		}

		if (robject_check_type(root, "link")) {
			path_prev(path);
			if (tail) *tail = path_tail(path);
			free(path);
			return root;
		}

		lookup = strvcat("dirent-", name, NULL);
		root = robject_data(root, lookup);
		free(lookup);
	}

	return NULL;
}

// XXX SEC - does not check for read access
static char *_find(struct robject *r, rp_t src, int argc, char **argv) {
	struct robject *file;
	const char *path;
	const char *tail;
	bool link;

	if (argc <= 1) return strdup("! arg");

	// check for link follow flag
	if (argc == 2) {
		link = false;
		path = argv[1];
	}
	else if (!strcmp(argv[1], "-L")) {
		link = true;
		path = argv[2];
	}
	else {
		return strdup("! arg");
	}

	// find resource
	file = __find(r, path, &tail);

	if (!file) return strdup("! nfound");

	if (robject_check_type(file, "link") && !(!tail && link)) {
		/* return redirect to symlink */
		return saprintf(">> %s/%s", robject_data(file, "link"), (tail) ? tail : "");
	}
	else {
		/* return resource pointer */
		return rtoa(RP_CONS(getpid(), file->index));
	}
}

// XXX SEC - does not check for read access
static char *_list(struct robject *r, rp_t src, int argc, char **argv) {
	return robject_data(r, "dirents");
}

// XXX SEC - does not check for write access
static char *_link(struct robject *r, rp_t src, int argc, char **argv) {
	char *entry;
	char *lookup;
	uint32_t index;
	struct robject *hardlink;

	if (argc == 3) {
		entry = argv[1];
		index = atoi(argv[2]);

		hardlink = robject_get(index);
		if (!hardlink) {
			// link does not exist
			return strdup("! noent");
		}

		lookup = strvcat("dirent-", entry, NULL);

		if (robject_get_data(r, lookup)) {
			// entry already exists, fail
			free(lookup);
			return strdup("! exist");
		}

		robject_set_data(r, lookup, hardlink);
		free(lookup);

		// XXX HACK
		lookup = robject_get_data(r, "dirents");
		lookup = strvcat(entry, lookup, NULL);
		robject_set_data(r, "dirents", lookup);

		return strdup("T");
	}

	return strdup("! arg");
}

// XXX SEC - does not check for write access
static char *_unlink(struct robject *r, rp_t src, int argc, char **argv) {
	char *entry;
	char *lookup;

	if (argc == 2) {
		entry = argv[1];

		lookup = strvcat("dirent-", entry, NULL);

		if (!robject_get_data(r, lookup)) {
			// entry does not exist, fail
			free(lookup);
			return strdup("! noent");
		}

		robject_set_data(r, lookup, NULL);
		free(lookup);

		return strdup("T");
	}

	return strdup("! arg");
}

struct robject *rdi_class_dir;

void __rdi_class_dir_setup() {
	
	rdi_class_dir = robject_cons(0, rdi_class_core);

	robject_set_call(rdi_class_dir, "find", _find);
	robject_set_call(rdi_class_dir, "list", _list);
	robject_set_call(rdi_class_dir, "link", _link);
	robject_set_call(rdi_class_dir, "unlink", _unlink);

	robject_set_data(rdi_class_dir, "type", (void*) "dir");
	robject_set_data(rdi_class_dir, "name", (void*) "RDI-class-dir");
}

struct robject *rdi_dir_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_dir);
	robject_set_data(r, "access-default", (void*) access);

	return r;
}

void rdi_dir_free(struct robject *r) {
	robject_free(r);
}
