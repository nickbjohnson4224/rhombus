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

static volatile struct held_signal {
	req_t *req;
	uint32_t caller;
	uint32_t signal;
	uint32_t transid;
	struct held_signal *next;
} *sigqueue[MAXSIGNAL];

static uint32_t sigmask(void);

static volatile sig_handler_t 	sighandler    [MAXSIGNAL];

static volatile uint8_t sighold_count [MAXSIGNAL];
static volatile uint8_t sigblock_count[MAXSIGNAL];
static volatile uint8_t allblock_count = 0;

void siginit(void) {
	extern void sighand(void);

	_hand((uint32_t) sighand);
}

static uint32_t sigmask(void) {
	uint32_t mask = 0;
	size_t i;

	if (allblock_count) {
		return 0xFFFFFFFF;
	}
	
	for (i = 0; i < MAXSIGNAL; i++) {
		if (sigblock_count[i] && !sighold_count[i]) {
			mask |= 1 << i;
		}
	}

	return mask;
}

void sigblock(bool v, uint8_t signal) {
	
	_ctrl(0xFFFFFFFF, 0xFFFFFFFF, 1);

	if (signal == VSIG_ALL) {
		if (v) allblock_count++;
		else   allblock_count--;
	}

	if (signal == VSIG_REQ) {
		if (v) {
			sigblock_count[SIG_READ] ++;
			sigblock_count[SIG_WRITE]++;
			sigblock_count[SIG_INFO] ++;
			sigblock_count[SIG_CTRL] ++;
		}
		else {
			sigblock_count[SIG_READ] --;
			sigblock_count[SIG_WRITE]--;
			sigblock_count[SIG_INFO] --;
			sigblock_count[SIG_CTRL] --;
		}
	}

	else {
		if (signal > MAXSIGNAL) {
			_ctrl(sigmask(), 0xFFFFFFFF, 1);
			return;
		}
		if (v) sigblock_count[signal]++;
		else   sigblock_count[signal]--;
	}

	_ctrl(sigmask(), 0xFFFFFFFF, 1);

}

int fire(uint32_t target, uint8_t signal, struct request *req) {
	return _fire(target, signal, req, 0);
}

void tail(uint32_t target, uint8_t signal, struct request *req) {
	if (req) rfree(req);
	while (_fire(target, signal, req, FIRE_TAIL)) sleep();
}

void sigregister(uint16_t signal, sig_handler_t handler) {
	sighandler[signal] = handler;
}

void sigredirect(uint32_t source, uint32_t signal, void *grant) {
	req_t *req;
	struct held_signal *hs;

	req = ralloc();
	emap(req, (uintptr_t) grant, PROT_READ | PROT_WRITE);

	if (sighold_count[signal]) {
		sigblock(true, VSIG_ALL);

		hs = heap_malloc(sizeof(struct held_signal));

		hs->req = req;
		hs->caller = source;
		hs->signal = signal;
		if (req) hs->transid = req->transid;

		hs->next = (void*) sigqueue[signal];
		sigqueue[signal] = hs;

		sigblock(false, VSIG_ALL);
	}

	else if (sighandler[signal]) {
		sighandler[signal](source, req);
	}
	
	else if (signal != SIG_REPLY && source != pinfo(INFO_GPID)) {
		if (!req) {
			req = ralloc();
		}

		req->format = REQ_ERROR;
		tail(source, SIG_REPLY, req_cksum(req));
	}
}

req_t *sigfind(uint16_t signal, uint32_t trans, uint32_t caller) {
	struct held_signal *hs, *old;
	struct request *req = NULL;

	while (!sigqueue[signal]) sleep();

	sigblock(true, signal);

	hs = (void*) sigqueue[signal];

	if ((trans == 0 || hs->transid == trans) 
			&& (caller == 0 || hs->caller == caller)) {
		req = hs->req;
		sigqueue[signal] = hs->next;
	}

	while (req == NULL) {

		if (!hs->next) {
			sigblock(false, signal);
			return NULL;
		}

		if ((trans == 0 || hs->next->transid == trans)
				&& (caller == 0 || hs->caller == caller)) {
			req = hs->next->req;
			old = hs->next;
			hs->next = old->next;
			heap_free(old);
		}
	}

	sigblock(false, signal);

	return req;
}

req_t *sigpulltc(uint16_t signal, uint32_t trans, uint32_t caller) {
	struct request *req = NULL;

	while (!(req = sigfind(signal, trans, caller))) sleep();

	return req;
}

struct request *sigpull(uint16_t signal) {
	return sigpulltc(signal, 0, 0);
}

req_t *sigpullt(uint16_t signal, uint32_t trans) {
	return sigpulltc(signal, trans, 0);
}

req_t *sigpullc(uint16_t signal, uint32_t caller) {
	return sigpulltc(signal, 0, caller);
}

void sighold(uint16_t signal) {
	sighold_count[signal]++;
}

void sigfree(uint16_t signal) {
	sighold_count[signal]--;
}
