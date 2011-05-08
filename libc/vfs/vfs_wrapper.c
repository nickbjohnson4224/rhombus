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

#include <natio.h>
#include <ipc.h>
#include <vfs.h>

/*****************************************************************************
 * vfs_init
 *
 * Initialize VFS request handling system. Returns zero on success, nonzero on
 * error.
 */

int vfs_init(void) {
	when(PORT_FIND, __find_wrapper);
	when(PORT_CONS, __cons_wrapper);
	when(PORT_MOVE, __move_wrapper);
	when(PORT_REMV, __remv_wrapper);
	when(PORT_LINK, __link_wrapper);
	when(PORT_LIST, __list_wrapper);
	when(PORT_SIZE, __size_wrapper);
	when(PORT_TYPE, __type_wrapper);
	when(PORT_PERM, __perm_wrapper);
	when(PORT_AUTH, __auth_wrapper);

	rcall_set("fs_find", __find_rcall_wrapper);

	return 0;
}
