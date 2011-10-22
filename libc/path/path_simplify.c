/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <rho/natio.h>

struct path_list {
	struct path_list *next;
	char *name;
};

char *path_simplify(const char *path) {
	struct path *path_struct;
	struct path_list *stack, *stack1, *node;
	char *name, *path1, *path2, *path3;

	stack = NULL;
	stack1 = NULL;
	path3 = strdup("");

	if (path[0] == '/') {
		path1 = strdup(path);
	}
	else if (path[0] == '@') {
		free(path3);
		path3 = struntil(path, "/", NULL);
		path1 = strdup(&path[strlen(path3)]);
	}
	else {
		path1 = strvcat(getenv("PWD"), "/", path, NULL);
		path2 = path_simplify(path1);
		free(path1);
		return path2;
	}

	path_struct = path_cons(path1);

	while ((name = path_next(path_struct))) {

		if (!strcmp(name, ".")) {
			continue;
		}
		else if (!strcmp(name, "..")) {
			if (stack) {
				node = stack;
				stack = stack->next;
				free(node->name);
				free(node);
				free(name);
			}
			else {
				errno = EPATH;
				return NULL;
			}
		}
		else {
			node = malloc(sizeof(struct path_list));
			node->name = name;
			node->next = stack;
			stack = node;
		}
	}

	while (stack) {
		node = stack;
		stack = stack->next;
		node->next = stack1;
		stack1 = node;
	}

	path2 = (stack1) ? strdup("") : strdup("/");

	while (stack1) {
		path2 = realloc(path2, strlen(path1) + strlen(stack1->name) + 2);
		strcat(path2, "/");
		strcat(path2, stack1->name);
		node = stack1;
		stack1 = stack1->next;
		free(node);
	}

	free(path1);
	free(path_struct);

	path1 = strvcat(path3, path2, NULL);
	free(path3);
	free(path2);

	return path1;
}
