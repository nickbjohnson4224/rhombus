#include <driver.h>
#include <kernel.h>
#include <stdint.h>
#include "floppy.h"
#include "console.h"

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

static volatile uint8_t got_irq6;

void init_floppy() {
	uint16_t base = 0x3F0;
	floppy_parameters_t floppy;

	/* Fetch floppy data */
	pull_call(0, FLOPPY_PARAMETER_TABLE, (uint32_t) &floppy, sizeof(floppy_parameters_t));

	got_irq6 = 0;

	/* Send software reset */
	outb(base + D_OUTPUT, 0x00);
	outb(base + D_OUTPUT, 0x0C);

	while (!got_irq6);

	update_progress("found floppy drive...");
	swrite((char*) &floppy);

	return;
}

void floppy_handler(void) {
	got_irq6 = 1;
}
