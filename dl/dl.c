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

#include "dl.h"

struct dl_object dl_object_tab[256];

int dl__init(void) {
	int i;

	for (i = 0; i < 256; i++) {
		dl_object_tab[i].soname[0] = '\0';
	}

	return 0;
}

int dl__get(const char *soname) {
	int i;

	for (i = 0; i < 256; i++) {

		if (!dl_object_tab[i].soname[0]) {
			return -1;
		}

		if (!dl_strcmp(dl_object_tab[i].soname, soname)) {
			return i;
		}
	}

	return -1;
}

int dl__load(void *image, uint32_t size, const char *soname) {
	int object;

	for (object = 0; object < 256; object++) {
		if (!dl_object_tab[object].soname[0]) {
			break;
		}
	}

	if (object == 256) {
		return -1;
	}

	if (!dl_elf_check(image)) {
		return -1;
	}

	dl_elf_load(image);
	dl_page_free(image, size);

	return 0;
}

int dl__fix(int object, int index, uint32_t value) {

	dl_object_tab[object].got[index] = value;

	return 0;
}

uint32_t dl__sym(int object, const char *symbol) {
	return 0;
}
