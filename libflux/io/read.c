/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

size_t read(struct file *fd, void *buf, size_t size, uint64_t offset) {
	struct request *req, *res;
	uint8_t *data = (void*) buf;
	uint16_t datasize;
	size_t oldsize, i = 1;

	oldsize = size;

	req = ralloc();

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = fd->resource;
		req->transid  = i;
		req->format   = REQ_READ;
		req->fileoff  = offset;

		fire(fd->target, SIG_READ, req_cksum(req));

		res = sigpull(SIG_REPLY);

		if (res->format == REQ_ERROR) {
			rfree(res);
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

	return oldsize;
}
