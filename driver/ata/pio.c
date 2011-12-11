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

#include "ata.h"

static void ata_pio_read   (uint8_t drive, uint64_t sector, uint16_t *buffer);
static void ata_pio_write  (uint8_t drive, uint64_t sector, uint16_t *buffer);
static void atapi_pio_read (uint8_t drive, uint64_t sector, uint16_t *buffer);
static void atapi_pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer);

static void ata_pio_read(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	size_t i;
	int lba48 = 0;

	lba48 = ata_send_lba(drive, sector);
	outb(ata[drive].base + REG_CMD, (lba48) ? CMD_READ_PIO48 : CMD_READ_PIO);

	if (ata_wait(drive)) {
		printf("drive error\n");
		return;
	}

	for (i = 0; i < (uint32_t) (1 << ata[drive].sectsize) >> 1; i++) {
		buffer[i] = inw(ata[drive].base + REG_DATA);
		printf("read %x\n", buffer[i]);
	}
	ata_sleep400(drive);
}

static void ata_pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer);

static void atapi_pio_read(uint8_t drive, uint64_t sector, uint16_t *buffer);

static void atapi_pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer);

void pio_read(uint8_t drive, uint64_t sector, uint16_t *buffer) {
	
	if (ata[drive].flags & FLAG_ATAPI) {
		
	}
	else {
		ata_pio_read(drive, sector, buffer);
	}
}

void pio_write(uint8_t drive, uint64_t sector, uint16_t *buffer);
