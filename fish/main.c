/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <io.h>
#include <exec.h>
#include <proc.h>
#include <ipc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	char buffer[100];
	char fbuffer[1000];
	FILE *file;
	size_t i, j, n;
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
					printf("failed to execute: %d\n", err);
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
