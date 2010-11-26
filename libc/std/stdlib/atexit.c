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

#include <stdlib.h>
#include <errno.h>

/****************************************************************************
 * __atexit_func_list
 *
 * List of functions to be called at process exit.
 */

struct __atexit_func *__atexit_func_list = NULL;

/****************************************************************************
 * atexit
 *
 * Register a function to be called on process exit. Functions registered
 * will be called in FILO order. Returns 0 on success, nonzero on failure.
 */

int atexit(void (*function)(void)) {
	struct __atexit_func *f;

	f = malloc(sizeof(struct __atexit_func));

	if (!f) {
		errno = ENOMEM;
		return -1;
	}

	f->next = __atexit_func_list;
	f->function = function;
	__atexit_func_list = f;

	return 0;
}
