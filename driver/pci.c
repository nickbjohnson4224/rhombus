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

uint32_t pci_dev_triple(uint32_t dev) {
	uint32_t triple = 0;

	triple |= (dev >> 16) & 0xFF;
	triple <<= 8;
	triple |= (dev >> 11) & 0x1F;
	triple <<= 8;
	triple |= (dev >> 8)  & 0x07;

	return triple;
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
	uint32_t bus, slot, func, dev, end;

	if (!start) {
		bus = 0;
		slot = 0;
		func = 0;
	}
	else {
		bus  = (start >> 16) & 0xFF;
		slot = (start >> 11) & 0x1F;
		func = ((start >> 8) & 0x07) + 1;
	}

	while (pci_check_dev(pci_address_dev(bus, slot, func))) {
	 while (pci_check_dev(pci_address_dev(bus, slot, func))) {
	  while (pci_check_dev(pci_address_dev(bus, slot, func))) {

		if (pci_config_inb(pci_address_dev(bus, slot, func), PCI_CLASS) == class) {
			return pci_address_dev(bus, slot, func);
		}

		func++;		
	  }
	  func = 0;
	  slot++;
	 }
	 slot = 0;
	 bus++;
	}

	return 0;
}

uint32_t pci_find_subclass(uint8_t class, uint8_t subclass, uint32_t start) {
	uint32_t bus, slot, func, dev, end;

	if (!start) {
		bus = 0;
		slot = 0;
		func = 0;
	}
	else {
		bus  = (start >> 16) & 0xFF;
		slot = (start >> 11) & 0x1F;
		func = ((start >> 8) & 0x07) + 1;
	}

	while (pci_check_dev(pci_address_dev(bus, slot, func))) {
	 while (pci_check_dev(pci_address_dev(bus, slot, func))) {
	  while (pci_check_dev(pci_address_dev(bus, slot, func))) {

		dev = pci_address_dev(bus, slot, func);
		if (pci_config_inw(dev, PCI_SUBCLASS) == 
				((uint32_t) class << 8) | (uint32_t) subclass) {
			return dev;
		}

		func++;		
	  }
	  func = 0;
	  slot++;
	 }
	 slot = 0;
	 bus++;
	}

	return 0;
}
