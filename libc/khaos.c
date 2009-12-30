#include <stdint.h>
#include <stdlib.h>
#include <khaos.h>

void block() {
	_ctrl(CTRL_SCHED, CTRL_SCHED);
}

void unblock() {
	_ctrl(CTRL_NONE, CTRL_SCHED);
}
