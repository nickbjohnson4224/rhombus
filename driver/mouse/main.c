/*
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail dot com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>

#define USE_IRQ 0

struct event_list *event_list;
uint8_t bytes[3];
size_t curbyte;
int prevbuttons;
#if USE_IRQ
bool mutex;
#else
int dx, dy;
int buttons;
#endif

void wait_signal() {
	while (inb(0x64) & 0x2);
}

void command(uint8_t byte) {
	wait_signal();
	outb(0x64, 0xd4);
	wait_signal();
	outb(0x60, byte);
	while ((inb(0x64) & 0x21) == 0x21 && inb(0x60) != 0xfa); // ACK
}

char *mouse_register(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *file;

	file = index_get(index);

	mutex_spin(&file->mutex);
	event_list = event_list_add(event_list, source);
	mutex_free(&file->mutex);

	return strdup("T");
}

char *mouse_deregister(uint64_t source, uint32_t index, int argc, char **argv) {
	struct resource *file;

	file = index_get(index);

	mutex_spin(&file->mutex);
	event_list = event_list_del(event_list, source);
	mutex_free(&file->mutex);

	return strdup("T");
}

static inline void send_event_delta(int dx, int dy) {
	char *event;

	if (dx || dy) {
		event = saprintf("mouse delta %d %d", dx, dy);
		eventl(event_list, event);
		free(event);
	}
}

static inline void send_event_button(int buttons) {
	char *event;

	if (buttons != prevbuttons) {
		event = saprintf("mouse button %d", buttons);
		eventl(event_list, event);
		free(event);
		prevbuttons = buttons;
	}
}

static inline void read_byte() {
#if USE_IRQ
	int dx = 0, dy = 0;
	int buttons;
#endif

	bytes[curbyte++] = inb(0x60);

	if (curbyte == 1) {
		if (!(bytes[0] & 0x08)) {
			// Out of sync
			curbyte = 0;
			return;
		}
		buttons = bytes[0] & 0x7;
#if USE_IRQ
		mutex_free(&mutex);
		send_event_button(buttons);
#endif
	}
	if (curbyte == 3) {
		curbyte = 0;
		dx += bytes[1] - ((bytes[0] & 0x10) ? 256 : 0);
		dy -= bytes[2] - ((bytes[0] & 0x20) ? 256 : 0);
#if USE_IRQ
		mutex_free(&mutex);
		send_event_delta(dx, dy);
#endif
	}
}

#if USE_IRQ
void mouse_irq(struct msg *msg) {
	mutex_spin(&mutex);
	read_byte();
	mutex_free(&mutex);
}
#endif

int main(int argc, char **argv) {
#if USE_IRQ
	uint8_t status;
#else
	bool first;
#endif

	command(0xa8);  // enable aux. PS2
	command(0xf6);  // load default config
	command(0xf4);  // enable mouse
	command(0xf3);  // set sample rate:
	outb(0x60, 10); // 10 samples per second

#if USE_IRQ
	wait_signal();
	outb(0x64, 0x20);
	status = inb(0x60);
	status &= ~0x20; // enable mouse clock
	status |= 0x02;  // enable IRQ12
	wait_signal();
	outb(0x64, 0x60);
	wait_signal();
	outb(0x60, status);
	wait_signal();
#endif

	index_set(0, resource_cons(FS_TYPE_FILE | FS_TYPE_EVENT, PERM_READ | PERM_WRITE));

	rcall_set("register",   mouse_register);
	rcall_set("deregister", mouse_deregister);
#if USE_IRQ
	rdi_set_irq(12, mouse_irq);
#endif
	rdi_init_all();

	fs_plink("/dev/mouse", RP_CONS(getpid(), 0), NULL);
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);

#if USE_IRQ
	_done();
#else
	while (1) {
		first = true;
		while ((inb(0x64) & 0x21) != 0x21) {
			if (first) {
				send_event_delta(dx, dy);
				send_event_button(buttons);
				dx = dy = 0;
			}
			first = false;
			sleep();
		}

		read_byte();
	}
#endif
	
	return 0;
}
