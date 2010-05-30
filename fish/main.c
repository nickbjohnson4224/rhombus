/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/exec.h>
#include <flux/proc.h>
#include <flux/ipc.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
	char buffer[100];
	char fbuffer[1000];
	FILE *file;
	size_t i, j, n;
	int err, pid;

	char pwd[100];

	char *argv[100];
	
	printf("\n");

	strcpy(pwd, "/");

	while (1) {
		printf("%s $ ", pwd);

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

		if (!strcmp(buffer, "cd")) {
			strcat(pwd, argv[1]);
		}
		else {
			pid = fork();
			if (pid < 0) {
				strcpy(fbuffer, pwd);
				strcat(fbuffer, "/");
				strcat(fbuffer, buffer);
				err = execv(fbuffer, (char const**) argv);
				if (err) {
					printf("failed to execute: %d\n", err);
				}
				exit(0);
			}
		}
	}

	return 0;
}
