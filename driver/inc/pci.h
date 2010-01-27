/* Copyright 2010 Nick Johnson */

#ifndef PCI_H
#define PCI_H

/* Configuration Space Layout */

#define PCI_VENDOR  	0x00
#define PCI_DEVICE  	0x02
#define PCI_COMMAND 	0x04
#define PCI_STATUS		0x06
#define PCI_REVISION	0x08
#define PCI_PROG_IF		0x09
#define PCI_SUBCLASS	0x0A
#define PCI_CLASS		0x0B

/* Low Level Manipulation Functions */

uint32_t pci_address_dev(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_dev_triple(uint32_t dev);

#define PCI_BUS(t)  ((t >> 16) & 0xFF)
#define PCI_SLOT(t) ((t >> 11) & 0x1F)
#define PCI_FUNC(t) ((t >> 8)  & 0x07)

uint32_t pci_config_ind(uint32_t dev, uint8_t off);
uint16_t pci_config_inw(uint32_t dev, uint8_t off);
uint8_t  pci_config_inb(uint32_t dev, uint8_t off);

void pci_config_outd(uint32_t dev, uint8_t off, uint32_t val);
void pci_config_outw(uint32_t dev, uint8_t off, uint16_t val);
void pci_config_outb(uint32_t dev, uint8_t off, uint16_t val);

/* High Level Search Functions */

int      pci_check_dev(uint32_t dev);
uint32_t pci_find_class(uint8_t class, uint32_t start);
uint32_t pci_find_subclass(uint8_t class, uint8_t subclass, uint32_t start);

#define CLASS_UNKNOWN		0x00
#define CLASS_STORAGE		0x01
#define CLASS_NETWORK		0x02
#define CLASS_DISPLAY		0x03
#define CLASS_MULTIMEDIA	0x04
#define CLASS_MEMORY		0x05
#define CLASS_BRIDGE		0x06
#define CLASS_SIMPLE		0x07
#define CLASS_PERHIPHERAL	0x08
#define CLASS_INPUT			0x09
#define CLASS_STATION		0x0A
#define CLASS_CPU			0x0B
#define CLASS_SERIAL		0x0C
#define CLASS_MISC			0xFF

#define SUBCLASS_OTHER		0x80

#define SUBCLASS_OLDVGA		0x01

#define SUBCLASS_SCSI		0x00
#define SUBCLASS_IDE		0x01
#define SUBCLASS_FLOPPY		0x02
#define SUBCLASS_IPI		0x03
#define SUBCLASS_RAID		0x04

#define SUBCLASS_ETHERNET	0x00
#define SUBCLASS_TOKENRING	0x01
#define SUBCLASS_FDDI		0x02
#define SUBCLASS_ATM		0x03

#define SUBCLASS_VGA		0x00
#define SUBCLASS_XGA		0x01

#define SUBCLASS_VIDEO		0x00
#define SUBCLASS_AUDIO		0x01

#define SUBCLASS_RAM		0x00
#define SUBCLASS_FLASH		0x01

#define SUBCLASS_HOST_PCI	0x00
#define SUBCLASS_PCI_ISA	0x01
#define SUBCLASS_PCI_EISA	0x02
#define SUBCLASS_PCI_MICRO	0x03
#define SUBCLASS_PCI_PCI	0x04
#define SUBCLASS_PCI_PCIMCIA 0x05
#define SUBCLASS_PCI_NUBUS	0x06
#define SUBCLASS_PCI_CARD	0x07

#define SUBCLASS_XT			0x00
#define SUBCLASS_PARALELL	0x01

#define SUBCLASS_PIC		0x00
#define SUBCLASS_DMA		0x01
#define SUBCLASS_TIMER		0x02
#define SUBCLASS_RTC		0x03

#define SUBCLASS_KEYBOARD	0x00
#define SUBCLASS_DIGITIZER	0x01
#define SUBCLASS_MOUSE		0x02

#define SUBCLASS_GENERIC	0x00

#define SUBCLASS_386		0x00
#define SUBCLASS_486		0x01
#define SUBCLASS_PENTIUM	0x02
#define SUBCLASS_ALPHA		0x10
#define SUBCLASS_POWERPC	0x20
#define SUBCLASS_COPROC		0x40

#define SUBCLASS_FIREWIRE	0x00
#define SUBCLASS_ACCESS		0x01
#define SUBCLASS_SSA		0x02
#define SUBCLASS_USB		0x03

#endif
