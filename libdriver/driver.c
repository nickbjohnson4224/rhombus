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

#include <driver.h>
#include <stdlib.h>
#include <proc.h>
#include <ipc.h>

/*****************************************************************************
 * active_driver
 *
 * The active driver, obviously. The functions in this structure are used to
 * handle filesystem and file requests.
 */

struct driver *active_driver = NULL;

/*****************************************************************************
 * driver_init
 *
 * Initializes the driver <driver> on the current process, allowing read/write
 * and filesystem control messages.
 */

void driver_init(struct driver *driver, int argc, char **argv) {
	struct fs_obj *root;
	
	active_driver = driver;

	when(PORT_IRQ,   irq_wrapper);
	when(PORT_FS,    lfs_wrapper);
	when(PORT_READ,  read_wrapper);
	when(PORT_WRITE, write_wrapper);
	when(PORT_SYNC,  sync_wrapper);
	when(PORT_RESET, reset_wrapper);

	if (active_driver->init) {
		active_driver->init(argc, argv);
	}
	else {
		root = calloc(sizeof(struct fs_obj), 1);
		root->type = FOBJ_FILE;
		root->size = 0;
		root->inode = 0;
		lfs_root(root);
	}
}
