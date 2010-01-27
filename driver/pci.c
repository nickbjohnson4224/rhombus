#include <driver.h>
#include <stdint.h>
#include <stdio.h>

#include <driver/pci.h>

uint32_t pci_address_dev(uint8_t bus, uint8_t slot, uint8_t func) {
	uint32_t addr = 0x80000000;

	addr |= ((uint32_t) bus) << 16;
	addr |= ((uint32_t) slot) << 11;
	addr |= ((uint32_t) func) << 8;

	return addr;
}

uint32_t pci_config_ind(uint32_t dev, uint8_t off) {
	
	dev |= off & 0xFC;
	outd(0xCF8, dev);

	return ind(0xCFC);
}

uint16_t pci_config_inw(uint32_t dev, uint8_t off) {

	dev |= off & 0xFC;
	outd(0xCF8, dev);

	return ((ind(0xCFC) >> ((off & 0x2) * 8)) & 0xFFFF);
}

uint8_t pci_config_inb(uint32_t dev, uint8_t off) {
	
	dev |= off & 0xFC;
	outd(0xCF8, dev);

	return ((ind(0xCFC) >> ((off & 0x3) * 8)) & 0xFF);
}

int pci_check_dev(uint32_t dev) {
	return (pci_config_ind(dev, 0) == 0xFFFFFFFF) ? 0 : 1;
}

uint32_t pci_find_class(uint8_t class, uint32_t start) {
	uint32_t dev, end;

	if (!start) {
		dev = pci_address_dev(0, 0, 0);
	}
	else {
		dev = start;
	}

	end = 0;

	while (pci_check_dev(dev)) {

		while (pci_check_dev(dev)) {

			while (pci_check_dev(dev)) {

				printf("checking %x\n", dev);

				if (pci_config_inw(dev, PCI_CLASS) == class) {
					return dev;
				}

				dev += 0x100;
			}
			dev &= ~0x7FF;
			dev += 0x800;
		}
		dev &= ~0xFFFF;
		dev += 0x10000;
	}

	return 0;
}
