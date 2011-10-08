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

#include <stdlib.h>
#include <string.h>
#include <hash.h>

/*****************************************************************************
 * _crc_poly
 *
 * A table of builtin CRC polynomials corresponding to the contstants in
 * hash.h.
 */

/*static uint32_t _crc_poly[CRC_COUNT][3] = {

	// CRC-8
	{ 0xD5, 0xAB, 0xEA },

	// CRC-16
	{ 0x8005, 0xA001, 0xC002 },

	// CRC-32
	{ 0x04C11DB7, 0xEDB88320, 0x82608EDB },

	// CRC-32C
	{ 0x1EDC6F41, 0x82F63B78, 0x8F6E37A0 },
};*/
