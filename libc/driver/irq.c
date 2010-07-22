/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <abi.h>

void rirq(uint8_t irq) {
	_pctl(CTRL_IRQRD | CTRL_IRQ(irq), CTRL_IRQRD | CTRL_IRQMASK);
}
