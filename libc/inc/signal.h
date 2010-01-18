#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>

#define MAXSIGNAL 256

/* Signal handler pointer */
typedef void (*signal_handler_t) (uint32_t caller, void *grant);

/* Set up signal base handler */
void siginit(void);

/* Block all signals */
void sigblock(void);

/* Unblock all signals */
void sigunblock(void);

/* Send a signal */
int fire(uint32_t target, uint16_t signal, void *grant);

/* Send a tail signal */
void tail(uint32_t target, uint16_t signal, void *grant);

/* Register a signal handler */
void sigregister(uint16_t signal, signal_handler_t handler);

/* Reset the wait counter */
void wreset(uint16_t signal);

/* Wait for a signal */
void wait(uint16_t signal);

/* Reserve IRQ */
void rirq(uint8_t irq);

#endif
