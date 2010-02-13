#ifndef ATA_H
#define ATA_H

extern struct driver_interface ata;

/* configuration flags */
#define ATACONF_ATAPI		/* Support ATAPI commands */
/*#define ATACONF_DMA  */	/* Use DMA if possible */
#define ATACONF_LONG		/* Enable 48 bit LBAs */
#define ATACONF_IRQ			/* Use IRQs instead of polling */
/*#define ATACONF_SATA */	/* Support SATA commands */

/* ATAPI needs IRQs */
#ifdef  ATACONF_ATAPI
#ifndef ATACONF_IRQ
#define ATACONF_IRQ
#endif
#endif

/* controllers */
#define ATA0				0x00
#define ATA1				0x02

/* drives */
#define ATA00				0x00
#define ATA01				0x01
#define ATA10				0x02
#define ATA11				0x03

/* drive selector */
#define SEL(d) ((d) & 1)
#define MASTER				0
#define SLAVE				1

/* ATA base registers */
#define REG_DATA			0x00
#define REG_ERR				0x01
#define REG_FEATURE			0x01
#define REG_COUNT0			0x02
#define REG_LBA0			0x03
#define REG_LBA1			0x04
#define REG_LBA2			0x05
#define REG_SELECT			0x06
#define REG_CMD				0x07
#define REG_STAT			0x07
#define REG_COUNT1			0x08
#define REG_LBA3			0x09
#define REG_LBA4			0x0A
#define REG_LBA5			0x0B

/* ATA control register */
#define REG_CTRL			0x02
#define REG_ASTAT			0x02

/* ATA DMA registers */
#define REG_DMA_CMD			0x00
#define REG_DMA_STAT		0x02
#define REG_DMA_ADDR		0x04

/* ATA/ATAPI commands */
#define CMD_READ_PIO		0x20
#define CMD_READ_PIO48		0x24
#define CMD_READ_DMA		0xC8
#define CMD_READ_DMA48		0x25
#define CMD_READ_ATAPI		0xA8
#define CMD_WRITE_PIO		0x30
#define CMD_WRITE_PIO48		0x34
#define CMD_WRITE_DMA		0xCA
#define CMD_WRITE_DMA48		0x35
#define CMD_CACHE_FLUSH		0xE7
#define CMD_CACHE_FLUSH48	0xEA
#define CMD_ID				0xEC
#define CMD_ID_ATAPI		0xA1
#define CMD_EJECT_ATAPI		0x1B
#define CMD_ATAPI			0xA0

/* ATA status bits */
#define STAT_ERROR			0x01
#define STAT_DRQ			0x08
#define STAT_SERVICE		0x10
#define STAT_FAULT			0x20
#define STAT_READY			0x40
#define STAT_BUSY			0x80

/* ATA control bits */
#define CTRL_NEIN			0x02
#define CTRL_RESET			0x04
#define CTRL_HBYTE			0x80

/* ATA IDENTIFY offsets */
#define ID_TYPE				0x00
#define ID_SERIAL			0x0A
#define ID_MODEL			0x1B
#define ID_CAP				0x31
#define ID_VALID			0x35
#define ID_MAX_LBA			0x3C
#define ID_CMDSET			0x52
#define ID_MAX_LBA48		0x64

/* ATA DMA commands */
#define DMA_CMD_RUN			0x01
#define DMA_CMD_RW			0x08
#define DMA_STAT_READY		0x01
#define DMA_STAT_ERROR		0x02
#define DMA_STAT_IRQ		0x04
#define DMA_STAT_MDMA		0x20
#define DMA_STAT_SDMA		0x40

/* drive structure flags */
#define FLAG_EXIST			0x01
#define FLAG_DMA			0x02
#define FLAG_ATAPI			0x04
#define FLAG_LONG			0x08
#define FLAG_SATA			0x10
#define FLAG_LOCK			0x20

/*** Helper Routines ***/

void ata_sleep400(uint8_t drive);
void ata_select(uint8_t drive);

/* ATA controller managed by this driver */
extern device_t ata_controller;

/* drive information structure */
extern struct ata_drive {
	uint8_t  flags;			 /* see FLAG_* */
	uint16_t signature;
	uint16_t capabilities;
	uint16_t commandsets;
	uint64_t size;			/* size in sectors of the entire drive */
	char     model[41];		/* model name as reported by IDENTIFY */
	uint16_t sectsize;
} ata_drive[4];

/* I/O port BARs and IRQs */
extern uint16_t ata_base[4];
extern uint16_t ata_ctrl[4];
extern uint16_t ata_dma [4];
extern uint8_t  ata_irq [4];

/*** Sector Addressing ***/

bool ata_send_lba(uint8_t drive, uint64_t sector);

/*** Port I/O ***/

void pio_read_sector (uint8_t drive, uint64_t sector, uint16_t *buffer);
void pio_write_sector(uint8_t drive, uint64_t sector, uint16_t *buffer);

void pio_add_read (uint8_t drive, uint64_t sector, uint16_t *buffer);
void pio_add_write(uint8_t drive, uint64_t sector, uint16_t *buffer);
void pio_run_read (uint8_t drive);
void pio_run_write(uint8_t drive);
void pio_run_all  (uint8_t drive);

/*** DMA ***/

void dma_read_sector (uint8_t drive, uint64_t sector, uint16_t *buffer);
void dma_write_sector(uint8_t drive, uint64_t sector, uint64_t *buffer);

void dma_add_read (uint8_t drive, uint64_t sector, uint16_t *buffer);
void dma_add_write(uint8_t drive, uint64_t sector, uint16_t *buffer);
void dma_run_read (uint8_t drive);
void dma_run_write(uint8_t drive);
void dma_run_all  (uint8_t drive);

#endif
