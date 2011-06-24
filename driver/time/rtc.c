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

#include <stdint.h>
#include <driver.h>

#include <rdi/arch.h>

#include "time.h"

static int from_bcd(uint8_t datum) {
	return ((datum & 0x0F) + ((datum & 0xF0) >> 4) * 10);
}

static int month_day_starts[] = {
	0,
	31,
	59,
	90,
	120,
	151,
	181,
	212,
	243,
	273,
	304,
	334
};

int get_time(void) {
	int time = 0;
	uint8_t datum;
	int month, year;

	// wait for RTC to be ready
	do {
		outb(0x70, 0x0A);
		if (inb(0x71) & 0x400) {
			continue;
		}
	} while(0);

	// get seconds
	outb(0x70, 0x00);
	datum = inb(0x71);
	time += from_bcd(datum);

	// get minutes
	outb(0x70, 0x02);
	datum = inb(0x71);
	time += from_bcd(datum) * 60;

	// get hours
	outb(0x70, 0x04);
	datum = inb(0x71);
	time += from_bcd(datum) * 3600;

	// check for 12 hour time
	if (datum & 0x80) {
		time += 43200;
	}

	// get days
	outb(0x70, 0x07);
	datum = inb(0x71);
	time += from_bcd(datum) * 86400;

	// get month
	outb(0x70, 0x08);
	datum = inb(0x71);
	month = from_bcd(datum);
	time += month_day_starts[month - 1] * 86400;

	// get year
	outb(0x70, 0x09);
	datum = inb(0x71);
	year  = from_bcd(datum);
	time += year * 31536000;

	// add leap years
	time += (year / 4) * 86400;

	// add offset to Jan 1, 2010
	time += 946684800;

	return time;
}
