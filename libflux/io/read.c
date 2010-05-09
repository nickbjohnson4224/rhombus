/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/ipc.h>
#include <flux/request.h>

size_t read(int fd, void *buf, size_t size, uint64_t offset) {
	struct request *req, *res;
	struct file *file = fdget(fd);
	uint8_t *data = (void*) buf;
	uint16_t datasize;
	size_t oldsize, i = 1;
	event_t old_handler;

	oldsize = size;
	req = ralloc();
	old_handler = event(PORT_REPLY, NULL);

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = file->resource;
		req->transid  = i;
		req->format   = REQ_READ;
		req->fileoff  = offset;

		send(PORT_READ, file->target, req_cksum(req));

		res = waits(PORT_REPLY, file->target);

		if (res->format == REQ_ERROR) {
			rfree(res);

			event(PORT_REPLY, old_handler);
			return (oldsize - size);
		}

		arch_memcpy(data, req_getbuf(res), res->datasize);

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
