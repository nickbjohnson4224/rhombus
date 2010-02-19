/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

char *strdup(const char *s) {
	char *d;

	d = malloc(sizeof(char) * (strlen(s) + 1));
	strcpy(d, s);

	return d;
}
