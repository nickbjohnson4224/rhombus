/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/signal.h>
#include <flux/request.h>
#include <flux/proc.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <driver/keyboard.h>

static void keyboard_init(device_t selector);
static void keyboard_halt(void);

static void keyboard_read(uint32_t caller, req_t *req);
static void keyboard_hand(uint32_t caller, req_t *req);

static const char dnkmap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
static const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

static uint32_t mutex_buffer;
static volatile char buffer[1024];
static volatile size_t buffer_top;
static volatile bool shift = false;

struct driver_interface keyboard = {
	keyboard_init,
	keyboard_halt,
	NULL,
	0,
};

static void keyboard_init(device_t selector) {

	signal_policy(SSIG_IRQ, POLICY_EVENT);
	signal_policy(SIG_READ, POLICY_EVENT);

	signal_register(SSIG_IRQ, keyboard_hand);
	signal_register(SIG_READ, keyboard_read);

	rirq(1);
}

static void keyboard_halt(void) {
	return;
}

static void keyboard_read(uint32_t caller, req_t *req) {

	if (!req_check(req)) {
		if (!req) req = ralloc();
		req->format = REQ_ERROR;
		tail(caller, SIG_REPLY, req_cksum(req));
	}

	while (!buffer_top) sleep();

	mutex_spin(&mutex_buffer);

	if (req->datasize > buffer_top) {
		req->datasize = buffer_top;
	}

	req->dataoff = STDOFF;
	memcpy(req_getbuf(req), (void*) buffer, req->datasize);
	
	if (req->datasize == buffer_top) {
		buffer_top = 0;
	}

	mutex_free(&mutex_buffer);

	req->format = REQ_WRITE;
	tail(caller, SIG_REPLY, req_cksum(req));
}

static void keyboard_hand(uint32_t caller, struct request *req) {
	uint8_t scan;

	scan = inb(0x60);

	mutex_spin(&mutex_buffer);

	if (scan & 0x80) {
		if (dnkmap[scan & 0x7F] == '\0') {
			shift = false;
		}
	}

	else if (dnkmap[scan & 0x7F] == '\0') {
		shift = true;
	}

	else {
		buffer[buffer_top++] = ((shift) ? upkmap[scan] : dnkmap[scan]);

		fwrite((char*) &buffer[buffer_top-1], 1, 1, stdout);
	}

	mutex_free(&mutex_buffer);

	return;
}
