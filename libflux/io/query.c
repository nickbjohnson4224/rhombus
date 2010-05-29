/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>

size_t query(int fd, void *rbuf, void *sbuf, size_t size) {
	return psend(fd, rbuf, sbuf, size, 0, PORT_QUERY);
}
