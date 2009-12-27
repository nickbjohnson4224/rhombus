#ifndef LIBDRIVER_H
#define LIBDRIVER_H

#include <stdint.h>

/***** PORT ACCESS *****/
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint16_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/***** REMOTE MEMORY ACCESS *****/
int32_t push_call(uint32_t target, uint32_t dest, uint32_t src, uint32_t size);
int32_t pull_call(uint32_t target, uint32_t src, uint32_t dest, uint32_t size);
uintptr_t phys_call(uintptr_t addr);

/***** DRIVER INTERFACE STRUCTURE *****/

struct driver_interface {
	int (*init) (uint16_t selector);
	int (*read) (uintmax_t seek, size_t size, void *data);
	int (*write)(uintmax_t seek, size_t size, void *data);
	int (*ctrl) (int16_t request, void *data);
	void (*handler) (uint32_t source, void *grant);
	uint8_t interrupt;
	int (*halt) (void);
};

#endif
