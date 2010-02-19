/* Copyright 2010 Nick Johnson */

#include <string.h>
#include <stdint.h>

char *strncat(char *d, const char *s, size_t n) {
	size_t i, e;

	for (e = 0; d[e] != '\0'; e++);

	for (i = 0; s[i] != '\0' && i < n; i++) {
		d[e + i] = s[i];
	}

	d[e + i] = '\0';

	return d;
}
