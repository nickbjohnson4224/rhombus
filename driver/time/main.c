/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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
#include <stdlib.h>
#include <string.h>
#include <mutex.h>
#include <natio.h>
#include <proc.h>

#include <rdi/core.h>
#include <rdi/vfs.h>
#include <rdi/arch.h>
#include <rdi/io.h>

#include "time.h"

size_t time_read(uint64_t source, uint32_t index, uint8_t *buffer, size_t size, uint64_t offset) {
	char *data;

	if (size > 20) {
		size = 20;
	}

	data = malloc(21);
	sprintf(data, "%d", get_time());
	memcpy(buffer, data, size);
	free(data);

	return size;
}

int main(int argc, char **argv) {

	index_set(0, resource_cons(FS_TYPE_FILE, PERM_READ));

	rdi_set_read(time_read);
	rdi_init_all();

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	_done();

	return 0;
}
