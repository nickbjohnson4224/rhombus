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

#include <string.h>
#include <stdlib.h>
#include <natio.h>

/*****************************************************************************
 * ator
 *
 * Converts the resource pointer string <str> to a resource pointer.
 */

uint64_t ator(const char *str) {
	uint32_t index;
	uint32_t pid;
	char *substr;

	if (!str || str[0] != 'r') {
		return 0;
	}

	str++;

	// extract pid
	substr = struntil(str, ":", &str);
	pid = atoi(substr);
	free(substr);

	str++;

	// extract index
	index = atoi(str);

	return RP_CONS(pid, index);
}
