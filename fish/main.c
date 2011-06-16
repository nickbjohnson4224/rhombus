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
#include <natio.h>
#include <errno.h>
#include <exec.h>
#include <proc.h>
#include <ipc.h>

int main() {
	char buffer[100];
	size_t i, n;
	int pid;
	int fg_gid;
	char *argv[100];
	char *path;

	setenv("PWD", "/");

	path = rcall(stdin->fd, "getfg");
	sscanf(path, "%i", &fg_gid);
	free(path);

	while (1) {
		printf(getenv("PWD"));
		printf(" $ ");

		fgets(buffer, 100, stdin);

		for (i = 0; buffer[i]; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
			}
		}

		argv[n = 0] = strtok(buffer, " ");
		while ((argv[++n] = strtok(NULL, " ")) != NULL);

		if (argv[0][0] == '\0') {
			continue;
		}

		if (!strcmp(argv[0], "cd")) {
			if (n < 2) {
				setenv("PWD", "/");
				continue;
			}

			path = path_simplify(argv[1]);

			if (path && FS_IS_DIR(fs_type(path))) {
				setenv("PWD", path);
			}
			else {
				fprintf(stderr, "%s: %s: no such directory\n", getname_s(), argv[1]);
			}
			continue;
		}

		pid = fork();
		if (pid < 0) {
			setpgid(getpid(), fg_gid);

			if (execv(argv[0], (char const **) argv)) {
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
	}

	return 0;
}
