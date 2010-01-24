/* Copyright 2010 Nick Johnson */

#include <driver.h>
#include <flux.h>

uint16_t dev_getdevice(device_t dev) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
	case 1:
	default: return 0;
	}
}

uint16_t dev_getiobase(device_t dev, int bar) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return (bar) ? 0x1F0 : 0x3F6;
		case 1: return (bar) ? 0x170 : 0x376;
		case 2: return (bar) ? 0x1E8 : 0x3E6;
		case 3: return (bar) ? 0x168 : 0x366;
		case 4: return 0x60;
		}
	case 1:
	default: return 0;
	}
}

uint16_t dev_getiolimit(device_t dev, int bar) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return (bar) ? 0x1F7 : 0x3F6;
		case 1: return (bar) ? 0x177 : 0x376;
		case 2: return (bar) ? 0x1E7 : 0x3E6;
		case 3: return (bar) ? 0x167 : 0x366;
		case 4: return 0x60;
		}
	case 1:
	default: return 0;
	}
}

uint16_t dev_getirqnum(device_t dev) {
	uint8_t type = (dev >> 24) & 0x7F;

	switch (type) {
	case 0:
		switch ((dev >> 8) & 0xFF) {
		case 0: return 14;
		case 1: return 15;
		case 4: return 1;
		case 6: return 6;
		}
	case 1:
	default: return 0;
	}
}

void rirq(uint8_t irq) {
	_ctrl(CTRL_IRQRD | CTRL_IRQ(irq), CTRL_IRQRD | CTRL_IRQMASK);
}
