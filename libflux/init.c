/* Copyright 2009, 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/heap.h>
#include <flux/abi.h>
#include <flux/signal.h>
#include <flux/io.h>

#include <stdio.h>

static void __exit(uint32_t source, struct request *req) {
	_exit(1);
}

void _cini() {
	extern void signal_init(void);

	/* Heap */
	heap_init();

	/* Fault handling */
	signal_init();
}
