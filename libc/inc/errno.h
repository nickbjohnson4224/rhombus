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

#ifndef __RLIBC_ERRNO_H
#define __RLIBC_ERRNO_H

#include <rho/proc.h>

/* errno *******************************************************************/

extern int errnov[MAX_THREADS];

#define errno (errnov[gettid()])

/* error codes *************************************************************/

#define ENONE			0		// no error
#define E2BIG			1		// argument list too long
#define EACCES			2		// permission denied
#define EADDRINUSE		3		// address in use
#define EADDRNOTAVAIL	4		// address not available
#define EAFNOSUPPORT	5		// address family not supported
#define EAGAIN			6		// resource unavailable, try again
#define EALREADY		7		// connection already in progress
#define EBADF			8		// bad file descriptor
#define EBADMSG			9		// bad message
#define EBUSY			10		// device or resource busy
#define ECHILD			11		// no child processes
#define ECONNABORTED	12		// connection aborted
#define ECONNREFUSED	13		// connection refused
#define ECONNRESET		14		// connection reset
#define EDEADLK			15		// deadlock would occur
#define EDESTADDRREQ	16		// destination address required
#define EDOM			17		// mathematics argument out of domain of function
#define EDQUOT			18		// reserved
#define EEXIST			19		// file exists
#define EFAULT			20		// bad address
#define EFBIG			21		// file too large
#define EHOSTUNREACH	22		// host unreachable
#define EIDRM			23		// identifier removed
#define EILSEQ			24		// illegal byte sequence
#define EINPROGRESS		25		// operation in progress
#define EINTR			26		// interrupted function
#define EINVAL			27		// invalid argument
#define EIO				28		// I/O error
#define EISCONN			29		// socket is connected
#define EISDIR			30		// is a directory
#define ELOOP			31		// too many levels of symbolic links
#define EMFILE			32		// file descriptor value too large
#define EMLINK			33		// too many links
#define EMSGSIZE		34		// message too large
#define EMULTIHOP		35		// reserved
#define ENAMETOOLONG	36		// filename too long
#define ENETDOWN		37		// network is down
#define ENETRESET		38		// connection aborted by network
#define ENETUNREACH		39		// network unreachable
#define ENFILE			40		// too many files open in system
#define ENOBUFS			41		// no buffer space available
#define ENODEV			42		// no such device
#define ENOENT			43		// no such file or directory
#define ENOEXEC			44		// executable file format error
#define ENOLCK			45		// no locks available
#define ENOLINK			46		// reserved
#define ENOMEM			47		// not enough space
#define ENOMSG			48		// no message of the desired type
#define ENOPROTOOPT		49		// protocol not available
#define ENOSPC			50		// no space left on device
#define ENOSYS			51		// function not supported
#define ENOTCONN		52		// the socket is not connected
#define ENOTDIR			53		// not a directory
#define ENOTEMPTY		54		// directory not empty
#define ENOTRECOVERABLE	55		// state not recoverable
#define ENOTSOCK		56		// not a socket
#define ENOTSUP			57		// not supported
#define ENOTTY			58		// inappropriate I/O control operation
#define ENXIO			59		// no such device or address
#define EOPNOTSUPP		ENOTSUP
#define EOVERFLOW		60		// value too large to be stored in data type
#define EOWNERDEAD		61		// previous owner died
#define EPERM			62		// operation not permitted
#define EPIPE			63		// broken pipe
#define EPROTO			64		// protocol error
#define EPROTONOSUPPORT	65		// protocol not supported
#define EPROTOTYPE		66		// protocol wrong type for socket
#define ERANGE			67		// result too large
#define EROFS			68		// read-only filesystem
#define ESPIPE			69		// invalid seek
#define ESRCH			70		// no such process
#define ESTALE			71		// reserved
#define ETIMEDOUT		72		// connection timed out
#define ETXTBUSY		73		// text file busy
#define ETYPE			74		// incorrect type
#define EWOULDBLOCK		EAGAIN
#define EXDEV			75		// cross-device link
#define EUNK			76		// unknown error

/*****************************************************************************
 * error code string representations
 *
 * Every error code defined in this header maps to an unique string 
 * representation for that error, which starts with the string "! ". This is
 * used to return error codes from rcall, but can also be used to pass error
 * codes around the system in other ways.
 *
 * errorstr() returns the string representation of <errnum>, if it exists,
 * and NULL otherwise.
 *
 * iserror() returns nonzero iff <error_str> has the format of an error string 
 * (i.e. if it begins with "! " or is NULL.)
 *
 * geterror() returns the number of the error associated with <error_str>. If
 * <error_str> is NULL or has the format of an error, but does not match any 
 * known error, EUNK is returned by default. If <error_str> does not have 
 * the format of an error, ENONE (zero) is returned.
 */

char *errorstr(int errnum);

int iserror (const char *error_str);
int geterror(const char *error_str);

#endif/*__RLIBC_ERRNO_H*/
