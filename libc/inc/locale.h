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

#ifndef __RLIBC_LOCALE_H
#define __RLIBC_LOCALE_H

#include <stddef.h>

/* locale conversion structure *********************************************/

struct lconv {
	const char *decimal_point;
	const char *grouping;
	const char *thousands_sep;
	const char *currency_symbol;
	const char *int_curr_symbol;

	const char *mon_decimal_point;
	const char *mon_grouping;
	const char *mon_thousands_sep;

	const char *negative_sign;
	const char *positive_sign;

	char  frac_digits;
	char  int_frac_digits;

	char  n_cs_precedes;
	char  n_sep_by_space;
	char  n_sign_posn;
	
	char  p_cs_precedes;
	char  p_sep_by_space;
	char  p_sign_posn;
};

extern struct lconv *__current_locale;
extern struct lconv __default_locale;
extern struct lconv __default_c_locale;

/* get locale **************************************************************/

struct lconv *localeconv(void);

/* set locale **************************************************************/

#define LC_ALL		0
#define LC_COLLATE	1
#define LC_CTYPE	2
#define LC_MONETARY	3
#define LC_NUMERIC	4
#define LC_TIME		5

char *setlocale(int category, const char *locale);

#endif/*__RLIBC_LOCALE_H*/
