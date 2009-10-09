#include <khaos/signal.h>
#include <khaos/kernel.h>

void khsignal_block(void) {
	sblk_call(0);
}

void khsignal_unblock(void) {
	sblk_call(1);
}

int khsignal_send(uint32_t target, uint8_t signal, uint32_t args[4]) {
	return ssnd_call(target, signal, args[0], args[1], args[2], args[3], 0);
}

int khsignal_asend(uint32_t target, uint8_t signal, uint32_t args[4]) {
	return ssnd_call(target, signal, args[0], args[1], args[2], args[3], 1);
}
