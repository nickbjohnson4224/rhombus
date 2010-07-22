/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <ctype.h>

int tolower(int c) {
	
	if ((c >= 'A') && (c <= 'Z')) {
		c += ('a' - 'A');
	}

	return c;
}
