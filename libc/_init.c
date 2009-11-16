#include <khaos/libc.h>
#include <khaos/signal.h>

void _cini() {
	init_heap();
	khsignal_init();
}
