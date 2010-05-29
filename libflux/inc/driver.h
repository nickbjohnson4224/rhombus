#ifndef DRIVER_H
#define DRIVER_H

/*** Port Access ***/
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/*** Device Descriptor ***/

typedef struct device {
	uint8_t type;
	uint8_t bus;
	uint8_t slot;
	uint8_t sub;
} device_t;

#define DEV_TYPE_NATIVE	0
#define DEV_TYPE_PCI	1
#define DEV_TYPE_FREE	2

/*** IRQ Redirection ***/

void rirq(uint8_t irq);

#endif
