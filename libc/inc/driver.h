#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <config.h>
#include <signal.h>

/***** PORT ACCESS *****/
uint8_t  inb(uint16_t port);
uint16_t inw(uint16_t port);
uint32_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/***** DEVICE DESCRIPTOR *****/

typedef struct device {
	uint8_t type;
	uint8_t bus;
	uint8_t slot;
	uint8_t sub;
} device_t;

#define DEV_TYPE_NATIVE	0
#define DEV_TYPE_PCI	1
#define DEV_TYPE_FREE	2

/***** DRIVER INTERFACE STRUCTURE *****/ 

typedef void (*callback_t)(struct request *r);
struct driver_interface {
	void (*init) (device_t dev);	/* Initialize driver on device */
	void (*halt) (void);			/* De-initialize device */
	void (*work) (void);			/* Do background work */
	size_t jobs;					/* Number of background jobs to complete */
};

/***** IRQ REDIRECTION *****/

void rirq(uint8_t irq);

#endif
