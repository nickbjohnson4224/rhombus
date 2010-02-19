/* Copyright 2010 Nick Johnson */

#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

#include <string.h>

size_t read(struct file *fd, void *buf, size_t size, uint64_t offset) {
	struct request *req, *res;
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
		req->format   = REQ_READ;
		req->fileoff  = offset;

		fire(fd->target, SIG_READ, req_cksum(req));

		res = sigpull(SIG_REPLY);

		if (res->format == REQ_ERROR) {
			return (oldsize - size);
		}

		memcpy(data, req_getbuf(res), res->datasize);

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
