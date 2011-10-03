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

#ifndef _RLIBC_HASH_H
#define _RLIBC_HASH_H

#include <stddef.h>
#include <stdint.h>

/*****************************************************************************
 * Rhombus hash functions
 *
 * This is a collection of hash functions intended to make the creation of
 * hashtables and checksums easier for programmers. For now, it does not
 * intend to contain any functions suitable for cryptography, and likely
 * never will.
 */

/*****************************************************************************
 * Cyclic Redunancy Checks (CRCs)
 *
 * CRCs are used in many applications for detecting random (non-malicious)
 * errors in a block of data. They are really a family of algorithms, each 
 * characterized by a specific standardized polynomial. Currently, this 
 * library supports the following common polynomials (i.e. algorithms):
 *
 *   CRC_8
 *   CRC_16
 *   CRC_32
 *   CRC_32C
 *
 * The crcx() and crcx_gen() functions may be used to perform a CRC with an 
 * arbitrary polynomial.
 *
 * CRCs may or may not make good hash functions, depending on the polynomial
 * used. They are not at all cryptographically secure.
 */

// constants for specifying builtin polynomials
#define CRC_8		0x00
#define CRC_16		0x01
#define CRC_32		0x02
#define CRC_32C		0x03

// number of builtin polynomials
#define CRC_COUNT	0x04

uint8_t  crc8 (int poly, const void *data, size_t length);
uint16_t crc16(int poly, const void *data, size_t length);
uint32_t crc32(int poly, const void *data, size_t length);
uint64_t crc64(int poly, const void *data, size_t length);

/*****************************************************************************
 * Generic Hash Functions
 *
 */

#define HASH_PEARSON 0
#define HASH_CRC     1
#define HASH_LOOKUP3 2
#define HASH_SDBM    3

uint8_t  hash8     (int algorithm, const void *data, size_t length);
uint8_t  hash8_str (int algorithm, const char *string);
uint16_t hash16    (int algorithm, const void *data, size_t length);
uint16_t hash16_str(int algorithm, const char *string);
uint32_t hash32    (int algorithm, const void *data, size_t length);
uint32_t hash32_str(int algorithm, const char *string);
uint64_t hash64    (int algorithm, const void *data, size_t length);
uint64_t hash64_str(int algorithm, const char *string);

/*****************************************************************************
 * Pearson Hashing
 *
 * Pearson hashing is a simple 8-bit hash with very good collision properties. 
 * One of its defining traits is that it produces hash values with very good
 * bit distributions, i.e. suitable for power-of-two-sized hash tables. It is, 
 * however, comparatively slow.
 *
 * hash_pearson8() implements the standard Pearson hash.
 *
 * hash_pearson16(), hash_pearson32(), and hash_pearson64() all perform
 * the Pearson hash multiple times in parallel, but with different permutation
 * tables for each hash, then concatenate them.
 */

// standard Pearson hash
uint8_t hash_pearson8    (const void *data, size_t length);
uint8_t hash_pearson8_str(const char *str);

// composite Pearson hash
uint16_t hash_pearson16    (const void *data, size_t length);
uint16_t hash_pearson16_str(const char *str);
uint32_t hash_pearson32    (const void *data, size_t length);
uint32_t hash_pearson32_str(const char *str);
uint64_t hash_pearson64    (const void *data, size_t length);
uint64_t hash_pearson64_str(const char *str);

#endif/*_RLIBC_HASH_H*/
