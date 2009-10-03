#include <khsignal.h>
#include <kernel.h>
#include <string.h>

static void (*khsignal_handlers[256])(uint32_t, uint32_t[4]);
static volatile uint8_t khsignal_count[256]; /* Used for wait */

void khsignal_redirect(uint32_t source, uint8_t signal, uint32_t args[4]) {
	khsignal_count[signal]++;
	if (khsignal_handlers[signal]) {
		khsignal_handlers[signal](source, args);
	}
}

void khsignal_register(uint8_t signal, void (*handler) (uint32_t source, uint32_t args[4])) {
	khsignal_handlers[signal] = handler;
}

void khsignal_wait(uint32_t signal) {
	while (!khsignal_count[signal]);
	khsignal_count[signal]--;
}

void khsignal_init(void) {
	extern uint32_t khsignal_handler;
	size_t i;

	sreg_call((uint32_t) &khsignal_handler);
	for (i = 0; i < 256; i++) {
		khsignal_handlers[i] = NULL;
		khsignal_count[i] = 0;
	}
}
