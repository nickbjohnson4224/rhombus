/* Copyright 2009, 2010 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <_libc.h>
#include <mmap.h>

static volatile signal_handler_t sighandlers[MAXSIGNAL];
static volatile uint8_t sigcount[MAXSIGNAL]; /* Used for waiting */
static volatile uint8_t block_count = 0;

void siginit(void) {
	extern void sighand(void);

	_hand((uint32_t) sighand);
}

void sigblock(void) {
	_ctrl(CTRL_SIGNAL, CTRL_SIGNAL);
	block_count++;

	if (block_count != 1) {
		_ctrl(CTRL_NONE, CTRL_SIGNAL); 
	}
}

void sigunblock(void) {
	if (block_count == 0) return;
	
	_ctrl(CTRL_SIGNAL, CTRL_SIGNAL);
	block_count--;

	if (block_count == 0) {
		_ctrl(CTRL_NONE, CTRL_SIGNAL);
	}
}

int fire(uint32_t target, uint16_t signal, struct request *req) {
	int ret = _fire(target, signal, req, 0);
	if (req) req_free(req);
	return ret;
}

void tail(uint32_t target, uint16_t signal, struct request *req) {
	if (req) req_free(req);
	while (_fire(target, signal, req, FIRE_TAIL)) sleep();
}

void sigregister(uint16_t signal, signal_handler_t handler) {
	sighandlers[signal] = handler;
}

void sigredirect(uint32_t source, uint32_t signal, void *grant) {
	if (signal > MAXSIGNAL) return;
	sigcount[signal]++;

	if (sighandlers[signal]) {
		sighandlers[signal](source, req_catch(grant));
	}
	else if (signal != SIG_ERROR && source != info(0)) {
		tail(source, SIG_ERROR, NULL);
	}
}

void wreset(uint16_t signal) {
	sigcount[signal] = 0;
}

void wait(uint16_t signal) {
	while (!sigcount[signal]) sleep();
	sigcount[signal]--;
}

struct request *req_alloc(void) {
	return _heap_req_alloc();
}

void req_free(struct request *r) {
	_heap_req_free(r);
}

struct request *req_catch(void *grant) {
	void *vaddr = _heap_req_alloc();
	emap(vaddr, (uintptr_t) grant, PROT_READ | PROT_WRITE);
	return vaddr;
}

struct request *req_checksum(struct request *r) {
	uint32_t *reqflat = (void*) r;
	size_t i;
	uint32_t checksum = 0;

	for (i = 1; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	r->checksum = checksum;
	return r;
}

bool req_check(struct request *r) {
	uint32_t *reqflat = (void*) r;
	uint32_t checksum = 0;
	size_t i;

	for (i = 0; i < sizeof(struct request) / sizeof(uint32_t); i++) {
		checksum ^= reqflat[i];
	}

	return ((checksum) ? false : true);
}

