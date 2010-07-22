/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <abi.h>
#include <proc.h>

uint32_t getpid(void) {
	return _gpid(GPID_SELF);
}
