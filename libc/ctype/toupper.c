/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <ctype.h>

int toupper(int c) {
	
	if ((c >= 'a') && (c <= 'z')) {
		c += ('A' - 'a');
	}

	return c;
}
