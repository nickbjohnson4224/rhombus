#ifndef KHSIGNAL_H
#define KHSIGNAL_H

#include <stdint.h>

/* Signal handler pointer */
typedef void (*khsig_handler_t) (uint32_t, uint32_t[4]);

/* Set up Khaos signal base handler */
void khsig_init(void);

/* Block all Khaos signals */
void khsig_block(void);

/* Unblock all Khaos signals */
void khsig_unblock(void);

/* Send a Khaos signal */
int khsig_send(uint32_t target, uint8_t signal, uint32_t args[4]);

/* Send an asynchronous Khaos signal */
int khsig_asend(uint32_t target, uint8_t signal, uint32_t args[4]);

/* Register a Khaos signal handler */
void khsig_register(uint8_t signal, khsig_handler_t handler);

/* Wait for a Khaos signal */
void khsig_wait(uint32_t signal);

#endif
