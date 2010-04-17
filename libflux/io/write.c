/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

size_t write(struct file *fd, void *buf, size_t size, uint64_t offset) {
	uint8_t *data;
	uint32_t old_policy;
	uint16_t datasize;
	size_t oldsize;
	size_t i;
	req_t *req;
	req_t *res;

	oldsize = size;
	data    = buf;
	res     = NULL;
	req     = ralloc();
	i       = 0;

	old_policy = signal_policy(SIG_REPLY, POLICY_QUEUE);

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = fd->resource;
		req->transid  = i;
		req->format   = REQ_WRITE;
		req->fileoff  = offset;

		arch_memcpy(req_getbuf(req), data, datasize);

		fire(fd->target, SIG_WRITE, req_cksum(req));
	
		res = signal_waits(SIG_REPLY, fd->target, false);

		if (res->format == REQ_ERROR) {
			rfree(res);

			signal_policy(SIG_REPLY, old_policy);
			return (oldsize - size);
		}

		data    = &data[res->datasize];
		size   -= res->datasize;
		offset += res->datasize;

		rfree(res);
		i++;
	}

	rfree(req);
	signal_policy(SIG_REPLY, old_policy);

	return oldsize;
}
