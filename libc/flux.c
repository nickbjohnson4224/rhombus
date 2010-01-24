/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <flux.h>
#include <driver.h>
#include <config.h>
#include <mmap.h>
#include <_libc.h>

void block() {
	_ctrl(CTRL_SCHED, CTRL_SCHED);
}

void unblock() {
	_ctrl(CTRL_NONE, CTRL_SCHED);
}
