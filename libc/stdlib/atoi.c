/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>

int atoi(const char *nptr) {
	int i, sum;

	for (sum = 0, i = 0; nptr[i]; i++) {
		sum *= 10;
		sum += nptr[i] - '0';
	}

	return sum;
}
