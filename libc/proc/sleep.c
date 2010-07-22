/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <arch.h>
#include <abi.h>

void sleep() {
	_send(0, 0);
}
