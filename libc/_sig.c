/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <flux.h>

static signal_handler_t sighandlers[MAXSIGNAL];
static volatile uint8_t sigcount[MAXSIGNAL]; /* Used for waiting */

void siginit(void) {
	extern void sighand(void);

	_hand((uint32_t) sighand);
}

void sigblock(void) {
	_ctrl(CTRL_SIGNAL, CTRL_SIGNAL); 
}

void sigunblock(void) {
	_ctrl(CTRL_NONE, CTRL_SIGNAL);
}

int fire(uint32_t target, uint16_t signal, void *grant) {
	return _fire(target, signal, grant);
}

void sigregister(uint16_t signal, signal_handler_t handler) {
	sighandlers[signal] = handler;
}

void sigredirect(uint32_t source, uint32_t signal, void *grant) {
	if (signal > MAXSIGNAL) return;
	sigcount[signal]++;

	if (sighandlers[signal]) {
		sighandlers[signal](source, grant);
	}
}

void wreset(uint16_t signal) {
	sigcount[signal] = 0;
}

void wait(uint16_t signal) {
	while (!sigcount[signal]);
	sigcount[signal]--;
}

void rirq(uint8_t irq) {
	_ctrl(CTRL_IRQRD | CTRL_IRQ(irq), CTRL_IRQRD | CTRL_IRQMASK);
}
