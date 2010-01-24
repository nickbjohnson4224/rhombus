/* Copyright 2009, 2010 Nick Johnson */

#include <lib.h>

void sleep(uint32_t cycles) {
	volatile uint32_t i;
	for (i = 0; i < cycles; i++);
}
