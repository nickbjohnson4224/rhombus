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

#ifndef __RLIBC_ERRNO_H
#define __RLIBC_ERRNO_H

#include <proc.h>

/* errno *******************************************************************/

extern int errnov[MAX_THREADS];

#define errno (errnov[gettid()])

/* error codes *************************************************************/

#define E2BIG	1
#define EACCES	2
#define EAGAIN	3
#define EBADMSG	4
#define EBUSY	5
#define ECHILD	6
#define EDOM	7
#define EEXIST	8
#define EILSEQ	9
#define EINVAL	10
#define EIO		11
#define EISDIR	12
#define EPATH	13
#define ENOENT	14
#define ENOEXEC	15
#define ENOMEM	16
#define ENOSPC	17
#define ENOSYS	18
#define ENOTDIR	19
#define EPERM	20
#define ERANGE	21
#define EUNK	22
#define ENOTEMPTY 23
#define ETIMEDOUT 24

#endif/*__RLIBC_ERRNO_H*/
