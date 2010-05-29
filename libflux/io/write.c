/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>

size_t write(int fd, void *buf, size_t size, uint64_t offset) {
	return psend(fd, NULL, buf, size, offset, PORT_WRITE);
}
