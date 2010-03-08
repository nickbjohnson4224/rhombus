#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <flux/arch.h>

/****************************************************************************
 * Port IO Routines
 */

void     outb(uint16_t port, uint8_t val);
void     outw(uint16_t port, uint8_t val);
void     outd(uint16_t port, uint8_t val);

uint8_t  inb (uint16_t port);
uint16_t inw (uint16_t port);
uint32_t ind (uint16_t port);

#endif
