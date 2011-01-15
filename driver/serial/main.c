/*
 * Copyright (C) 2011 Jaagup Repan
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
#include <driver.h>
#include <mutex.h>
#include <proc.h>

#define PORT 0x3f8 // COM1

void char_write(char c) {
   while (!(inb(PORT + 5) & 0x20));
   outb(PORT, c);
   if (c == '\n')
	   char_write('\r');
}

size_t serial_write(struct vfs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	mutex_spin(&file->mutex);

	for (size_t i = 0; i < size; i++) {
		char_write(buffer[i]);
	}

	mutex_free(&file->mutex);
	
	return size;
}

int main(int argc, char **argv) {
	struct vfs_obj *root;

	root = calloc(sizeof(struct vfs_obj), 1);
	root->type = FOBJ_FILE;
	root->size = 0;
	root->acl = acl_set_default(root->acl, FS_PERM_WRITE);
	vfs_set_index(0, root);

	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03); // 38400 baud
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
	outb(PORT + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
	outb(PORT + 4, 0x0B);

	di_wrap_write(serial_write);
	vfs_wrap_init();

	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
