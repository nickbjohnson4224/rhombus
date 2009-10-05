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

/***** REMOTE MEMORY ACCESS *****/
int32_t push_call(uint32_t target, uint32_t dest, uint32_t src, uint32_t size);
int32_t pull_call(uint32_t target, uint32_t src, uint32_t dest, uint32_t size);

/***** IRQ ACCESS *****/
int32_t rirq_call(uint32_t irq);
int32_t lriq_call(uint32_t irq);

/***** EMERGENCY OUTPUT *****/
int32_t eout_call(const char *message);

#endif
