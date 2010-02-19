/* Copyright 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/proc.h>

void block(bool v) {
	if (v) {
		pctrl(CTRL_BLOCK, CTRL_BLOCK, 0);
	}
	else {
		pctrl(CTRL_NONE, CTRL_BLOCK, 0);
	}
}
