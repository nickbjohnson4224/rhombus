/* Copyright 2009, 2010 Nick Johnson */

#include <flux.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <_libc.h>
#include <mmap.h>

static volatile struct held_signal {
	req_t *req;
	uint32_t caller;
	uint32_t signal;
	uint32_t transid;
	struct held_signal *next;
} *sigheld[MAXSIGNAL];

static volatile sig_handler_t 	sighandlers[MAXSIGNAL];
static volatile bool 		 	sigholders[MAXSIGNAL];
static volatile uint8_t 	 	block_count = 0;

void siginit(void) {
	extern void sighand(void);

	_hand((uint32_t) sighand);
}

void sigblock(bool v) {
	if (v) {
	
		_ctrl(CTRL_SIGNAL, CTRL_SIGNAL);
		block_count++;

		if (block_count != 1) {
			_ctrl(CTRL_NONE, CTRL_SIGNAL); 
		}
	}
	else {
		if (block_count == 0) return;
	
		_ctrl(CTRL_SIGNAL, CTRL_SIGNAL);
		block_count--;

		if (block_count == 0) {
			_ctrl(CTRL_NONE, CTRL_SIGNAL);
		}
	}
}

int fire(uint32_t target, uint16_t signal, struct request *req) {
	return _fire(target, signal, req, 0);
}

void tail(uint32_t target, uint16_t signal, struct request *req) {
	if (req) req_free(req);
	while (_fire(target, signal, req, FIRE_TAIL)) sleep();
}

void sigregister(uint16_t signal, sig_handler_t handler) {
	sighandlers[signal] = handler;
}

void sigredirect(uint32_t source, uint32_t signal, void *grant) {
	req_t *req = req_catch(grant);
	struct held_signal *hs = NULL;

	if (sigholders[signal] == true) {
		sigblock(true);

		hs = malloc(sizeof(struct held_signal));

		hs->req = req;
		hs->caller = source;
		hs->signal = signal;
		hs->transid = req->transid;

		hs->next = (void*) sigheld[signal];
		sigheld[signal] = hs;

		sigblock(false);
	}
	else if (sighandlers[signal]) {
		sighandlers[signal](source, req);
	}
	else if (signal != SIG_ERROR && source != info(0)) {
		tail(source, SIG_ERROR, NULL);
	}
}

struct request *sigpull(uint16_t signal) {
	struct held_signal *hs;
	struct request *req;

	while (!sigheld[signal]) sleep();

	sigblock(true);

	hs = (void*) sigheld[signal];
	req = hs->req;
	sigheld[signal] = hs->next;
	free(hs);

	sigblock(false);

	return req;
}

void sighold(uint16_t signal) {
	sigholders[signal] = true;
}

void sigfree(uint16_t signal) {
	sigholders[signal] = false;
}

req_t *req_alloc(void) {
	return _heap_req_alloc();
}

void req_free(req_t *r) {
	_heap_req_free(r);
}

req_t *req_catch(void *grant) {
	void *vaddr = req_alloc();
	emap(vaddr, (uintptr_t) grant, PROT_READ | PROT_WRITE);
	return vaddr;
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
