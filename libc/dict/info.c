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

#include <dict.h>
#include <arch.h>
#include <mmap.h>
#include <string.h>

/****************************************************************************
 * dict_info
 *
 * Structure at the beginning of the dictionary heap that stores basic
 * information about the dictionary state.
 */

struct __info *dict_info = (void*) DICTIONARY;

/****************************************************************************
 * dict_init
 *
 * Initialize the dictionary. This function is only called by init, and
 * should NOT be called by any normal process (unless it wants its dictionary
 * cleared and its only connections to the rest of the system severed).
 */

void dict_init(void) {
	
	mmap(dict_info, sizeof(struct __info), MMAP_READ | MMAP_WRITE);

	memclr(dict_info, sizeof(struct __info));
	dict_info->brk = (uintptr_t) dict_info + sizeof(struct __info);
}
