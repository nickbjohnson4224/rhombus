/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <interrupt.h>
#include <timer.h>
#include <ports.h>
#include <irq.h>

/*****************************************************************************
 * tick
 *
 * The number of times that the timer interrupt has been fired (obstensibly:
 * it can be reset). This is approximately how many times the period last
 * given to timer_set_freq() has passed, but may be slightly inaccurate
 * depending on hardware limitations. For example, the PC's PIT is accurate
 * to 2 seconds per day.
 */

static uint64_t tick = 0;

/*****************************************************************************
 * timer_set_tick
 *
 * Sets the value of the timer tick to <value>.
 */

void timer_set_tick(uint64_t value) {
	tick = value;
}

/*****************************************************************************
 * timer_get_tick
 *
 * Returns the value of the timer tick.
 */

uint64_t timer_get_tick(void) {
	return tick;
}

/*****************************************************************************
 * timer_handler (interrupt handler)
 *
 * Increments the timer tick and schedules the next thread.
 */

static struct thread *timer_handler(struct thread *image) {

	tick++;
	image->tick++;
	image->proc->tick++;

	return schedule_next();
}

/******************************************************************************
 * timer_set_freq
 *
 * Sets the timer to fire with a frequency of <hertz> hertz (approximately). If
 * the timer has not been initialized, it is done now. Returns zero on success,
 * nonzero on failure (i.e. if the given frequency is impossible to obtain).
 */

int timer_set_freq(uint32_t hertz) {
	uint32_t divider;

	divider = 1193180 / hertz;

	if ((divider == 0) || (divider >= 65536)) {
		return 1;
	}

	outb(0x43, 0x36);
	outb(0x40, (uint8_t) (divider & 0xFF));
	outb(0x40, (uint8_t) (divider >> 0x8));

	irq_allow(0);
	int_set_handler(IRQ2INT(0), timer_handler);

	return 0;
}
