#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

/***** PORT ACCESS *****/
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint16_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

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
