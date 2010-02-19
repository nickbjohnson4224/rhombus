/* Copyright 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/mmap.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <driver/ata.h>

static uint32_t *ata_prdt[4];
static uint8_t  *ata_buf[4];

static void dma_init(void) {
	int i;

	ata_prdt[0] = malloc(0x1000);
	ata_buf[0]  = malloc(0x1000);

	for (i = 1; i < 4; i++) {
		ata_prdt[i] = &ata_prdt[0][i * ata_drive[i].sectsize / sizeof(uint32_t)];
		ata_buf[i]  = &ata_buf [0][i * ata_drive[i].sectsize / sizeof(uint8_t)];

		ata_prdt[i][0] = phys(ata_buf[i]);
		ata_prdt[i][1] = ata_drive[i].sectsize | (0x1000 << 16);

		if (ata_drive[i].flags & FLAG_EXIST) {
			if (ata_drive[i].flags & FLAG_ATAPI) {
				ata_select(i);
				outb(ata_base[i] + REG_FEATURE, 0x01);
			}
		}
	}
}

