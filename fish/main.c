/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/exec.h>
#include <flux/proc.h>
#include <flux/ipc.h>
#include <flux/info.h>

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

	setinfo("env/PWD", "/");
	setinfo("env/PATH", "/initrd.tarfs");

	while (1) {
		printf("%s $ ", getinfo("env/PWD"));

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
				setinfo("env/PWD", buffer);
			}
			else {
				strcpy(buffer, getinfo("env/PWD"));
				strcat(buffer, argv[1]);
				setinfo("env/PWD", buffer);
			}
		}
		else if (!strcmp(buffer, "set")) {
			setinfo(argv[1], argv[2]);
		}
		else if (!strcmp(buffer, "get")) {
			printf("%s\n", getinfo(argv[1]));
		}
		else {
			pid = fork();
			if (pid < 0) {
				strcpy(fbuffer, getinfo("env/PATH"));
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
