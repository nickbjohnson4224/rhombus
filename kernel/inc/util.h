/* Copyright 2009, 2010 Nick Johnson */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <arch.h>

/***** I/O, ETC. *****/
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)
void sleep(uint32_t cycles);

#endif/*UTIL_H*/
