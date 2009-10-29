#ifndef PCI_H
#define PCI_H

uint16_t pci_scan_class(uint16_t bus, uint8_t class, uint8_t subclass);
uint32_t pci_read_dword(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off);
uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off);
uint8_t  pci_read_byte(uint16_t bus, uint16_t slot, uint16_t func, uint16_t off);

#endif
