#include <khaos/driver.h>
#include <khaos/kernel.h>
#include <khaos/signal.h>
#include <stdint.h>
#include <string.h>
#include <driver/floppy.h>
#include <driver/console.h>

struct driver_interface floppy = {
	init_floppy,
	NULL,
	NULL,
	NULL,
	NULL,
	6,
	NULL,
};

enum floppy_commands {
	CMD_SPECIFY = 3,
	CMD_WRITE_DATA = 5,
	CMD_READ_DATA = 6,
	CMD_RECALIBRATE = 7,
	CMD_SENSE_INTERRUPT = 8,
	CMD_SEEK = 15
};

#define STATUS_A	0x0000
#define STATUS_B	0x0001
#define D_OUTPUT	0x0002
#define STATUS_M	0x0004
#define DATA_RATE	0x0004
#define DATA		0x0005
#define D_INPUT		0x0007
#define CONTROL		0x0007
#define RESULT		0x0000

#define FLOPPY_PARAMETER_TABLE 0xFEFC7

typedef struct {
	uint8_t steprate_headunload;
	uint8_t headload_ndma;
	uint8_t motor_delay_off;
	uint8_t bytes_per_sector;
	uint8_t sectors_per_track;
	uint8_t gap_length;
	uint8_t data_length;
	uint8_t format_gap_length;
	uint8_t unused;
	uint8_t head_settle_time;
	uint8_t motor_start_time;
} floppy_parameters_t;

static volatile uint32_t floppy_size;
static volatile uint16_t base;
static volatile floppy_parameters_t floppy_params;

static const char * drive_types[8] = {
	"none",
	"360 KB 5.25 inch",
	"1.2 MB 5.25 inch",
	"720 kB 3.5 inch",
	"1.44 MB 3.5 inch",
	"2.88 MB 3.5 inch",
	"unknown type",
	"unknown type"
};

static const uint32_t drive_sizes[8] = {
	0x000000,
	0x05A000,
	0x12C000,
	0x0B4000,
	0x168000,
	0x2D0000,
	0x000000,
	0x000000
};

int init_floppy(uint16_t selector) {
	uint8_t fdtype;
	
	base = (selector) ? 0x370 : 0x3F0;

	/* Fetch floppy data from EBDA */
	pull_call(0, FLOPPY_PARAMETER_TABLE, (uint32_t) &floppy_params, sizeof(floppy_parameters_t));

	outb(0x70, 0x10);
	fdtype = inb(0x71);
	fdtype = (selector) ? fdtype & 0xF : fdtype >> 4;

	if (fdtype != 0) {
		update_progress("found floppy drive: ");
		swrite(drive_types[fdtype]);
	}
	else {
		update_progress("no floppy drive found!");
	}

	floppy_size = drive_sizes[fdtype >> 4];

	/* Send software reset */
	outb(base + D_OUTPUT, 0x00);
	outb(base + D_OUTPUT, 0x0C);

	khsignal_wait(3);

	return 0;
}

void floppy_write_cmd(uint8_t cmd) {
	size_t i;
	for (i = 0; i < 600; i++) {
		if (0x80 & inb(0x3F0 + STATUS_M)) {
			outb(0x03F0 + DATA, cmd);
			return;
		}
	}
}

uint8_t floppy_read_data() {
	size_t i;
	for (i = 0; i < 600; i++) {
		if (0x80 & inb(0x03F0 + STATUS_M)) {
			return inb(0x03F0 + DATA);
		}
	}
	return 0;
}
