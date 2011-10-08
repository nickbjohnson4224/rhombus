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

#include <rdi/core.h>
#include <rdi/access.h>
#include <rdi/vfs.h>
#include <rdi/io.h>

#include <natio.h>

void rdi_init_core() {
	
	rcall_set("cons",  __rdi_cons_handler);
	rcall_set("open",  __rdi_open_handler);
//	rcall_set("close", __rdi_close_handler);
	rcall_set("type",  __rdi_type_handler);
	rcall_set("size",  __rdi_size_handler);
}

void rdi_init_all() {

	rdi_init_core();
	rdi_init_io();
	rdi_init_vfs();
	vfs_init();
}

void rdi_init() {

	__rdi_class_core_setup();
	__rdi_class_event_setup();
	__rdi_class_dir_setup();
	__rdi_class_link_setup();
	__rdi_class_file_setup();
}
