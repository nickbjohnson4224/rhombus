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

#include <util.h>
#include <ktime.h>
#include <space.h>
#include <init.h>
#include <debug.h>

typedef void (*init_t)(void);

init_t init_list[] = {
	mem_init,
	thread_init,
	process_init,
	init_task,
	NULL
};

struct multiboot *mboot;

typedef void (*entry_t)();

void *init(void *mboot_ptr, uint32_t mboot_magic) {
	extern void halt(void);
	extern uint32_t get_cr0(void);
	uint32_t i;
	struct thread *boot_image;

	debug_init();
	debug_printf("Flux Operating System Kernel v0.5a\n");

	if (mboot_magic != 0x2BADB002) {
		debug_panic("Bootloader is not multiboot compliant");
	}
	mboot = mboot_ptr;

	for (i = 0; init_list[i]; i++) {
		init_list[i]();
	}

	boot_image = thread_alloc();

	debug_printf("dropping to usermode\n");

	return &boot_image->tss_start;
}
