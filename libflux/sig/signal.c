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

volatile sig_handler_t signal_handler[MAXSIGNAL];

void signal_init(void) {
	extern void signal_handle(void);

	_sctl(SCTL_HANDLE, 0, (uint32_t) signal_handle);
}

int fire(uint32_t target, uint32_t signal, struct request *req) {
	return _fire(target, signal, req, false);
}

int tail(uint32_t target, uint32_t signal, struct request *req) {
	return _fire(target, signal, req, false);
}

uint32_t signal_policy(uint32_t signal, uint32_t policy) {
	return _sctl(SCTL_POLICY, signal, policy);
}

struct request *signal_recvs(uint32_t signal, uint32_t source) {
	req_t *req;
	uintptr_t grant;

	req = ralloc();
	grant = _mail(signal, source, 0);

	if (grant & 1) {
		return NULL;
	}

	emap(req, grant, PROT_READ | PROT_WRITE);

	return req;
}

struct request *signal_recv(uint32_t signal) {
	return signal_recvs(signal, 0);
}

struct request *signal_waits(uint32_t signal, uint32_t source, bool dosleep) {
	struct request *req;

	req = NULL;

	while (!req) {
		req = signal_recv(signal);
		if (dosleep) {
			sleep();
		}
	}

	return req;
}

struct request *signal_wait(uint32_t signal, bool dosleep) {
	return signal_waits(signal, 0, dosleep);
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
			tail(source, SIG_REPLY, req_cksum(req));
		}
	}
}
