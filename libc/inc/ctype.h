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

#ifndef CTYPE_H
#define CTYPE_H

#include <stdint.h>
#include <stdbool.h>

/* character identification ************************************************/

extern uint16_t _ctype_tbl[256];

#define isalnum(c) (_ctype_tbl[(size_t) (c)] & 0x001)
#define isalpha(c) (_ctype_tbl[(size_t) (c)] & 0x002)
#define isblank(c) (_ctype_tbl[(size_t) (c)] & 0x004)
#define iscntrl(c) (_ctype_tbl[(size_t) (c)] & 0x008)
#define isdigit(c) (_ctype_tbl[(size_t) (c)] & 0x010)
#define isgraph(c) (_ctype_tbl[(size_t) (c)] & 0x020)
#define islower(c) (_ctype_tbl[(size_t) (c)] & 0x040)
#define isprint(c) (_ctype_tbl[(size_t) (c)] & 0x080)
#define ispunct(c) (_ctype_tbl[(size_t) (c)] & 0x100)
#define isspace(c) (_ctype_tbl[(size_t) (c)] & 0x200)
#define isupper(c) (_ctype_tbl[(size_t) (c)] & 0x400)
#define isxdigit(c)(_ctype_tbl[(size_t) (c)] & 0x800)

bool __isbdigit(char c, int base);

/* character case **********************************************************/

int tolower(int c);
int toupper(int c);

#endif/*CTYPE_H*/
