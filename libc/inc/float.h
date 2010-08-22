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

#ifndef FLOAT_H
#define FLOAT_H

#define FLT_RADIX		2
#define FLT_ROUNDS		-1
#define FLT_EVAL_METHOD	0

/* float specifics *********************************************************/

#define FLT_MANT_DIG	23
#define FLT_DIG			7
#define FLT_MIN_EXP		(-126)
#define FLT_MAX_EXP		127
#define FLT_MIN_10_EXP	(-38)
#define FLT_MAX_10_EXP	38
#define FLT_MAX			(1.7014118346046923E+38)
#define FLT_EPSILON		(1.1754943508222875E-38)
#define FLT_MIN			(2.2420775429197073E-44)

/* double specifics ********************************************************/

#define DBL_MANT_DIG	52
#define DBL_DIG			16
#define DBL_MIN_EXP		(-1022)
#define DBL_MAX_EXP		1023
#define DBL_MIN_10_EXP	(-308)
#define DBL_MAX_10_EXP	308
#define DBL_MAX			(8.9884656743115795E+307)
#define DBL_EPSILON		(2.2250738585072014E-308)
#define DBL_MIN			(4.9406564584124654E-324)

/* long double specifics ***************************************************/

/* NOTE - copied from double */

#define LDBL_MANT_DIG	52
#define LDBL_DIG		16
#define LDBL_MIN_EXP	(-1022)
#define LDBL_MAX_EXP	1023
#define LDBL_MIN_10_EXP	(-308)
#define LDBL_MAX_10_EXP	308
#define LDBL_MAX		(8.9884656743115795E+307)
#define LDBL_EPSILON	(2.2250738585072014E-308)
#define LDBL_MIN		(4.9406564584124654E-324)

#endif/*FLOAT_H*/
