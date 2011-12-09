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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <rho/layout.h>
#include <rho/natio.h>
#include <rho/exec.h>
#include <rho/proc.h>
#include <rho/ipc.h>

int fish_cd(int argc, char **argv) {
	char *path;

	if (argc < 2) {
		setenv("PWD", "/");
		return 0;
	}

	path = path_simplify(argv[1]);

	if (path && fs_find(path)) {
		if (checktype(path, "dir")) {
			setenv("PWD", path);
		}
		else {
			fprintf(stderr, "%s: %s: not a directory\n", getname_s(), argv[1]);
			free(path);
			return 1;
		}
	}
	else {
		fprintf(stderr, "%s: %s: no such directory\n", getname_s(), argv[1]);
		free(path);
		return 1;
	}

	free(path);
	return 0;
}

int fish_exec_bg(int argc, char **argv, FILE *in, FILE *out, FILE *err) {
	char const **_argv;
	int pid;

	pid = fork();
	if (pid < 0) {

		if (in)  stdin  = in;
		if (out) stdout = out;
		if (err) stderr = err;

		_argv = malloc(sizeof(char*) * (argc + 1));
		memcpy(_argv, argv, sizeof(char*) * argc);
		_argv[argc] = NULL;

		if (execv(_argv[0], _argv)) {
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

int fish_exec_fg(int argc, char **argv, FILE *in, FILE *out, FILE *err) {
	char const **_argv;
	int pid;

	pid = fork();
	if (pid < 0) {
		if (in)  stdin  = in;
		if (out) stdout = out;
		if (err) stderr = err;

		_argv = malloc(sizeof(char*) * (argc + 1));
		memcpy(_argv, argv, sizeof(char*) * argc);
		_argv[argc] = NULL;

		if (execv(_argv[0], _argv)) {
			if (errno == ENOENT) {
				fprintf(stderr, "%s: %s: command not found\n", getname_s(), argv[0]);
			}
			else {
				perror(argv[0]);
			}
			
			abort();
		}
	}
	frcall(stdout->fd, "set_fgjob %d", pid);
	mwait(PORT_CHILD, RP_CONS(pid, 0));
	frcall(stdout->fd, "set_fgjob %d", 0);
	
	return 0;
}

int fish_do(int argc, char **argv) {

	if (!strcmp(argv[0], "cd")) {
		return fish_cd(argc, argv);
	}

	if (!strcmp(argv[argc-1], "&")) {
		return fish_exec_bg(argc - 1, argv, NULL, NULL, NULL);
	}
	else {
		return fish_exec_fg(argc, argv, NULL, NULL, NULL);
	}
}

int main() {
	char buffer[100];
	size_t i, n;
	char *argv[100];

	uint32_t *object0, *object1;
	const char *symbol = "baz";

	object0 = dlopen("/lib/libc.so", 0);
	printf("libc.so:\t%p\n", object0);

	object1 = dlopen("/lib/libtest.so", 0);
	printf("libtest.so:\t%p\n", object1);

	printf("libtest.so:__zab\t%p\n", dlsym(object1, "__zab"));

	printf("%d\n", ((int (*)(int)) dlsym(object1, symbol))(7));

	setenv("PWD", "/");

	while (1) {
		printf(getenv("PWD"));
		printf(" $ ");
		fflush(stdout);

		fgets(buffer, 100, stdin);

		for (i = 0; buffer[i]; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
			}
		}

		argv[n = 1] = strtok(buffer, " ");
		while ((argv[++n] = strtok(NULL, " ")) != NULL);

		if (argv[1][0] == '\0') {
			continue;
		}

		if (!strcmp(argv[1], "cd")) {
			fish_cd(n - 1, &argv[1]);
		}
		else {
			argv[0] = strdup("pipe");
			fish_do(n, argv);
		}
	}

	return 0;
}
