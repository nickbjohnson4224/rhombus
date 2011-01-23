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

#include <string.h>
#include <errno.h>

/****************************************************************************
 * strerror
 *
 * Returns a string corresponding to the error code <errnum>.
 */

const char *strerror(int errnum) {
	switch (errnum) {
	case E2BIG:		return "argument list too long";
	case EACCES:	return "access denied";
	case EAGAIN:	return "resource unavailable";
	case EBADMSG:	return "bad message";
	case EBUSY:		return "resource busy";
	case ECHILD:	return "no child processes";
	case EDOM: 		return "argument out of domain";
	case EEXIST:	return "file exists";
	case EILSEQ:	return "illegal byte sequence";
	case EINVAL:	return "invalid argument";
	case EIO:		return "I/O error";
	case EISDIR:	return "is a directory";
	case EPATH:		return "invalid path";
	case ENOENT:	return "no such file or directory";
	case ENOEXEC:	return "invalid executable";
	case ENOMEM:	return "out of memory";
	case ENOSPC:	return "no space left on device";
	case ENOSYS:	return "function not supported";
	case ENOTDIR:	return "is not a directory";
	case EPERM:		return "operation not permitted";
	case ERANGE:	return "result out of range";
	case ENOTEMPTY:	return "directory not empty";
	case EUNK:		
	default:		return "unknown error";
	}
}
