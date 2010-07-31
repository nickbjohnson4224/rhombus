/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <exec.h>
#include <proc.h>
#include <ipc.h>
#include <dict.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	char buffer[100];
	char fbuffer[1000];
	size_t i, n;
	int err, pid;
	char *argv[100];
	bool daemon;

	printf("\n");

	setenv("PWD", "/");
	setenv("PATH", "/initrd.tarfs");

	while (1) {
		printf("%s $ ", getenv("PWD"));

		fgets(buffer, 100, stdin);

		for (i = 0; buffer[i]; i++) {
			if (buffer[i] == '\n') {
				buffer[i] = '\0';
			}
		}

		argv[0] = &buffer[0];

		for (i = 0, n = 1; buffer[i]; i++) {
			if (buffer[i] == ' ') {
				buffer[i] = '\0';
				argv[n++] = &buffer[i+1];
			}
		}

		argv[n] = NULL;

		if (argv[n-1][0] == '&') {
			argv[n-1] = NULL;
			daemon = true;
		}
		else {
			daemon = false;
		}

		if (!strcmp(buffer, "cd")) {
			if (buffer[0] == '/') {
				setenv("PWD", buffer);
			}
			else {
				strcpy(buffer, getenv("PWD"));
				strcat(buffer, argv[1]);
				setenv("PWD", buffer);
			}
		}
		else if (!strcmp(buffer, "set")) {
			setenv(argv[1], argv[2]);
		}
		else if (!strcmp(buffer, "get")) {
			printf("%s\n", getenv(argv[1]));
		}
		else {
			pid = fork();
			if (pid < 0) {
				strcpy(fbuffer, getenv("PATH"));
				strcat(fbuffer, "/");
				strcat(fbuffer, buffer);
				err = execv(fbuffer, (char const**) argv);
				if (err) {
					if (err == 1) {
						printf("%s: command not found\n", buffer);
					}
					else {
						printf("failed to execute: %d\n", err);
					}
				}
				exit(0);
			}
			if (!daemon) {
				waits(PORT_DEATH, pid);
			}
			else {
				waits(PORT_SYNC, pid);
			}
		}
	}

	return 0;
}
