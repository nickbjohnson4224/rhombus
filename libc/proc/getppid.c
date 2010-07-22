/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <abi.h>

uint32_t getppid(void) {
	return _gpid(GPID_PARENT);
}
