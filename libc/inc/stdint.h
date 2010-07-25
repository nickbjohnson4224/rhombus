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

#ifndef STDINT_H
#define STDINT_H

/* integer types ***********************************************************/

/* precise size integers */
typedef unsigned char			uint8_t;
typedef unsigned short			uint16_t;
typedef unsigned int			uint32_t;
typedef unsigned long long int	uint64_t;
typedef signed char				int8_t;
typedef signed short			int16_t;
typedef signed int				int32_t;
typedef signed long long int	int64_t;

/* minimum size integers */
typedef uint32_t		uint_least8_t;
typedef uint32_t		uint_least16_t;
typedef uint32_t		uint_least32_t;
typedef uint64_t		uint_least64_t;
typedef int32_t			int_least8_t;
typedef int32_t			int_least16_t;
typedef int32_t			int_least32_t;
typedef int64_t			int_least64_t;

/* fast integers */
typedef uint32_t		uint_fast8_t;
typedef uint32_t		uint_fast16_t;
typedef uint32_t		uint_fast32_t;
typedef uint64_t		uint_fast64_t;
typedef int32_t			int_fast8_t;
typedef int32_t			int_fast16_t;
typedef int32_t			int_fast32_t;
typedef int64_t			int_fast64_t;

/* pointer equivalents */
typedef uint32_t		uintptr_t;
typedef int32_t			intptr_t;

/* maximum size integers */
typedef uint64_t		uintmax_t;
typedef int64_t			intmax_t;

/* various integer types */
typedef uint32_t		size_t;

/* integer limits **********************************************************/

#define INT8_MIN		-0x80
#define INT8_MAX		0x7F
#define UINT8_MAX		0xFF

#define INT16_MIN		-0x8000
#define INT16_MAX		0x7FFF
#define UINT16_MAX		0xFFFF

#define INT32_MIN		-0x80000000
#define INT32_MAX		0x7FFFFFFF
#define UINT32_MAX		0xFFFFFFFF

#define INT64_MIN		-0x8000000000000000LL
#define INT64_MAX		0x7FFFFFFFFFFFFFFFLL
#define UINT64_MAX		0xFFFFFFFFFFFFFFFFULL

#define INT_LEAST8_MIN		INT8_MIN
#define INT_LEAST8_MAX		INT8_MAX
#define UINT_LEAST8_MAX		UINT8_MAX

#define INT_LEAST16_MIN		INT16_MIN
#define INT_LEAST16_MAX		INT16_MAX
#define UINT_LEAST16_MAX	UINT16_MAX

#define INT_LEAST32_MIN		INT32_MIN
#define INT_LEAST32_MAX		INT32_MAX
#define UINT_LEAST32_MAX	UINT32_MAX

#define INT_LEAST64_MIN		INT64_MIN
#define INT_LEAST64_MAX		INT64_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

#define INT_FAST8_MIN		INT32_MIN
#define INT_FAST8_MAX		INT32_MAX
#define UINT_FAST8_MAX		UINT32_MAX

#define INT_FAST16_MIN		INT32_MIN
#define INT_FAST16_MAX		INT32_MAX
#define UINT_FAST16_MAX		UINT32_MAX

#define INT_FAST32_MIN		INT32_MIN
#define INT_FAST32_MAX		INT32_MAX
#define UINT_FAST32_MAX		UINT32_MAX

#define INT_FAST64_MIN		INT64_MIN
#define INT_FAST64_MAX		INT64_MAX
#define UINT_FAST64_MAX		UINT64_MAX

#define INTPTR_MIN			INT32_MIN
#define INTPTR_MAX			INT32_MAX
#define UINTPTR_MAX			UINT32_MAX

#define INTMAX_MIN			INT64_MIN
#define INTMAX_MAX			INT64_MAX
#define UINTMAX_MAX			UINT64_MAX

#define SIZE_MAX			UINT32_MAX

/* null pointer constant ***************************************************/

#define NULL ((void*) 0)

#endif/*STDINT_H*/
