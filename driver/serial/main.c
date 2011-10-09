/*
 * Copyright (C) 2011 Jaagup Repan <jrepan at gmail.com>
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

#include <stdlib.h>
#include <mutex.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>
#include <rdi/io.h>

#define PORT 0x3F8 // COM1

void char_write(char c) {

	while (!(inb(PORT + 5) & 0x20));
	outb(PORT, c);
	
	if (c == '\n') {
		char_write('\r');
	}
}

size_t serial_write(struct robject *self, rp_t source, uint8_t *buffer, size_t size, off_t offset) {
	size_t i;
	
	for (i = 0; i < size; i++) {
		char_write(buffer[i]);
	}

	return size;
}

int main(int argc, char **argv) {
	struct robject *root;

	rdi_init();

	// create device file
	root = rdi_file_cons(0, PERM_READ | PERM_WRITE);
	robject_set(0, root);
	robject_root = root;

	// initialize serial port
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03); // 38400 baud
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B);

	// set interface functions
	rdi_global_write_hook = serial_write;
	
	// daemonize
	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
