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

#include <math.h>
#include <float.h>

float atan2f(float y, float x) {
	
	if (y == 0.0) {
		if (x >= 0.0) {
			return 0.0;
		}
		else {
			return M_PI;
		}
	}
	else if (y > 0.0) {
		if (x == 0.0) {
			return M_PI_2;
		}
		else if (x > 0.0) {
			return tanf(y / x);
		}
		else {
			return M_PI - tanf(y / x);
		}
	}
	else {
		if (x == 0.0) {
			return M_PI + M_PI_2;
		}
		else if (x > 0.0) {
			return 2 * M_PI - tanf(y / x);
		}
		else {
			return M_PI + tanf(y / x);
		}
	}
}

double atan2(double y, double x) {
	
	if (y == 0.0) {
		if (x >= 0.0) {
			return 0.0;
		}
		else {
			return M_PI;
		}
	}
	else if (y > 0.0) {
		if (x == 0.0) {
			return M_PI_2;
		}
		else if (x > 0.0) {
			return tan(y / x);
		}
		else {
			return M_PI - tan(y / x);
		}
	}
	else {
		if (x == 0.0) {
			return M_PI + M_PI_2;
		}
		else if (x > 0.0) {
			return 2 * M_PI - tan(y / x);
		}
		else {
			return M_PI + tan(y / x);
		}
	}
}

long double atan2l(long double y, long double x) {
	
	if (y == 0.0) {
		if (x >= 0.0) {
			return 0.0;
		}
		else {
			return M_PI;
		}
	}
	else if (y > 0.0) {
		if (x == 0.0) {
			return M_PI_2;
		}
		else if (x > 0.0) {
			return tanl(y / x);
		}
		else {
			return M_PI - tanl(y / x);
		}
	}
	else {
		if (x == 0.0) {
			return M_PI + M_PI_2;
		}
		else if (x > 0.0) {
			return 2 * M_PI - tanl(y / x);
		}
		else {
			return M_PI + tanl(y / x);
		}
	}
}
