/* Copyright 2009, 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/heap.h>
#include <flux/abi.h>
#include <flux/io.h>

void _fini() {

	/* Heap */
	heap_init();
}
