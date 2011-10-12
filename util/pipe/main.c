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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <errno.h>
#include <proc.h>
#include <exec.h>

int pipe_exec_bg(char const **argv, FILE *in, FILE *out) {
	int pid;

	pid = fork();
	if (pid < 0) {

		if (in)  stdin  = in;
		if (out) stdout = out;

		if (execv(argv[0], argv)) {
			if (errno == ENOENT) {
				fprintf(stderr, "%s: %s: command not found\n", getname_s(), argv[0]);
			}
			else {
				perror(argv[0]);
			}
			
			abort();
		}
	}
	
	return 0;
}

int pipe_exec_fg(char const **argv, FILE *in, FILE *out) {
	int pid;

	pid = fork();
	if (pid < 0) {

		if (in)  stdin  = in;
		if (out) stdout = out;

		if (execv(argv[0], argv)) {
			if (errno == ENOENT) {
				fprintf(stderr, "%s: %s: command not found\n", getname_s(), argv[0]);
			}
			else {
				perror(argv[0]);
			}
			
			abort();
		}
	}
	mwait(PORT_CHILD, RP_CONS(pid, 0));
	
	return 0;
}

char const **argv_copy(int argc, char **argv) {
	char const **_argv;

	_argv = malloc(sizeof(const char *) * (argc + 1));
	memcpy(_argv, argv, sizeof(const char *) * argc);
	_argv[argc] = NULL;

	return _argv;
}

struct pipe_list {
	struct pipe_list *next;
	char const **argv;
	FILE *out;
};

struct pipe_list *pipe_list_add(struct pipe_list *list, int argc, char **argv) {
	struct pipe_list *node;
	FILE *pipe;

	if (list) {
		list->next = pipe_list_add(list->next, argc, argv);
		return list;
	}
	else {
		pipe = fdopen(rp_cons(fs_find("/sys/pipe"), "file"), "w+");

		node = malloc(sizeof(struct pipe_list));
		node->next = NULL;
		node->argv = argv_copy(argc, argv);
		node->out  = pipe;

		return node;
	}
}

int pipe_list_exec(struct pipe_list *list, FILE *in, FILE *out) {

	if (!list) {
		return 0;
	}

	while (list->next) {
		pipe_exec_bg(list->argv, in, list->out);
		in = list->out;
		list = list->next;
	}

	pipe_exec_fg(list->argv, in, out);

	return 0;
}

int main(int argc, char **argv) {
	struct pipe_list *list = NULL;
	int start;
	int i;

	start = 1;
	for (i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "|")) {
			list = pipe_list_add(list, i - start, &argv[start]);
			start = i + 1;
		}
	}
	list = pipe_list_add(list, i - start, &argv[start]);

	pipe_list_exec(list, stdin, stdout);

	return 0;
}
