/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/request.h>

size_t query(int fd, void *rbuf, void *sbuf, size_t size) {
	struct file *file;
	uint8_t *send_data;
	uint8_t *recv_data;
	event_t old_handler;
	size_t oldsize;
	size_t i;
	req_t *req;
	req_t *res;

	oldsize   = size;
	send_data = sbuf;
	recv_data = rbuf;
	file      = fdget(fd);
	res       = NULL;
	req       = ralloc();
	i         = 0;

	old_handler = event(PORT_REPLY, NULL);

	req_setbuf(req, STDOFF, size);
	req->resource = file->resource;
	req->format   = REQ_WRITE;

	arch_memcpy(req_getbuf(req), send_data, size);

	send(PORT_QUERY, file->target, req_cksum(req));
	res = waits(PORT_REPLY, file->target);

	if (res->format == REQ_ERROR) {
		rfree(res);

		event(PORT_REPLY, old_handler);
		return 0;
	}

	size = res->datasize;
	arch_memcpy(recv_data, req_getbuf(res), size);

	rfree(res);
	rfree(req);

	event(PORT_REPLY, old_handler);

	return size;
}
