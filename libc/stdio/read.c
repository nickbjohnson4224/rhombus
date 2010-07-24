/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <ipc.h>

size_t read(FILE *file, void *buf, size_t size, uint64_t offset) {
	return ssend(file, buf, NULL, size, offset, PORT_READ);
}
