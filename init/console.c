#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>
#include <flux.h>
#include <driver.h>
#include <signal.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

extern void swrite(const char*);

static void callback_save(struct request *r);
static volatile struct request *saved = NULL;

void console_handler(uint32_t caller, void *grant) {
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

	r->resource   = 0;
	r->datasize   = length;
	r->format     = REQ_WRITE;

	memcpy(r->reqdata, buffer, length);

	req_checksum(r);
	terminal.write(r, callback_save);

	length = r->datasize;

	free((void*) saved);
	saved = NULL;

	return length;
}

size_t console_read(char *buffer, size_t length) {
	struct request *r = req_alloc();
	size_t oldlength = length;

	r->resource   = 0;
	r->datasize   = length;
	r->format     = REQ_READ;

	req_checksum(r);
	keyboard.read(r, callback_save);

	memcpy(buffer, r->reqdata, r->datasize);
	length = r->datasize;

	free((void*) saved);
	saved = NULL;

	if (length == 0) {
		wreset(SSIG_IRQ);
		wait(SSIG_IRQ);
		return console_read(buffer, oldlength);
	}

	return length;
}

static void callback_save(struct request *r) {
	saved = r;
}
