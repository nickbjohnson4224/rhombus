/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <io.h>
#include <info.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	char buffer[100];
	FILE *target;

	if (argc < 3) {
		return 1;
	}

	target = fopen(argv[1], "r");

	if (!target) {
		return 1;
	}

	info(target->filedes, buffer, argv[2]);

	printf("%s\n", buffer);

	return 0;
}
