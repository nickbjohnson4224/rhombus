/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#ifndef SVGA_H
#define SVGA_H

#include <stdint.h>

struct vbe_ctrlinfo {
	char     signature[4];
	uint16_t version;
	uint32_t oem_string_ptr;
	uint8_t  caps[4];
	uint32_t mode_ptr;
	uint16_t memory;

	uint16_t soft_rev;
	uint32_t vendor_name;
	uint32_t product_name;
	uint32_t product_rev;
	uint16_t reserved;
	uint16_t oem_data;
};

extern struct vbe_ctrlinfo *svga_controller;
int vbe_readctrl(void);

struct vbe_modeinfo {
	uint16_t attributes;
	uint8_t  win_a;
	uint8_t  win_b;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t seg_a;
	uint16_t seg_b;
	uint32_t func_ptr;
	uint32_t scan_bytes;

	// VBE 1.2+
	uint16_t xres;
	uint16_t yres;
	uint8_t  xchar;
	uint8_t  ychar;
	uint8_t  planes;
	uint8_t  depth;
	uint8_t  banks;
	uint8_t  mem_model;
	uint8_t  bank_size;
	uint8_t  images;
	uint8_t  reserved0;

	// direct color
	uint8_t  red_size;
	uint8_t  res_off;
	uint8_t  green_size;
	uint8_t  green_off;
	uint8_t  blue_size;
	uint8_t  blue_off;
	uint8_t  resvd_size;
	uint8_t  resvd_off;
	uint8_t  dcolor_mode;
	
	// VBE 2+
	uint32_t linear_ptr;
	uint32_t reserved1;
	uint16_t reserved2;

	// VBE 3+
	uint16_t lin_bytes_line;
	uint8_t  bnk_images;
	uint8_t  lin_images;
	uint8_t  lin_red_size;
	uint8_t  lin_red_off;
	uint8_t  lin_green_size;
	uint8_t  lin_green_off;
	uint8_t  lin_blue_size;
	uint8_t  lin_blue_off;
	uint8_t  lin_resvd_size;
	uint8_t  lin_resvd_off;
	uint32_t max_pixel_clock;
};

extern struct vbe_modeinfo **svga_modev;
int vbe_readmode(uint16_t mode);

#endif/*SVGA_H*/
