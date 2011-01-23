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

#include <locale.h>

/****************************************************************************
 * __current_locale
 *
 * Currently used locale structure. Initally set to default locale.
 */

struct lconv *__current_locale = &__default_locale;

/****************************************************************************
 * __default_locale
 *
 * Default locale structure. New locales should use this as a template.
 */

struct lconv __default_locale = {
	".",	/* decimal point */
	"",		/* grouping (?) */
	"",		/* thousands separator */
	"$",	/* currency symbol */
	"$",	/* international currency symbol */

	".",	/* monetary decimal point */
	"",		/* monetary grouping (?) */
	",",	/* monetary thousands separator */

	"-",	/* negative sign */
	"",		/* positive sign */

	2,		/* decimal digits in monetary values */
	2,		/* decimal digits in int'l monetary values */
	
	1,		/* currency symbol {0 - follows, 1 - precedes} negative value */
	0,		/* currency symbol {0 - isn't, 1 - is} separated from negative value */
	3,		/* sign position for negative values */

	1,		/* currency symbol {0 - follows, 1 - precedes} positive value */
	0,		/* currency symbol {0 - isn't, 1 - is} separated from positive value */
	3		/* sign position for positive values */
};

/****************************************************************************
 * __default_c_locale
 *
 * Default C locale structure.
 */

struct lconv __default_c_locale = {
	".",	/* decimal point */
	"",		/* grouping (?) */
	"",		/* thousands separator */
	"$",	/* currency symbol */
	"$",	/* international currency symbol */

	".",	/* monetary decimal point */
	"",		/* monetary grouping (?) */
	"",		/* monetary thousands separator */

	"-",	/* negative sign */
	"",		/* positive sign */

	2,		/* decimal digits in monetary values */
	2,		/* decimal digits in int'l monetary values */
	
	1,		/* currency symbol {0 - follows, 1 - precedes} negative value */
	0,		/* currency symbol {0 - isn't, 1 - is} separated from negative value */
	3,		/* sign position for negative values */

	1,		/* currency symbol {0 - follows, 1 - precedes} positive value */
	0,		/* currency symbol {0 - isn't, 1 - is} separated from positive value */
	3		/* sign position for positive values */
};
