/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/driver.h>
#include <flux/mmap.h>
#include <flux/signal.h>
#include <flux/request.h>

#include <driver/vga.h>

static void vga_init (device_t selector);
static void vga_halt (void);
static void vga_write(uint32_t caller, req_t *req);

#define VMEM 0xA0000
#define SIZE 0x10000

#define VGA_INDEX	0x3C0
#define VGA_DATA	0x3C0
#define VGA_MISC_O	0x3C2
#define VGA_MISC_I	0x3CC

static uint8_t *vmem;
static void draw_pixel(uint16_t x, uint16_t y, uint8_t r, uint8_t g, uint8_t b);

struct driver_interface vga = {
	vga_init,
	vga_halt,

	NULL,
	0,
};

static void vga_init(device_t selector) {
	size_t i;

	/* Allocate video memory */
	vmem = malloc(SIZE);

	for (i = 0; i < (SIZE / PAGESZ); i++) {
		emap(&vmem[i * PAGESZ], VMEM + (i * PAGESZ), PROT_READ | PROT_WRITE);
	}

}

static void vga_halt(void) {
	return;
}
