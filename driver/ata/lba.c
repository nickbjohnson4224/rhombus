/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <flux.h>

#include <driver/ata.h>

bool ata_send_lba(uint8_t drive, uint64_t sector) {
	uint8_t lba[6], head;
	bool lba48 = false;

	/* format LBA bytes from sector index */
	#ifdef ATACONF_LONG
	if (sector > 0x10000000 && ata_drive[drive].flags & FLAG_LONG) {
		/* Use LBA48 */
		lba48 = true;
		lba[5] = (sector >> 40) & 0xFF;
		lba[4] = (sector >> 32) & 0xFF;
		lba[3] = (sector >> 24) & 0xFF;
		head   = 0;
	}
	#else
	if (0) ata_sleep400(drive);
	#endif
	else {
		/* Use LBA24 */
		lba48 = false;
		lba[5] = 0;
		lba[4] = 0;
		lba[3] = 0;
		head   = (sector >> 24) & 0xF;
	}

	lba[2] = (sector >> 16) & 0xFF;
	lba[1] = (sector >> 8)  & 0xFF;
	lba[0] = (sector >> 0)  & 0xFF;
	

	/* wait for drive to be ready */
	while (inb(ata_base[drive] + REG_STAT) & STAT_BUSY);

	/* select drive, send LBA, and send read command */
	outb(ata_base[drive] + REG_SELECT, (SEL(drive) ? 0xF0 : 0xE0) | head);
	outb(ata_base[drive] + REG_COUNT0, 0x01);
	outb(ata_base[drive] + REG_LBA0, lba[0]);
	outb(ata_base[drive] + REG_LBA1, lba[1]);
	outb(ata_base[drive] + REG_LBA2, lba[2]);
	if (lba48) {
		outb(ata_base[drive] + REG_LBA3, lba[3]);
		outb(ata_base[drive] + REG_LBA3, lba[4]);
		outb(ata_base[drive] + REG_LBA3, lba[5]);
	}

	return lba48;
}
