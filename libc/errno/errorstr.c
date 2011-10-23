/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <rho/struct.h>

static const char *__errorstr_table[EUNK] = {
	NULL,					// 0	ENONE
	"! toobig",				// 1	E2BIG
	"! denied",				// 2	EACCES
	"! addrinuse",			// 3	EADDRINUSE
	"! addrnotavail",		// 4	EADDRNOTAVAIL
	"! afnosupport",		// 5	EAFNOSUPPORT
	"! again",				// 6	EAGAIN
	"! already",			// 7	EALREADY
	"! badfd",				// 8	EBADF
	"! badmsg",				// 9	EBADMSG
	"! busy",				// 10	EBUSY
	"! child",				// 11	ECHILD
	"! connaborted",		// 12	ECONNABORTED
	"! connrefused",		// 13	ECONNREFUSED
	"! connreset",			// 14	ECONNRESET
	"! deadlock",			// 15	EDEADLK
	"! destaddrreq",		// 16	EDESTADDRREQ
	"! domain",				// 17	EDOM
	"! dquot",				// 18	EDQUOT
	"! exist",				// 19	EEXIST
	"! fault",				// 20	EFAULT
	"! filetoobig",			// 21	EFBIG
	"! hostunreach",		// 22	EHOSTUNREACH
	"! idremoved",			// 23	EIDRM
	"! ilseq",				// 24	EILSEQ
	"! inprogress",			// 25	EINPROGRESS
	"! interrupted",		// 26	EINTR
	"! arg",				// 27	EINVAL
	"! io",					// 28	EIO
	"! isconn",				// 29	EISCONN
	"! isdir",				// 30	EISDIR
	"! loop",				// 31	ELOOP
	"! mfile",				// 32	EMFILE
	"! mlink",				// 33	EMLINK
	"! msgsize",			// 34	EMSGSIZE
	"! multihop",			// 35	EMULTIHOP
	"! nametoolong",		// 36	ENAMETOOLONG
	"! netdown",			// 37	ENETDOWN
	"! netreset",			// 38	ENETRESET
	"! netunreach",			// 39	ENETUNREACH
	"! nfile",				// 40	ENFILE
	"! buffer",				// 41	ENOBUFS
	"! nodev",				// 42	ENODEV
	"! nfound",				// 43	ENOENT
	"! noexec",				// 44	ENOEXEC
	"! nolock",				// 45	ENOLCK
	"! nolink",				// 46	ENOLINK
	"! nomem",				// 47	ENOMEM
	"! nomsg",				// 48	ENOMSG
	"! noprotoopt",			// 49	ENOPROTOOPT
	"! nospace",			// 50	ENOSPC
	"! nosys",				// 51	ENOSYS
	"! notconn",			// 52	ENOTCONN
	"! notdir",				// 53	ENOTDIR
	"! notempty",			// 54	ENOTEMPTY
	"! notrecoverable",		// 55	ENOTRECOVERABLE
	"! notsock",			// 56	ENOTSOCK
	"! notsupported",		// 57	ENOTSUP
	"! control",			// 58	ENOTTY
	"! address",			// 59	ENXIO
	"! overflow",			// 60	EOVERFLOW
	"! ownerdead",			// 61	EOWNERDEAD
	"! permit",				// 62	EPERM
	"! pipe",				// 63	EPIPE
	"! protocol",			// 64	EPROTO
	"! protonosupport",		// 65	EPROTONOSUPPORT
	"! prototype",			// 66	EPROTOTYPE
	"! range",				// 67	ERANGE
	"! readonly",			// 68	EROFS
	"! seek",				// 69	ESPIPE
	"! process",			// 70	ESRCH
	"! stale",				// 71	ESTALE
	"! timedout",			// 72	ETIMEDOUT
	"! txtbusy",			// 73	ETXTBUSY
	"! type",				// 74	ETYPE
	"! exdev",				// 75	EXDEV
};

static struct s_table *__errorstr_lookup = NULL;

/*****************************************************************************
 * errorstr
 *
 * Returns the string representation of <errnum>, if it exists. If it does
 * not, NULL is returned and errno is set to EINVAL.
 */

char *errorstr(int errnum) {

	if (errnum > 0 && errnum < EUNK) {
		return strdup(__errorstr_table[errnum]);
	}
	else {
		errno = EINVAL;
		return NULL;
	}
}

/*****************************************************************************
 * iserror
 *
 * Returns nonzero iff <error_str> has the format of an error string, i.e.
 * if it begins with "! " or is NULL.
 */

int iserror(const char *error_str) {
	
	if (!error_str) {
		return 1;
	}

	if (error_str[0] == '!' && error_str[1] == ' ') {
		return 1;
	}

	return 0;
}

/*****************************************************************************
 * geterror
 *
 * Return the error number of the error associated with <error_str>. If
 * <error_str> is NULL or has the format of an error, but does not match any
 * known error, EUNK is returned by default. If <error_str> does not have the
 * format of an error, ENONE (zero) is returned.
 */

int geterror(const char *error_str) {
	int errnum;

	if (!__errorstr_lookup) {
		// initialize lookup table
		for (errnum = 1; errnum < EUNK; errnum++) {
			__errorstr_lookup = s_table_set(__errorstr_lookup, 
				__errorstr_table[errnum], (void*) errnum);
		}
	}

	if (!error_str) {
		return EUNK;
	}

	if (!iserror(error_str)) {
		return ENONE;
	}

	errnum = (int) s_table_get(__errorstr_lookup, error_str);
	
	if (!errnum) {
		errnum = EUNK;
	}

	return errnum;
}
