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

#include <string.h>
#include <errno.h>

/****************************************************************************
 * strerror
 *
 * Returns a string corresponding to the error code <errnum>.
 */

const char *strerror(int errnum) {

	switch (errnum) {
	case ENONE:				return "no error";
	case E2BIG:				return "argument list too long";
	case EACCES:			return "permission denied";
	case EADDRINUSE:		return "address in use";
	case EADDRNOTAVAIL:		return "address not available";
	case EAFNOSUPPORT:		return "address family not supported";
	case EAGAIN:			return "resource unavailable";
	case EALREADY:			return "connection already in progress";
	case EBADF:				return "bad file descriptor";
	case EBADMSG:			return "bad message";
	case EBUSY:				return "resource busy";
	case ECHILD:			return "no child processes";
	case ECONNABORTED:		return "connection aborted";
	case ECONNREFUSED:		return "connection refused";
	case ECONNRESET:		return "connection reset";
	case EDEADLK:			return "deadlock";
	case EDESTADDRREQ:		return "destination address required";
	case EDOM: 				return "argument out of domain";
	case EDQUOT:			return "dquot?";
	case EEXIST:			return "file exists";
	case EFAULT:			return "bad address";
	case EFBIG:				return "file too large";
	case EHOSTUNREACH:		return "host unreachable";
	case EIDRM:				return "identifier removed";
	case EILSEQ:			return "illegal byte sequence";
	case EINVAL:			return "invalid argument";
	case EIO:				return "I/O error";
	case EISCONN:			return "socket is connected";
	case EISDIR:			return "is a directory";
	case ELOOP:				return "too many levels of symbolic links";
	case EMFILE:			return "file descriptor value too large";
	case EMLINK:			return "too many links";
	case EMSGSIZE:			return "message too large";
	case EMULTIHOP:			return "multihop?";
	case ENAMETOOLONG:		return "filename too long";
	case ENETDOWN:			return "network is down";
	case ENETRESET:			return "connection aborted by network";
	case ENETUNREACH:		return "network unreachable";
	case ENFILE:			return "too many files open in system";
	case ENOBUFS:			return "no buffer space available";
	case ENODEV:			return "no such device";
	case ENOENT:			return "no such file or directory";
	case ENOEXEC:			return "invalid executable";
	case ENOMEM:			return "out of memory";
	case ENOMSG:			return "no message of the desired type";
	case ENOPROTOOPT:		return "protocol not available";
	case ENOSPC:			return "no space left on device";
	case ENOSYS:			return "function not supported";
	case ENOTCONN:			return "socket not connected";
	case ENOTDIR:			return "not a directory";
	case ENOTEMPTY:			return "directory not empty";
	case ENOTRECOVERABLE:	return "state not recoverable";
	case ENOTSOCK:			return "not a socket";
	case ENOTSUP:			return "not supported";
	case ENOTTY:			return "inappropriate I/O control operation";
	case ENXIO:				return "no such device or address";
	case EOVERFLOW:			return "value too large to be stored in data type";
	case EOWNERDEAD:		return "previous owner died";
	case EPERM:				return "operation not permitted";
	case EPIPE:				return "broken pipe";
	case EPROTO:			return "protocol error";
	case EPROTONOSUPPORT:	return "protocol not supported";
	case EPROTOTYPE:		return "protocol wrong type for socket";
	case ERANGE:			return "result out of range";
	case EROFS:				return "read-only filesystem";
	case ESPIPE:			return "invalid seek";
	case ESRCH:				return "no such process";
	case ESTALE:			return "stale?";
	case ETIMEDOUT:			return "connection timed out";
	case ETXTBUSY:			return "text file busy";
	case EXDEV:				return "cross-device link";
	case EUNK:				return "unknown error";
	default:
		errno = EINVAL;
		return NULL;
	}
}
