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

#include <rdi/robject.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>

struct __dirent_list {
	struct __dirent_list *next;
	struct __dirent_list *prev;
	char *name;
};

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

static char *_list(struct robject *r, rp_t src, int argc, char **argv) {
	struct __dirent_list *node;
	char *list;
	char *temp;

	list = NULL;
	node = robject_data(r, "list");

	while (node) {
		if (list) {
			temp = list;
			list = strvcat(node->name, "\t", list, NULL);
			free(temp);
		}
		else {
			list = strdup(node->name);
		}

		node = node->next;
	}

	if (list) {
		return strdup(list);
	}
	else {
		return NULL;
	}
}

static char *_link(struct robject *r, rp_t src, int argc, char **argv) {
	char *entry;
	char *lookup;
	uint32_t index;
	struct robject *hardlink;
	struct __dirent_list *list, *node;

	if (argc == 3) {
		entry = argv[1];
		index = RP_INDEX(ator(argv[2]));

		if (RP_PID(ator(argv[2])) != getpid()) {
			return strdup("! extern");
		}

		hardlink = robject_get(index);
		if (!hardlink) {
			// link does not exist
			return strdup("! noent");
		}

		lookup = strvcat("dirent-", entry, NULL);

		if (robject_get_data(r, lookup)) {
			// entry already exists; only change hardlink
			robject_set_data(r, lookup, hardlink);
			free(lookup);
			return strdup("T");
		}

		robject_set_data(r, lookup, hardlink);
		free(lookup);

		list = robject_data(r, "list");
		node = malloc(sizeof(struct __dirent_list));
		node->next = list;
		node->prev = NULL;
		node->name = strdup(entry);
		if (node->next) node->next->prev = node;
		robject_set_data(r, "list", node);

		return strdup("T");
	}

	return strdup("! arg");
}

static char *_unlink(struct robject *r, rp_t src, int argc, char **argv) {
	char *entry;
	char *lookup;
	struct __dirent_list *list;

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

		list = robject_data(r, "list");
		while (list) {
			if (!strcmp(list->name, entry)) {
				if (list->prev) {
					list->prev->next = list->next;
				}
				else {
					robject_set_data(r, "list", list->next);
				}

				if (list->next) {
					list->next->prev = list->prev;
				}
				break;
			}
			list = list->next;
		}

		return strdup("T");
	}

	return strdup("! arg");
}

struct robject *rdi_class_dir;

void __rdi_class_dir_setup() {
	
	rdi_class_dir = robject_cons(0, rdi_class_core);

	robject_set_call(rdi_class_dir, "find",   _find,   0);
	robject_set_call(rdi_class_dir, "list",   _list,   STAT_READER);
	robject_set_call(rdi_class_dir, "link",   _link,   STAT_WRITER);
	robject_set_call(rdi_class_dir, "unlink", _unlink, STAT_WRITER);

	robject_set_data(rdi_class_dir, "type", (void*) "dir");
	robject_set_data(rdi_class_dir, "name", (void*) "RDI-class-dir");
}

struct robject *rdi_dir_cons(uint32_t index, uint32_t access) {
	struct robject *r;

	r = robject_cons(index, rdi_class_dir);
	robject_set_default_access(r, access);

	return r;
}

void rdi_dir_free(struct robject *r) {
	struct __dirent_list *node, *temp_node;
	char *temp;

	node = robject_get_data(r, "list");

	while (node) {		
		temp = strvcat("dirent-", node->name, NULL);
		free(robject_get_data(r, temp));
		free(temp);

		free(node->name);
		temp_node = node->next;
		free(node);
		node = temp_node;
	}

	robject_free(r);
}

void rdi_vfs_add(struct robject *root, const char *path, struct robject *r) {
	struct robject *dir;
	char *path1;
	char *call;

	if (!r) {
		return;
	}

	// find parent directory
	path1 = path_parent(path);
	
	dir = __find(root, path1, NULL);
	free(path1);

	if (!dir) {
		return;
	}

	path1 = path_name(path);
	call = saprintf("link %s %r", path1, RP_CONS(getpid(), r->index));
	free(robject_call(dir, 0, call));
	free(call);
	free(path1);
}
