/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

/* Port access ********************************************************/
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/* IRQ redirection ****************************************************/
void rirq(uint8_t irq);

#endif/*DRIVER_H*/
