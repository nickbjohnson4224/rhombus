/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/abi.h>

void sleep() {
	_fire(0, 0, NULL, false);
}
