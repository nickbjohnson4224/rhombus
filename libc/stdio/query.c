/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdio.h>
#include <ipc.h>

size_t query(FILE *fd, void *rbuf, void *sbuf, size_t size) {
	return ssend(fd, rbuf, sbuf, size, 0, PORT_QUERY);
}
