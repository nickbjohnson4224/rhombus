/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <driver.h>

#include "vga.h"

uint8_t get_color_true(uint32_t c) {
	uint8_t r, g, b;

	/* get color channels */
	b = (c >> 0)  & 0xFF;
	g = (c >> 8)  & 0xFF;
	r = (c >> 16) & 0xFF;

	/* reduce color channels to 3-3-2 RGB */
	r = (r >> 5);
	g = (g >> 5);
	b = (b >> 6);

	/* compose into color byte */
	return ((r << 0) | (g << 3) | (b << 6));
}

uint8_t get_color_rgbi(uint32_t c) {
	uint8_t r, g, b, ret;

	/* get color channels */
	b = (c >> 0)  & 0xFF;
	g = (c >> 8)  & 0xFF;
	r = (c >> 16) & 0xFF;

	/* get hue */
	ret = 0;
	if (r > 64) ret |= 1;
	if (g > 64) ret |= 2;
	if (b > 64) ret |= 4;

	/* get value */
	if (r > 160 || b > 160 || g > 160) ret |= 8;

	return ret;
}

uint8_t get_color_mono(uint32_t c) {
	uint8_t r, g, b, ret;

	/* get color channels */
	r = (c >> 0)  & 0xFF;
	g = (c >> 8)  & 0xFF;
	b = (c >> 16) & 0xFF;	
	
	/* get value */
	ret = 0;
	if (r > 128 || b > 128 || g > 128) ret = 1;

	return ret;
}

uint8_t get_color_gray(uint32_t c) {
	uint32_t v;

	v =   0;
	v +=  ((c >> 0)  & 0xFF);
	v +=  ((c >> 8)  & 0xFF);
	v +=  ((c >> 16) & 0xFF);
	v /=  3;
	v >>= 2;

	if (v > 0x3F) v = 0x3F;

	return (v & 0x3F);
}

void set_color_true(void) {
	uint32_t i;
	uint8_t r, g, b;
	
	for (i = 0; i < 256; i++) {
		outb(DAC_WRITE_INDEX, i);

		r = ((i >> 0) & 0x7) << 3;
		g = ((i >> 3) & 0x7) << 3;
		b = ((i >> 6) & 0x3) << 4;

		outb(DAC_DATA, r);
		outb(DAC_DATA, g);
		outb(DAC_DATA, b);
	}
}

void set_color_rgbi(void) {
	uint8_t i;
	uint8_t r, g, b;
	
	for (i = 0; i < 16; i++) {
		outb(DAC_WRITE_INDEX, i);

		r = (i & 1) ? 0x1F : 0;
		g = (i & 2) ? 0x1F : 0;
		b = (i & 4) ? 0x1F : 0;

		if (i & 8) {
			if (r) r |= 0x20;
			if (g) g |= 0x20;
			if (b) b |= 0x20;
		}
		
		outb(DAC_DATA, r);
		outb(DAC_DATA, g);
		outb(DAC_DATA, b);
	}
}

void set_color_gray(void) {
	uint32_t i;

	for (i = 0; i < 256; i++) {
		outb(DAC_WRITE_INDEX, i);
		outb(DAC_DATA, i & 0x3F);
		outb(DAC_DATA, i & 0x3F);
		outb(DAC_DATA, i & 0x3F);
	}
}

void set_color_mono(void) {
	uint32_t i;
	uint8_t v;

	for (i = 0; i < 256; i++) {
		outb(DAC_WRITE_INDEX, i);
		v = (i & 1) ? 0x3F : 0x00;
		outb(DAC_DATA, v);
		outb(DAC_DATA, v);
		outb(DAC_DATA, v);
	}
}
