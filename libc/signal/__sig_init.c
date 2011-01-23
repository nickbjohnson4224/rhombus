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

#include <stdlib.h>
#include <signal.h>
#include <mutex.h>
#include <proc.h>
#include <ipc.h>

/****************************************************************************
 * _sigwrap
 *
 * Wrapper for catching signals and redirecting them to __raise.
 */

static void _sigwrap(struct msg *msg) {
	
	if (msg->packet) {
		free(msg->packet);
	}

	__raise(msg->source, msg->port);

	free(msg);
}

/****************************************************************************
 * __sig_init
 *
 * Initializes signal subsystem: sets all signals handlers to SIG_DFL, and
 * registers signal wrappers for all events.
 */

void __sig_init(void) {
	size_t i;
	
	mutex_spin(&__sigmutex);

	for (i = 0; i < SIGMAX; i++) {
		__sighandlerv[i] = SIG_DFL;
		when(i, _sigwrap);
	}

	mutex_free(&__sigmutex);
}
