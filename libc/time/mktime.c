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

#include <time.h>

time_t mktime(struct tm *ptr) {
	time_t timer;

	timer = 0;
	timer += ptr->tm_sec;
	timer += ptr->tm_min * 60;
	timer += ptr->tm_hour * 3600;
	timer += ptr->tm_yday * 86400;
	timer += (ptr->tm_year - 70) * 31536000;
	timer += ((ptr->tm_year - 69) / 4) * 86400;
	timer -= ((ptr->tm_year - 1) / 100) * 86400;
	timer += ((ptr->tm_year + 299) / 400) * 86400;

	return timer;
}
