#ifndef KHSIGNAL_H
#define KHSIGNAL_H

#include <stdint.h>

/* Set up Khaos signal base handler */
void khsignal_init(void);

/* Block all Khaos signals */
void khsignal_block(void);

/* Unblock all Khaos signals */
void khsignal_unblock(void);

/* Send a Khaos signal */
int khsignal_send(uint32_t target, uint8_t signal, uint32_t args[4]);

/* Send an asynchronous Khaos signal */
int khsignal_asend(uint32_t target, uint8_t signal, uint32_t args[4]);

/* Register a Khaos signal handler */
void khsignal_register(uint8_t signal, void (*handler) (uint32_t source, uint32_t args[4]));

/* Wait for a Khaos signal */
void khsignal_wait(uint32_t signal);

#endif
