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

#ifndef SETJMP_H
#define SETJMP_H

#include <stdint.h>

/* stack context structure **************************************************/

struct __jmp_buf {
	uintptr_t esp;
	uintptr_t ebp;
	uintptr_t eip;

	uintptr_t ebx;
	uintptr_t edi;
	uintptr_t esi;
} __attribute__ ((packed))i;

typedef struct __jmp_buf jmp_buf[1];

/* save stack context *******************************************************/

int setjmp(jmp_buf env);

/* load stack context *******************************************************/

int longjmp(jmp_buf env, int val);

#endif/*SETJMP_H*/
