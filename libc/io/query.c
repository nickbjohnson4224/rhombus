/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <io.h>
#include <ipc.h>

size_t query(int fd, void *rbuf, void *sbuf, size_t size) {
	return psend(fd, rbuf, sbuf, size, 0, PORT_QUERY);
}
