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

static volatile sig_handler_t sighandler [MAXSIGNAL];

void siginit(void) {
	extern void sighand(void);

	_sctl(SCTL_HANDLE, -1, (uint32_t) sighand);
}

int fire(uint32_t target, uint8_t signal, struct request *req) {
	return _fire(target, signal, req);
}

void sigregister(uint16_t signal, sig_handler_t handler) {
	sighandler[signal] = handler;
}

void sigredirect(uint32_t source, uint32_t signal, void *grant) {
	req_t *req;

	req = ralloc();
	emap(req, (uintptr_t) grant, PROT_READ | PROT_WRITE);

	if (sighandler[signal]) {
		sighandler[signal](source, req);
	}
	
	else {
		if (signal == SIG_REPLY) {
			rfree(req);
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

struct request *sigpull(uint16_t signal) {
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
