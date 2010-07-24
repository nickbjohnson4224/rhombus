/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	char filelist[1000];
	size_t n, i, j;

	if (argc == 1) {
		list(getenv("PWD"), filelist);

		for (i = 0, j = 1; filelist[i]; i++) {
			if (filelist[i] == ' ') {
				filelist[i] = (j % 6) ? '\t' : '\n';
				j++;
			}
		}

		printf("%s\n", filelist);
	}

	else for (n = 1; n < argc; n++) {

		if (argc > 2) {
			printf("%s:\n", argv[n]);
		}

		list(argv[n], filelist);

		for (i = 0, j = 1; filelist[i]; i++) {
			if (filelist[i] == ' ') {
				filelist[i] = (j % 6) ? '\t' : '\n';
				j++;
			}
		}

		printf("%s\n", filelist);
	}

	return 0;
}
