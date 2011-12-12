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
#include <errno.h>
#include <stdio.h>

#include <sys/wait.h>

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/exec.h>

int pipe_exec_bg(char const **argv, int in, int out) {
	int pid;

	pid = fork();
	if (pid == 0) {

		ropen(0, fd_rp(in), STAT_READER);
		ropen(1, fd_rp(out), STAT_WRITER);

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
	close(in);
	close(out);
	
	return 0;
}

int pipe_exec_fg(char const **argv, int in, int out) {
	int pid;

	pid = fork();
	if (pid == 0) {

		ropen(0, fd_rp(in), STAT_READER);
		ropen(1, fd_rp(out), STAT_WRITER);

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
	close(in);
	close(out);
	waitpid(pid, NULL, 0);

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
};

struct pipe_list *pipe_list_add(struct pipe_list *list, int argc, char **argv) {
	struct pipe_list *node;

	if (list) {
		list->next = pipe_list_add(list->next, argc, argv);
		return list;
	}
	else {
		node = malloc(sizeof(struct pipe_list));
		node->next = NULL;
		node->argv = argv_copy(argc, argv);

		return node;
	}
}

int pipe_list_exec(struct pipe_list *list, int in, int out) {
	int pipefd[2];

	if (!list) {
		return 0;
	}

	while (list->next) {
		if (pipe(pipefd)) {
			fprintf(stderr, "%s: error: could not open pipe\n", getname_s());
			abort();
		}

		pipe_exec_bg(list->argv, in, pipefd[1]);
		in = pipefd[0];

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

	pipe_list_exec(list, dup(stdin->fd), dup(stdout->fd));

	return 0;
}
