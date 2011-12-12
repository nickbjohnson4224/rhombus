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

#ifndef __RLIBC_SYS_WAIT_H
#define __RLIBC_SYS_WAIT_H

#include <sys/types.h>
#include <rho/proc.h>

#define WNOHANG 0x0001

#define WIFEXITED(status)   (((status) & (EXIT_SIGNAL | EXIT_STOP | EXIT_ABORT)) == 0)
#define WEXITSTATUS(status) ((status) & 0xFF)
#define WIFSIGNALED(status) ((status) & EXIT_SIGNAL)
#define WTERMSIG(status)    (((status) >> 8) & 0xFF)
#define WCOREDUMP(status)   ((status) & EXIT_DUMPED)
#define WIFSTOPPED(status)  ((status) & EXIT_STOP)
#define WSTOPSIG(status)    (((status) >> 8) & 0xFF)

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

#endif/*__RLIBC_SYS_WAIT_H*/
