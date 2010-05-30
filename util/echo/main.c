/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	size_t n;

	for (n = 1; n < argc; n++) {
		printf("%s ", argv[n]);
	}

	printf("\n");

	return 0;
}
