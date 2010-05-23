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

static void keyboard_hand(uint32_t source, struct packet *packet);

static const char dnkmap[] = 
"\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 ";
static const char upkmap[] = 
"\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 ";

static uint32_t mutex_buffer;
static volatile char buffer[1024];
static volatile size_t buffer_top;
static volatile bool shift = false;

int main() {

	when(PORT_IRQ,  keyboard_hand);
	rirq(1);

	send(PORT_SYNC, 1, NULL);

	printf("Keyboard: ready\n");

	_done();

	return 0;
}

static void keyboard_hand(uint32_t source, struct packet *packet) {
	uint8_t scan;
	char c;

	if (source == 0) {
		scan = inb(0x60);

		if (scan & 0x80) {
			if (dnkmap[scan & 0x7F] == '\0') {
				shift = false;
			}
		}

		else if (dnkmap[scan & 0x7F] == '\0') {
			shift = true;
		}

		else {
			c = ((shift) ? upkmap[scan] : dnkmap[scan]);

			fwrite(&c, sizeof(char), 1, stdout);
		}
	}
}
