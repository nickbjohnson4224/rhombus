/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <abi.h>

void exit(int status) {
	_exit(status);
}
