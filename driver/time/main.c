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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <mutex.h>
#include <natio.h>
#include <proc.h>

#include "time.h"

static bool m_time = false;

void time_init(int argc, char **argv) {
	struct fs_obj *root;

	root        = calloc(sizeof(struct fs_obj), 1);
	root->type  = FOBJ_FILE;
	root->size  = 0;
	root->inode = 0;
	root->acl   = acl_set_default(root->acl, ACL_READ);
	lfs_root(root);
}

size_t time_read(struct fs_obj *file, uint8_t *buffer, size_t size, uint64_t offset) {
	char *data;

	if (size > 20) {
		size = 20;
	}

	data = malloc(21);
	mutex_spin(&m_time);
	sprintf(data, "%d", get_time());
	mutex_free(&m_time);
	memcpy(buffer, data, size);
	free(data);

	return size;
}

struct driver time_driver = {
	time_init,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	time_read,
	NULL,
	NULL,
	NULL,

	NULL,
};

int main(int argc, char **argv) {

	driver_init(&time_driver, argc, argv);

	msend(PORT_CHILD, getppid(), NULL);
	_done();

	return 0;
}
