/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <flux.h>

void block(bool v) {
	if (v) {
		_ctrl(CTRL_BLOCK, CTRL_BLOCK, 0);
	}
	else {
		_ctrl(CTRL_NONE, CTRL_BLOCK, 0);
	}
}

void sleep() {
	_fire(0, 0, NULL, 0);
}
