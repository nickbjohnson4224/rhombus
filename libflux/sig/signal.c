/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/abi.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/heap.h>
#include <flux/proc.h>
#include <flux/mmap.h>

static volatile sig_handler_t signal_handler[MAXSIGNAL];

void signal_init(void) {
	extern void signal_handle(void);

	_sctl(SCTL_HANDLE, 0, (uint32_t) signal_handle);
}

int fire(uint32_t target, uint32_t signal, struct request *req) {
	return _fire(target, signal, req);
}

bool signal_queue(uint32_t signal, bool value) {
	return _sctl(SCTL_POLICY, signal, (value) ? 1 : 0);
}

struct request *signal_recv(uint32_t signal) {
	req_t *req;
	uintptr_t grant;

	req = ralloc();
	grant = _mail(signal, -1, 0);

	if (!grant) {
		return NULL;
	}

	emap(req, grant, PROT_READ | PROT_WRITE);

	return req;
}

void signal_register(uint32_t signal, sig_handler_t handler) {
	signal_handler[signal] = handler;
}

void signal_redirect(uint32_t source, uint32_t signal, void *grant) {
	req_t *req;

	req = ralloc();
	emap(req, (uintptr_t) grant, PROT_READ | PROT_WRITE);

	if (signal_handler[signal]) {
		signal_handler[signal](source, req);
	}
	
	else {
		if (signal == SIG_REPLY) {
			if (req) {
				rfree(req);
			}
		}
		else {
			if (!req) {
				req = ralloc();
			}

			req->format = REQ_ERROR;
			fire(source, SIG_REPLY, req_cksum(req));
		}
	}
}


