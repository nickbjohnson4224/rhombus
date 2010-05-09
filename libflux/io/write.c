/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/request.h>

size_t write(int fd, void *buf, size_t size, uint64_t offset) {
	struct file *file;
	uint8_t *data;
	event_t old_handler;
	uint16_t datasize;
	size_t oldsize;
	size_t i;
	req_t *req;
	req_t *res;

	oldsize = size;
	data    = buf;
	file    = fdget(fd);
	res     = NULL;
	req     = ralloc();
	i       = 0;

	old_handler = event(PORT_REPLY, NULL);

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = file->resource;
		req->transid  = i;
		req->format   = REQ_WRITE;
		req->fileoff  = offset;

		arch_memcpy(req_getbuf(req), data, datasize);

		send(PORT_WRITE, file->target, req_cksum(req));
		res = waits(PORT_REPLY, file->target);

		if (res->format == REQ_ERROR) {
			rfree(res);

			event(PORT_REPLY, old_handler);
			return (oldsize - size);
		}

		data    = &data[res->datasize];
		size   -= res->datasize;
		offset += res->datasize;

		rfree(res);
		i++;
	}

	rfree(req);
	event(PORT_REPLY, old_handler);

	return oldsize;
}
