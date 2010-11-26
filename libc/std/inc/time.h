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

#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <stddef.h>

/* clock ticks since process start *****************************************/

typedef uint32_t clock_t;

#define CLOCKS_PER_SEC 1000000

#define CLK_PER_SEC CLOCKS_PER_SEC
#define CLK_TCK CLOCKS_PER_SEC

clock_t clock(void);

/* current time in seconds past UNIX epoch *********************************/

typedef int time_t;

time_t time(time_t *timer);
double difftime(time_t timer2, time_t timer1);

/* calendar time ***********************************************************/

struct tm {
	int tm_offset;
	int tm_isdst;

	int tm_year;
	int tm_mon;

	int tm_yday;
	int tm_mday;
	int tm_wday;

	int tm_hour;
	int tm_min;
	int tm_sec;
};

extern struct tm __tm_internal;

struct tm *gmtime   (const time_t *timer);
struct tm *gmtime_r (const time_t *timer, struct tm *result);
struct tm *localtime(const time_t *timer);

time_t mktime(struct tm *ptr);

/* conversion **************************************************************/

char *asctime(const struct tm *tmptr);
char *ctime  (const time_t *timer);

size_t strftime(char *s, size_t n, const char *format, const struct tm *ptr);
char  *strptime(const char *s, const char *format, struct tm *ptr);

#endif/*TIME_H*/
