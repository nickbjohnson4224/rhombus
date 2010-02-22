/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

int atoi(const char *nptr) {
	int i, sum;

	for (sum = 0, i = 0; isdigit(nptr[i]); i++) {
		sum *= 10;
		sum += nptr[i] - '0';
	}

	return sum;
}
