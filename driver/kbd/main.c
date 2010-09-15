/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <ipc.h>
#include <proc.h>
#include <driver.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "keyboard.h"

void keyboard_irq (struct packet *packet, uint8_t port, uint32_t caller);
void keyboard_read(struct packet *packet, uint8_t port, uint32_t caller);

int main() {

	when(PORT_IRQ,  keyboard_irq);
	when(PORT_READ, keyboard_read);
	rirq(1);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}

void keyboard_irq(struct packet *packet, uint8_t port, uint32_t caller) {
	static bool shift = false;
	uint8_t scan;
	char c;

	if (caller == 0) {
		scan = inb(0x60);

		if (scan & 0x80) {
			if (keymap[scan & 0x7F] == '\0') {
				shift = false;
			}
		}

		else if (keymap[scan & 0x7F] == '\0') {
			shift = true;
		}

		else {
			if (shift) {
				c = keymap[scan + 58];
			}
			else {
				c = keymap[scan];
			}

			fwrite(&c, sizeof(char), 1, stdout);
			push_char(c);
		}
	}
}

void keyboard_read(struct packet *packet, uint8_t port, uint32_t caller) {
	char *data;
	size_t offset;
	
	if (!packet) {
		return;
	}

	data = pgetbuf(packet);
	offset = 0;

	for (offset = 0; offset < packet->data_length; offset++) {
		data[offset] = pop_char();

		if (data[offset] == 'D') {
			psetbuf(&packet, 0);
			break;
		}
	}

	psend(PORT_REPLY, caller, packet);
}
