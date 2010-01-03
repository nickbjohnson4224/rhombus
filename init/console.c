#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>
#include <flux.h>
#include <driver.h>
#include <signal.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

static void callback_save(struct request *r);
static volatile struct request *saved = NULL;

void console_handler(uint32_t caller, void *grant) {
	extern void swrite(const char*);
	swrite("*");
	keyboard.handler();
}

void console_init() {
	terminal.init(0);
	keyboard.init(0);

	rirq(keyboard.irq);
	sigregister(SSIG_IRQ, console_handler);
}

size_t console_write(char *buffer, size_t length) {
	struct request *r = req_alloc();
	struct localrequest l;
	uint8_t *data = (void*) ((uintptr_t) r + sizeof(struct request));

	l.resource = 0;
	l.transaction = 0;
	l.offset = 0;
	l.datasize = length;
	l.data = data;

	memcpy(data, buffer, length);

	req_encode(&l, r);
	terminal.write(r, callback_save);

	req_decode((void*) saved, &l);
	length = *((size_t*) l.data);

	free((void*) saved);
	saved = NULL;

	return length;
}

size_t console_read(char *buffer, size_t length) {
	struct request *r = req_alloc();
	struct localrequest l;
	uint8_t *data = (void*) ((uintptr_t) r + sizeof(struct request));

	l.resource = 0;
	l.transaction = 0;
	l.offset = 0;
	l.datasize = 4;
	l.data = data;

	*((size_t*) data) = length;
	
	req_encode(&l, r);
	keyboard.read(r, callback_save);

	req_decode((void*) saved, &l);
	memcpy(buffer, l.data, l.datasize);
	length = l.datasize;

	free((void*) saved);
	saved = NULL;

	return length;
}

static void callback_save(struct request *r) {
	saved = r;
}
