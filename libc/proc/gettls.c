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

#include <stdint.h>
#include <rho/arch.h>
#include <rho/proc.h>
#include <rho/abi.h>

/****************************************************************************
 * gettls
 *
 * Returns the current thread's "thread-local segment", an area of memory
 * which is associated only with the current thread. This area of memory
 * contains the stack, which grows downward from the top. All of this
 * segment will be dynamically allocated by the kernel on page faults, for
 * stack reasons. Other data may be stored in the segment, but be wary of
 * stack overflows.
 */

uint32_t gettls(void) {
	int x;

	return ((uint32_t) &x) & ~(0x3FFFFFU);
}
