/*
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/io.h>
#include <flux/signal.h>
#include <flux/request.h>

size_t query(int fd, void *rbuf, void *sbuf, size_t size) {
	struct file *file;
	uint8_t *send_data;
	uint8_t *recv_data;
	uint32_t old_policy;
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

	old_policy = signal_policy(SIG_REPLY, POLICY_QUEUE);

	req_setbuf(req, STDOFF, size);
	req->resource = file->resource;
	req->format   = REQ_WRITE;

	arch_memcpy(req_getbuf(req), send_data, size);

	fire(file->target, SIG_QUERY, req_cksum(req));
	res = signal_waits(SIG_REPLY, file->target, false);

	if (res->format == REQ_ERROR) {
		rfree(res);

		signal_policy(SIG_REPLY, old_policy);
		return 0;
	}

	size = res->datasize;
	arch_memcpy(recv_data, req_getbuf(res), size);

	rfree(res);
	rfree(req);

	signal_policy(SIG_REPLY, old_policy);

	return size;
}
