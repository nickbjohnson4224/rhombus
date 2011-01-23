/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <interrupt.h>
#include <space.h>
#include <elf.h>

/*****************************************************************************
 * syscall_exec (int 0x4b)
 * 
 * (no arguments)
 *
 * Executes the executable image at ESPACE. This image must be an ELF file.
 * This system call is going to be removed once userspace executable loading
 * is finished, so for now it is not secure and can be used to crash the
 * kernel. Returns zero and jumps to the loaded executable on success, returns
 * nonzero on failure.
 */

struct thread *syscall_exec(struct thread *image) {
	struct elf32_ehdr *file;

	file = (void*) ESPACE;

	if (elf_check_file(file)) {
		image->eax = -1;
		return image;
	}

	mem_free(0, SSPACE);

	elf_load_file(file);
	image->eip = file->e_entry;
	image->useresp = image->stack + SEGSZ;
	image->proc->entry = 0;

	return image;
}
