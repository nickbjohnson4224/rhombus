/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/proc.h>

void block(bool v) {
	if (v) {
		_pctl(CTRL_BLOCK, CTRL_BLOCK);
	}
	else {
		_pctl(CTRL_NONE, CTRL_BLOCK);
	}
}
