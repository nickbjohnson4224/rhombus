/* Copyright 2009, 2010 Nick Johnson */

#include <flux/flux.h>
#include <flux/heap.h>

static volatile struct held_signal {
	req_t *req;
	uint32_t caller;
	uint32_t signal;
	uint32_t transid;
	struct held_signal *next;
} *sigqueue[MAXSIGNAL];

static uint32_t sigmask(void);

static volatile sig_handler_t 	sighandler    [MAXSIGNAL];
static volatile uint8_t			sighold_count [MAXSIGNAL];
static volatile uint8_t 	 	sigblock_count[MAXSIGNAL];
static volatile uint8_t 		allblock_count = 0;

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
		hs->transid = req->transid;

		hs->next = (void*) sigqueue[signal];
		sigqueue[signal] = hs;

		sigblock(false, VSIG_ALL);
	}

	else if (sighandler[signal]) {
		sighandler[signal](source, req);
	}
	
	else if (signal != SIG_REPLY && source != info(0)) {
		if (!req) {
			req = ralloc();
		}

		req->format = REQ_ERROR;
		tail(source, SIG_REPLY, req_cksum(req));
	}
}

struct request *sigpull(uint16_t signal) {
	struct held_signal *hs;
	struct request *req;

	while (!sigqueue[signal]) sleep();

	sigblock(true, VSIG_ALL);

	hs = (void*) sigqueue[signal];
	req = hs->req;
	sigqueue[signal] = hs->next;
	heap_free(hs);

	sigblock(false, VSIG_ALL);

	return req;
}

void sighold(uint16_t signal) {
	sighold_count[signal]++;
}

void sigfree(uint16_t signal) {
	sighold_count[signal]--;
}

req_t *req_cksum(req_t *r) {
	uint32_t *reqflat = (void*) r;
	size_t i;
	uint32_t checksum = 0;

	for (i = 1; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	r->checksum = checksum;
	return r;
}

bool req_check(req_t *r) {
	uint32_t *reqflat = (void*) r;
	uint32_t checksum = 0;
	size_t i;

	if (!r) {
		return false;
	}

	for (i = 0; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	if (checksum) {
		return false;
	}

	if (r->format == REQ_WRITE && 
		(r->dataoff + r->datasize > PAGESZ || r->dataoff < 32)) {
		return false;
	}

	return true;
}

bool req_setbuf(req_t *r, uint16_t offset, uint16_t size) {
	if (!req_check(r) || offset < 32 || offset + size > PAGESZ) return false;

	r->dataoff = offset;
	r->datasize = size;

	return true;
}

uint8_t *req_getbuf(req_t *r) {
	return &r->reqdata[r->dataoff - sizeof(req_t)];
}
