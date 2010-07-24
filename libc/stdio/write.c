/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdio.h>
#include <ipc.h>

size_t write(FILE *file, void *buf, size_t size, uint64_t offset) {
	return ssend(file, NULL, buf, size, offset, PORT_WRITE);
}
