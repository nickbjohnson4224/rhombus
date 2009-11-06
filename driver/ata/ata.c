#include <khaos/driver.h>
#include <khaos/kernel.h>
#include <khaos/signal.h>
#include <stdint.h>
#include <string.h>
#include <driver/pci.h>

static int init_ata(uint16_t selector);
/*static int read_ata(uintmax_t seek, size_t size, void *data);
static int write_ata(uintmax_t seek, size_t size, void *data);*/

struct driver_interface ata = {
	init_ata,
	NULL,
	NULL,
	NULL,
	NULL,
	2,
	NULL,
};

static int init_ata(uint16_t selector) {
	if (pci_scan_class(0, 0x0, 0x0) != 0xFFFF) {
		return 0;
	}
	else {
		return 1;
	}
}

/*static int read_ata(uintmax_t seek, size_t size, void *data);

static int write_ata(uintmax_t seek, size_t size, void *data); */
