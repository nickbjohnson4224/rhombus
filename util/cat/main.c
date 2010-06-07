/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char **argv) {
	size_t n, i;
	char buffer[1000];
	FILE *file;

	for (n = 1; n < argc; n++) {
		file = fopen(argv[n], "r");

		if (!file) printf("%s: file not found\n", argv[n]);

		while (1) {
			i = fread(buffer, sizeof(char), 1000, file);
			if (i == 0) break;
			fwrite(buffer, sizeof(char), i, stdout);
		}
	}

	return 0;
}
