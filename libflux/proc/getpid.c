/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/abi.h>
#include <flux/proc.h>

uint32_t getpid(void) {
	return _gpid();
}
