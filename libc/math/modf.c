/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <math.h>
#include <float.h>
#include <stdint.h>

/* If GCC/CLang builtins are available, use them */
#ifdef __GNUC__

float modff(float x, float *iptr) {
	return __builtin_modff(x, iptr);
}

double modf(double x, double *iptr) {
	return __builtin_modf(x, iptr);
}

long double modfl(long double x, long double *iptr) {
	return __builtin_modfl(x, iptr);
}

#else

float modff(float x, float *iptr) {
	if (fabsf(x) >= 8388608.0) {
		*iptr = x;
		return 0.0;
	}
	else if (fabs(x) < 1.0) {
		*iptr = 0.0;
		return x;
	}
	else {
		*iptr = (float) (int) x;
		return (x - *iptr);
	}
}

double modf(double x, double *iptr) {
	if (fabs(x) >= 4503599627370496.0) {
		*iptr = x;
		return 0.0;
	}
	else if (fabs(x) < 1.0) {
		*iptr = 0.0;
		return x;
	}
	else {
		*iptr = (double) (int64_t) x;
		return (x - *iptr);
	}
}

long double modfl(long double x, long double *iptr) {
	if (fabs(x) >= 4503599627370496.0) {
		*iptr = x;
		return 0.0;
	}
	else if (fabs(x) < 1.0) {
		*iptr = 0.0;
		return x;
	}
	else {
		*iptr = (long double) (int64_t) x;
		return (x - *iptr);
	}
}

#endif
