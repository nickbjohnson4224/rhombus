/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

size_t write(struct file *fd, void *buf, size_t size, uint64_t offset) {
	req_t *req, *res;
	uint8_t *data = (void*) buf;
	uint16_t datasize;
	size_t oldsize, i = 0;

	oldsize = size;

	sighold(SIG_REPLY);

	req = ralloc();

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = fd->resource;
		req->transid  = i;
		req->format   = REQ_WRITE;
		req->fileoff  = offset;

		arch_memcpy(req_getbuf(req), data, datasize);

		fire(fd->target, SIG_WRITE, req_cksum(req));

		res = sigpulltc(SIG_REPLY, i, fd->target);

		if (res->format == REQ_ERROR) {
			return (oldsize - size);
		}

		data    = &data[res->datasize];
		size   -= res->datasize;
		offset += res->datasize;

		rfree(res);
		i++;
	}

	rfree(req);

	sigfree(SIG_REPLY);

	return oldsize;
}
