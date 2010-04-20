/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details 
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

size_t read(int fd, void *buf, size_t size, uint64_t offset) {
	struct request *req, *res;
	struct file *file = fdget(fd);
	uint8_t *data = (void*) buf;
	uint16_t datasize;
	size_t oldsize, i = 1;
	uint32_t old_policy;

	oldsize = size;
	req = ralloc();
	old_policy = signal_policy(SIG_REPLY, POLICY_QUEUE);

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req_setbuf(req, STDOFF, datasize);
		req->resource = file->resource;
		req->transid  = i;
		req->format   = REQ_READ;
		req->fileoff  = offset;

		fire(file->target, SIG_READ, req_cksum(req));

		res = signal_waits(SIG_REPLY, file->target, false);

		if (res->format == REQ_ERROR) {
			rfree(res);

			signal_policy(SIG_REPLY, old_policy);
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
	signal_policy(SIG_REPLY, old_policy);

	return oldsize;
}
