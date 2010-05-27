/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/ipc.h>
#include <flux/packet.h>
#include <flux/proc.h>
#include <flux/driver.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "keyboard.h"

void keyboard_irq (uint32_t source, struct packet *packet);
void keyboard_read(uint32_t source, struct packet *packet);

int main() {

	when(PORT_IRQ,  keyboard_irq);
	when(PORT_READ, keyboard_read);
	rirq(1);

	printf("kbd: ready\n");

	send(PORT_SYNC, 1, NULL);
	_done();

	return 0;
}

void keyboard_irq(uint32_t source, struct packet *packet) {
	static bool shift = false;
	uint8_t scan;
	char c;

	if (source == 0) {
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

void keyboard_read(uint32_t source, struct packet *packet) {
	char *line;
	char *data;
	size_t offset;
	
	if (!packet) {
		return;
	}

	data = packet_getbuf(packet);
	offset = 0;

	for (offset = 0; offset < packet->data_length; offset++) {
		data[offset] = pop_char();
	}

	send(PORT_REPLY, source, packet);
}
