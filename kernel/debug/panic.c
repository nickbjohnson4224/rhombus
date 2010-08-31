/* 
 * Copyright 2009 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <debug.h>
#include <util.h>

void debug_panic(const char *message) {
	extern void halt(void);

	debug_color(COLOR_RED);
	debug_printf("Kernel panic: %s\n", message);
	halt();
}
