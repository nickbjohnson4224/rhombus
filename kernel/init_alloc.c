#include <config.h>
#include <stdint.h>
#include <lib.h>
#include <mem.h>

__attribute__ ((section(".idata"))) 
uint32_t allocator_space = KMALLOC;

__attribute__ ((section(".itext"))) 
void *kmalloc(uint32_t size) {
	allocator_space += size;

	return (void*) (allocator_space - size);
}
