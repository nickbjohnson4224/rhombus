/* Copyright 2010 Nick Johnson */

#ifndef PCI_H
#define PCI_H

#include <flux/arch.h>
#include <flux/driver.h>

/* Configuration Space Layout */

#define PCI_VENDOR  	0x00
#define PCI_DEVICE  	0x02
#define PCI_COMMAND 	0x04
#define PCI_STATUS		0x06
#define PCI_REVISION	0x08
#define PCI_PROG_IF		0x09
#define PCI_CLASSCODE	0x0A
#define PCI_SUBCLASS	0x0A
#define PCI_CLASS		0x0B

#define PCI_HTYPE		0x0E

#define PCI_BAR(n) 		(0x10 + ((n) << 2))
#define PCI_BAR0		0x10
#define PCI_BAR1		0x14
#define PCI_BAR2		0x18
#define PCI_BAR3		0x1C
#define PCI_BAR4		0x20
#define PCI_BAR5		0x24

#define PCI_INTLINE		0x3C
#define PCI_INTPIN		0x3D

/* Low Level Manipulation Functions */

uint32_t pci_address(uint8_t bus, uint8_t slot, uint8_t func);

device_t pci_to_dev(uint32_t pci);
uint32_t dev_to_pci(device_t dev);

uint32_t pci_config_ind(device_t dev, uint8_t off);
uint16_t pci_config_inw(device_t dev, uint8_t off);
uint8_t  pci_config_inb(device_t dev, uint8_t off);

void pci_config_outd(device_t dev, uint8_t off, uint32_t val);
void pci_config_outw(device_t dev, uint8_t off, uint16_t val);
void pci_config_outb(device_t dev, uint8_t off, uint16_t val);

uint32_t pci_config_barbase(device_t dev, uint8_t index);
uint32_t pci_config_bartype(device_t dev, uint8_t index);

/* High Level Search Functions */

int      pci_check(device_t dev);

device_t pci_findb(uint8_t  val, uint8_t off, device_t start);
device_t pci_findw(uint16_t val, uint8_t off, device_t start);
device_t pci_findd(uint32_t val, uint8_t off, device_t start);

#define CLASSCODE(class, subclass) (((class) << 8) | (subclass))

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
