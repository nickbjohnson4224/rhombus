#include "libc.h"
#include <khaos/signal.h>
#include <khaos/kernel.h>

static void _exit(uint32_t source, uint32_t args[4]) {
	exit_call(1);
}

void _cini() {
	/* Heap */
	_heap_init();

	/* Fault handling */
	khsig_init();
	khsig_register(0, _exit);
	khsig_register(2, _exit);
	khsig_register(5, _exit);
}
