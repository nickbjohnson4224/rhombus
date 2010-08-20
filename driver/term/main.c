/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <driver.h>
#include <mutex.h>
#include <ipc.h>
#include <proc.h>
#include <mmap.h>

#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define VMEM 0xB8000
#define WIDTH 80
#define HEIGHT 25
#define TAB 8

static void terminal_write(struct packet *packet, uint8_t port, uint32_t caller);

static uint16_t *vbuf;
static uint16_t c_base = 0;
static uint16_t cursor = 0;
static uint16_t buffer = 0;
static void char_write(char c);

static bool m_vbuf = 0;

int main() {
	size_t i;

	mutex_spin(&m_vbuf);

	vbuf = valloc(80 * 25 * 2);
	emap(vbuf, VMEM, PROT_READ | PROT_WRITE);

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F00 | ' ';
	}

	mutex_free(&m_vbuf);

	when(PORT_WRITE, terminal_write);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}

static void terminal_write(struct packet *packet, uint8_t port, uint32_t caller) {
	size_t i;
	char *buffer;

	if (!packet) {
		return;
	}

	buffer = pgetbuf(packet);

	mutex_spin(&m_vbuf);

	for (i = 0; i < packet->data_length; i++) {
		char_write(buffer[i]);
	}

	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);

	mutex_free(&m_vbuf);

	psend(PORT_REPLY, caller, packet);
	pfree(packet);
}

static void char_write(char c) {
	uint16_t i;

	if (cursor >= WIDTH * HEIGHT) {
		for (i = 0; i < WIDTH * HEIGHT - WIDTH; i++) {
			vbuf[i] = vbuf[i + WIDTH];
		}

		for (i = WIDTH * HEIGHT - WIDTH; i < WIDTH * HEIGHT; i++) {
			vbuf[i] = 0x0F20;
		}
		
		c_base -= WIDTH;
		cursor -= WIDTH;
	}

	switch (c) {
		case '\0': 
			break;
		case '\t': 
			cursor = (cursor + TAB) - (cursor % TAB); 
			break;
		case '\n': 
			buffer = c_base;
			vbuf[cursor] = 0x0000;
			c_base = cursor = (cursor - cursor % WIDTH) + 80; 
			break;
		case '\r': 
			cursor = (cursor - cursor % WIDTH); 
			c_base = cursor; 
			break;
		case '\b': 
			if (c_base < cursor) cursor--; 
			vbuf[cursor] = 0x0F00; 
			break;
		default: 
			vbuf[cursor++] = (uint16_t) (c | 0x0F00); 
			break;
	}

	if (cursor >= WIDTH * HEIGHT) {
		for (i = 0; i < WIDTH * HEIGHT - WIDTH; i++) {
			vbuf[i] = vbuf[i + WIDTH];
		}

		for (i = WIDTH * HEIGHT - WIDTH; i < WIDTH * HEIGHT; i++) {
			vbuf[i] = 0x0F20;
		}
		
		c_base -= WIDTH;
		cursor -= WIDTH;
	}
}
