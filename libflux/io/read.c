/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>

size_t read(int fd, void *buf, size_t size, uint64_t offset) {
	return psend(fd, buf, NULL, size, offset, PORT_READ);
}
