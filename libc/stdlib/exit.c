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

#include <stdlib.h>

#include <rho/natio.h>
#include <rho/proc.h>
#include <rho/ipc.h>
#include <rho/abi.h>

/****************************************************************************
 * exit
 *
 * Exit the current process with status <status>. Performs all functions
 * registered with atexit.
 */

void exit(int status) {
	struct __atexit_func *f;
	
	while (__atexit_func_list) {
		f = __atexit_func_list;
		f->function();
		__atexit_func_list = f->next;
		free(f);
	}

	msendb(RP_CONS(getppid(), 0), PORT_CHILD);
	__exit(status);
} 
