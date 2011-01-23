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

#include <time.h>
#include <stdbool.h>

static bool leapyear(int year) {
	year -= 100;
	return ((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0));
}

static int month_limit[] = {
	32,
	60,
	91,
	121,
	152,
	182,
	213,
	244,
	274,
	305,
	335,
	366
};

static int month_limit_leap[] = {
	32,
	61,
	92,
	122,
	153,
	183,
	214,
	245,
	275,
	306,
	336,
	367
};

struct tm *gmtime_r(const time_t *timer, struct tm *result) {
	int day, i;
	time_t timer0 = *timer;
	
	/* GMT => no offset */
	result->tm_offset = 0;

	/* no daylight savings time */
	result->tm_isdst = 0;

	/* get seconds */
	result->tm_sec = timer0 % 60;
	timer0 -= result->tm_sec;

	/* get minutes */
	result->tm_min = (timer0 % 3600) / 60;
	timer0 -= result->tm_min * 60;

	/* get hours */
	result->tm_hour = (timer0 % 86400) / 3600;
	timer0 -= result->tm_hour * 3600;

	/* get days since Jan 1, 1900 */
	day = (*timer / 86400) + 25567;

	/* get weekday */
	result->tm_wday = 4 + (day % 7);

	/* get years since 1900 */
	result->tm_year = 0;
	while (day > (leapyear(result->tm_year) ? 366 : 365)) {
		day -= ((leapyear(result->tm_year)) ? 366 : 365);
		result->tm_year++;
	}

	/* get day of year */
	result->tm_yday = day;

	/* get day of month */
	result->tm_mday = day;
	if (leapyear(result->tm_year)) {
		for (i = 0; result->tm_mday < month_limit_leap[i]; i++);
		result->tm_mday -= (i) ? month_limit_leap[i - 1] : 0;
		result->tm_mon = i;
	}
	else {
		for (i = 0; result->tm_mday < month_limit[i]; i++);
		result->tm_mday -= (i) ? month_limit[i - 1] : 0;
		result->tm_mon = i;
	}

	return result;
}
