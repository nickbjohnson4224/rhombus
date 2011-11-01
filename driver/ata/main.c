/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdint.h>
#include <string.h>

#include <rdi/core.h>
#include <rdi/arch.h>

struct ata_device {
	uint16_t id[256];
};

#define PORT_DATA	0
#define PORT_ERROR	1
#define PORT_COUNT	2
#define PORT_LBAL	3
#define PORT_LBAM	4
#define PORT_LBAH	5
#define PORT_DRIVE	6
#define PORT_STATUS	7
#define PORT_CTRL	0x206
#define PORT_ASTAT	0x206

#define STATUS_ERR	0x01
#define STATUS_DRQ	0x08
#define STATUS_SRV	0x10
#define STATUS_DF	0x20
#define STATUS_RDY	0x40
#define STATUS_BSY	0x80

uint16_t port_base = 0x1F0;
uint16_t drive_num = 42;

void ata_delay(void) {

	inb(port_base + PORT_STATUS);
	inb(port_base + PORT_STATUS);
	inb(port_base + PORT_STATUS);
	inb(port_base + PORT_STATUS);
}

void ata_select(int num) {
	
	if (num == drive_num) return;

	if (num > 8) return;
	port_base = (num & 4) ? ((num & 2) ? 0x168 : 0x1E8) : ((num & 2) ? 0x170 : 0x1F0);
	drive_num = num;
	
	outb(port_base + PORT_DRIVE, (num & 1) ? 0xB0 : 0xA0);
	ata_delay();
}

void ata_identify(uint16_t id[256]) {
	
	outb(port_base + PORT_COUNT, 0);
	outb(port_base + PORT_LBAL, 0);
	outb(port_base + PORT_LBAM, 0);
	outb(port_base + PORT_LBAH, 0);
	ata_delay();

	outb(port_base + PORT_STATUS, 0xEC);
	ata_delay();
	
	if (!inb(port_base + PORT_STATUS)) {
		printf("no drive\n");
		// no drive
		id[0] = 0xFF;
		return;
	}

	while (inb(port_base + PORT_STATUS) & STATUS_BSY);

	if (inb(port_base + PORT_LBAM) || inb(port_base + PORT_LBAH)) {
		printf("found non-ATA drive %d: %X %X\n", drive_num, inb(port_base + PORT_LBAM), inb(port_base + PORT_LBAH));
		// ATAPI
		id[0] = 0xFF;
		return;
	}

	while ((inb(port_base + PORT_STATUS) & (STATUS_DRQ | STATUS_ERR)) == 0);

	if (inb(port_base + PORT_STATUS) & STATUS_ERR) {
		// error
		id[0] = 0xFF;
		return;
	}

	for (int i = 0; i < 256; i++) {
		id[i] = inb(port_base + PORT_DATA);
	}
}

int main(int argc, char **argv) {
	struct ata_device dev[4];

	for (int i = 0; i < 4; i++) {
		
		ata_select(i);

		// perform IDENTIFY command
		ata_identify(dev[i].id);

		printf("port base: %X\n", port_base);

		if (dev[i].id[0] != 0xFF) {
			printf("drive %d:\n", i);
			printf("\tword 0:\t\t%X\n",	dev[i].id[0]);
			printf("\tword 83:\t%X\n", 	dev[i].id[83]);
			printf("\tword 88:\t%X\n", 	dev[i].id[88]);
			printf("\tword 93:\t%X\n", 	dev[i].id[93]);
			printf("\tword 60:\t%X\n", 	dev[i].id[60]);
			printf("\tword 61:\t%X\n", 	dev[i].id[61]);
			printf("\tword 100:\t%X\n",	dev[i].id[100]);
			printf("\tword 101:\t%X\n",	dev[i].id[101]);
			printf("\tword 102:\t%X\n",	dev[i].id[102]);
			printf("\tword 103:\t%X\n",	dev[i].id[103]);
		}
		else {
			printf("no drive %d\n", i);
		}
	}

	return 0;
}
