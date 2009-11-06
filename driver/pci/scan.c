#include <khaos/driver.h>
#include <khaos/kernel.h>
#include <stdint.h>
#include <driver/pci.h>
#include <driver/console.h>

uint16_t pci_scan_class(uint16_t bus, uint8_t class, uint8_t subclass) {
	uint16_t i;

	for (i = 0; i < 0xFFFF; i++) {
		if (pci_read_word(bus, i, 0, 0x0) == 0xFFFF) break;
		if (pci_read_word(bus, i, 0, 0xA) == class || (subclass << 8)) {
			return i;
		}
	}

	return 0xFFFF;
}

uint32_t pci_read_dword(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off) {
	uint32_t address;

	address = (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000;
	outd(0xCF8, address);
	return ind(0xCFC);
}

uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off) {
	return ((pci_read_dword(bus, slot, func, off) >> ((off & 0x2) << 3)) & 0xFFFF);
}

uint8_t pci_read_byte(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off) {
	return ((pci_read_dword(bus, slot, func, off) >> ((off & 0x3) << 3)) & 0xFF);
}
