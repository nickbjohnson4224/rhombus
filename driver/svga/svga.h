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

/* Resize event reporting ***************************************************/

extern struct event_list *event_list;

/* VESA BIOS extensions driver **********************************************/

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
} __attribute__((packed));

struct vbe_modeinfo {
	uint16_t attributes;
	uint8_t  win_a;
	uint8_t  win_b;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t seg_a;
	uint16_t seg_b;
	uint32_t func_ptr;
	uint16_t scan_bytes;

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
	uint8_t  red_off;
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
} __attribute__((packed));

struct vbe_ctrcinfo {
	uint16_t horiz_total;
	uint16_t horiz_start;
	uint16_t horiz_end;
	uint16_t vert_total;
	uint16_t vert_start;
	uint16_t vert_end;
	uint8_t  flags;
	uint32_t pixel_clock;
	uint16_t refresh;
} __attribute__((packed));

#define FIXPTR(ptr) ((ptr) = ((ptr & 0xFFFF) | (ptr >> 12)))

extern struct vbe_crtcinfo *svga_crtc;
extern struct vbe_ctrlinfo *svga_ctrl;
extern struct vbe_modeinfo *svga_mode;

int vbe_readctrl(void);
int vbe_readmode(uint16_t mode);
int vbe_setmode (uint16_t mode, uint8_t flags);
int vbe_setbank (uint8_t window, uint16_t bank);

int vbe_cmd(uint8_t num, uint16_t bx, uint16_t cx, uint16_t dx, uint32_t esdi);

/* SVGA driver **************************************************************/

extern struct svga {
	int mode; // 0 - banked, 1 - linear

	// linear mode
	uint8_t *linear;
	uint32_t linear_size;

	// banked mode
	uint8_t *window0;
	uint8_t *window1;
	uint32_t window_size;

	// resolution
	uint16_t w;  // width
	uint16_t h;  // height
	uint16_t d;  // depth
	uint16_t p;  // pitch
	uint16_t pw; // pixel width

	// color information
	uint8_t  r_size;
	uint8_t  r_shft;
	uint8_t  g_size;
	uint8_t  g_shft;
	uint8_t  b_size;
	uint8_t  b_shft;
} svga;

extern struct svga_mode {
	uint16_t vesa_mode;
	uint16_t w;
	uint16_t h;
	uint16_t d;
} *modelist;
extern int modelist_count;

int svga_init     (void); 
int svga_find_mode(int width, int height, int depth);
int svga_set_mode (int mode);
int svga_flip     (uint32_t *buffer);
int svga_fliprect (uint32_t *buffer, int x, int y, int w, int h);

#endif/*SVGA_H*/
