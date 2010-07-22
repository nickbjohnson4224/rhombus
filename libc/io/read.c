/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <stdint.h>
#include <stdbool.h>
#include <io.h>
#include <ipc.h>

size_t read(int fd, void *buf, size_t size, uint64_t offset) {
	return psend(fd, buf, NULL, size, offset, PORT_READ);
}
