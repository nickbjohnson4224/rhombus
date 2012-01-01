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

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <rho/proc.h>

/****************************************************************************
 * _sigpanic
 *
 * Print an error message and abort.
 */

static void _sigpanic(int signum, const char *message) {

	_stop((uint32_t) -1);

	fprintf(stderr,
		"%s (pid %d \"%s\" terminated)\n", message, getpid(), getname_s());
	
	exit(EXIT_FAILURE | ((signum & 0xFF) << 8) | EXIT_SIGNAL);
}

/****************************************************************************
 * __sig_default
 *
 * Default signal handler. Prints error messages and aborts for certain
 * signals; does nothing for others.
 */

void __sig_default(int signum) {
	
	switch (signum) {
	case SIGQUIT:	_sigpanic(signum, "Termination request");		break;
	case SIGTERM:	_sigpanic(signum, "Termination request");		break;
	case SIGABRT:	_sigpanic(signum, "Abnormal termination");		break;
	case SIGKILL:	_sigpanic(signum, "Process killed");			break;
	case SIGINT:	_sigpanic(signum, "Process interrupted");		break;
	case SIGFPE:	_sigpanic(signum, "Floating point exception");	break;
	case SIGSEGV:	_sigpanic(signum, "Segmentation fault");		break;
	case SIGILL:	_sigpanic(signum, "Illegal operation");			break;
	}
}
