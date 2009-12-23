#include "libc.h"
#include <khaos/signal.h>
#include <khaos/kernel.h>

static void __exit(uint32_t source, uint32_t args[4]) {
	_exit(1);
}

void _cini() {
	/* Heap */
	_heap_init();

	/* Fault handling */
	khsig_init();
	khsig_register(0, __exit);
	khsig_register(2, __exit);
	khsig_register(5, __exit);
}
