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

#include <rhombus.h>
#include <rho/natio.h>

int pipe(int pipefd[2]) {
	rp_t rp;
	rp_t pipe_driver;

	pipe_driver = fs_find("/sys/pipe");

	if (!pipe_driver) {
		return -1;
	}

	rp = rp_cons(pipe_driver, "file");

	if (!rp) {
		return -1;
	}

	pipefd[0] = ropen(-1, rp, STAT_READER);
	pipefd[1] = ropen(-1, rp, STAT_WRITER);

	return 0;
}
