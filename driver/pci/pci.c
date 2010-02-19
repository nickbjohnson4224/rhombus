/* Copyright 2010 Nick Johnson */

#include <flux/arch.h>
#include <flux/driver.h>
#include <driver/pci.h>

uint32_t pci_address(uint8_t bus, uint8_t slot, uint8_t func) {
	uint32_t addr = 0x80000000;

	addr |= ((uint32_t) bus) << 16;
	addr |= ((uint32_t) slot) << 11;
	addr |= ((uint32_t) func) << 8;

	return addr;
}

device_t pci_to_dev(uint32_t pci) {
	device_t dev;

	dev.type = DEV_TYPE_PCI;
	dev.bus  = (pci >> 16) & 0xFF;
	dev.slot = (pci >> 11) & 0x1F;
	dev.sub  = (pci >> 8)  & 0x07;

	return dev;
}

uint32_t dev_to_pci(device_t dev) {
	uint32_t pci = 0x80000000;

	pci |= (uint32_t) dev.bus  << 16;
	pci |= (uint32_t) dev.slot << 11;
	pci |= (uint32_t) dev.sub  << 8;

	return pci;
}

uint32_t pci_config_ind(device_t dev, uint8_t off) {
	uint32_t pci = dev_to_pci(dev);

	pci |= off & 0xFC;
	outd(0xCF8, pci);

	return ind(0xCFC);
}

uint16_t pci_config_inw(device_t dev, uint8_t off) {
	uint32_t pci = dev_to_pci(dev);

	pci |= off & 0xFC;
	outd(0xCF8, pci);

	return ((ind(0xCFC) >> ((off & 0x2) * 8)) & 0xFFFF);
}

uint8_t pci_config_inb(device_t dev, uint8_t off) {
	uint32_t pci = dev_to_pci(dev);
	
	pci |= off & 0xFC;
	outd(0xCF8, pci);

	return ((ind(0xCFC) >> ((off & 0x3) * 8)) & 0xFF);
}

int pci_check(device_t dev) {
	if (dev.type != DEV_TYPE_PCI) return 0;
	return (pci_config_inw(dev, 0) == 0xFFFF) ? 0 : 1;
}

device_t pci_findb(uint8_t val, uint8_t off, device_t start) {
	device_t dev;

	if (start.type != DEV_TYPE_PCI) {
		dev.type = DEV_TYPE_PCI;
		dev.bus  = 0;
		dev.slot = 0;
		dev.sub  = 0;
	}
	else {
		dev = start;
		dev.sub++;
	}

	while (dev.bus < 8) {
		while (dev.slot < 0x1F) {
			while (pci_check(dev)) {

				if (pci_config_inb(dev, off) == val) {
					return dev;
				}

				dev.sub++;
			}
			dev.sub = 0;
			dev.slot++;
		}
		dev.slot = 0;
		dev.bus++;
	}

	dev.type = -1;
	return dev;
}

device_t pci_findw(uint16_t val, uint8_t off, device_t start) {
	device_t dev;

	if (start.type != DEV_TYPE_PCI) {
		dev.type = DEV_TYPE_PCI;
		dev.bus  = 0;
		dev.slot = 0;
		dev.sub  = 0;
	}
	else {
		dev = start;
		dev.sub++;
	}

	while (pci_check(dev)) {
		while (pci_check(dev)) {
			while (pci_check(dev)) {

				if (pci_config_inw(dev, off) == val) {
					return dev;
				}

				dev.sub++;
			}
			dev.sub = 0;
			dev.slot++;
		}
		dev.slot = 0;
		dev.bus++;
	}

	dev.type = -1;
	return dev;
}

device_t pci_findd(uint32_t val, uint8_t off, device_t start) {
	device_t dev;

	if (start.type != DEV_TYPE_PCI) {
		dev.type = DEV_TYPE_PCI;
		dev.bus  = 0;
		dev.slot = 0;
		dev.sub  = 0;
	}
	else {
		dev = start;
		dev.sub++;
	}

	while (pci_check(dev)) {
		while (pci_check(dev)) {
			while (pci_check(dev)) {

				if (pci_config_ind(dev, off) == val) {
					return dev;
				}

				dev.sub++;
			}
			dev.sub = 0;
			dev.slot++;
		}
		dev.slot = 0;
		dev.bus++;
	}

	dev.type = -1;
	return dev;
}

uint32_t pci_config_barbase(device_t dev, uint8_t index) {
	uint32_t bar = pci_config_ind(dev, PCI_BAR(index));

	if (bar & 0x1) {
		return (bar & ~0x3);
	}
	else {
		return (bar & ~0xF);
	}
}

uint32_t pci_config_bartype(device_t dev, uint8_t index) {
	uint32_t bar = pci_config_ind(dev, PCI_BAR(index));

	if (bar & 0x1) {
		return (bar & 0x1);
	}
	else {
		return (bar & 0x7);
	}
}
