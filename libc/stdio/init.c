/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>

void _cini(void) {

	/* setup standard streams */
	stdin  = fdopen(0, "w");
	stdout = fdopen(1, "r");
	stderr = fdopen(2, "w");
	stdvfs = fdopen(3, "rw");
	stddev = fdopen(4, "r");
	stdpmd = fdopen(5, "r");
	extin  = fdopen(6, "r");
	extout = fdopen(7, "w");
}
