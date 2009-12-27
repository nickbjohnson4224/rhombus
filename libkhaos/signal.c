#include <stdint.h>
#include <string.h>
#include <khaos/signal.h>
#include <khaos/kernel.h>

static khsig_handler_t khsig_handlers[256];
static volatile uint8_t khsig_count[256]; /* Used for waiting */

void khsig_init(void) {
	extern void khsig_handler(void);

	_hand((uint32_t) khsig_handler);

	memclr(&khsig_handlers, sizeof(khsig_handler_t) * 256);
	memclr(&khsig_count, sizeof(uint8_t) * 256);
}

void khsig_block(void) {
	_ctrl(CTRL_SIGNAL, CTRL_SIGNAL); 
}

void khsig_unblock(void) {
	_ctrl(CTRL_NONE, CTRL_SIGNAL);
}

int khsig_send(uint32_t target, uint8_t signal, uint32_t args[4]) {
	return ssnd_call(target, signal, args[0], args[1], args[2], args[3], 0);
}

int khsig_asend(uint32_t target, uint8_t signal, uint32_t args[4]) {
	return ssnd_call(target, signal, args[0], args[1], args[2], args[3], 1);
}

void khsig_register(uint8_t signal, khsig_handler_t handler) {
	khsig_handlers[signal] = handler;
}

void khsig_redirect(uint32_t source, uint32_t signal, uint32_t args[4]) {
	khsig_count[signal]++;

	if (khsig_handlers[signal]) {
		khsig_handlers[signal](source, args);
	}
}

void khsig_wreset(uint32_t signal) {
	khsig_count[signal] = 0;
}

void khsig_wait(uint32_t signal) {
	while (!khsig_count[signal]);
	khsig_count[signal]--;
}
