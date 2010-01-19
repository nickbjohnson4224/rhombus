/* Copyright 2009 Nick Johnson */

#include "libc.h"
#include <signal.h>
#include <flux.h>

static void __exit(uint32_t source, struct request *req) {
	_exit(1);
}

void _cini() {
	extern void siginit(void);

	/* Heap */
	_heap_init();

	/* Fault handling */
	siginit();
	sigregister(0, __exit);
	sigregister(2, __exit);
	sigregister(5, __exit);
}
