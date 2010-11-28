/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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

#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <mutex.h>
#include <proc.h>
#include <mmap.h>

#define WIDTH 80
#define HEIGHT 25
#define TAB 8

static uint16_t c_base = 0;
static uint16_t cursor = 0;
static uint16_t buffer = 0;
static uint16_t *vbuf  = NULL;
static bool m_vbuf     = false;
static void char_write(char c);

void term_init(int argc, char **argv) {
	struct fs_obj *root;
	size_t i;

	root = calloc(sizeof(struct fs_obj), 1);
	root->type = FOBJ_FILE;
	root->size = 0;
	root->inode = 0;
	root->acl = acl_set_default(root->acl, ACL_WRITE);

	mutex_spin(&m_vbuf);

	vbuf = valloc(WIDTH * HEIGHT * 2);
	emap(vbuf, 0xB8000, PROT_READ | PROT_WRITE);

	for (i = 0; i < WIDTH * HEIGHT; i++) {
		vbuf[i] = 0x0F00 | ' ';
	}

	mutex_free(&m_vbuf);

	lfs_root(root);
}

size_t term_write(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	size_t i;
	
	mutex_spin(&file->mutex);

	for (i = 0; i < size; i++) {
		char_write(buffer[i]);
	}

	outb(0x3D4, 14);
	outb(0x3D5, cursor >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, cursor & 0xFF);

	mutex_free(&file->mutex);
	
	return size;
}

struct driver term_driver = {
	term_init, 

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,
	term_write,
	NULL,
	NULL,

	NULL,
};

int main(int argc, char **argv) {

	driver_init(&term_driver, argc, argv);

	psend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
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
