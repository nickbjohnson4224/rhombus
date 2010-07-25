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
#include <stdint.h>
#include <dict.h>
#include <string.h>

/****************************************************************************
 * getenv
 *
 * Searches the current environment for the key <name>. Returns the found
 * value on success, or NULL on failure.
 */

const char *getenv(const char *name) {

	return (const char*) dict_readstrns("env:", name);
}

/****************************************************************************
 * setenv
 *
 * Sets the value of the environment key <key> to the value <value>. Returns
 * 0 on success, or nonzero on failure.
 */

int setenv(const char *name, const char *value) {

	dict_writestrns("env:", name, (const uint8_t*) value, strlen(value));

	return 0;
}
