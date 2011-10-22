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

#ifndef __RLIBC_SIGNAL_H
#define __RLIBC_SIGNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <rho/ipc.h>

/* signal numbers */

#define SIGQUIT PORT_QUIT
#define SIGTERM	PORT_TERM
#define SIGABRT PORT_ABORT
#define SIGKILL PORT_KILL
#define SIGSTOP PORT_STOP
#define SIGCONT PORT_CONT
#define SIGTRAP PORT_TRAP
#define SIGINT  PORT_INT
#define SIGIRQ	PORT_IRQ
#define SIGALRM PORT_ALARM
#define SIGCHLD PORT_CHILD
#define SIGFPE  PORT_FLOAT
#define SIGSEGV PORT_PAGE
#define SIGILL  PORT_ILL
#define SIGUSR1 PORT_USER1
#define SIGUSR2 PORT_USER2

#define SIGMAX  16

/* signal handling */

typedef void (*sighandler_t)(int);

extern sighandler_t __sighandlerv[SIGMAX];
extern bool __sigmutex;

sighandler_t signal(int signum, sighandler_t handler);

void __sig_init(void);

void __sig_default(int signum);
void __sig_ignore (int signum);

#define SIG_DFL __sig_default
#define SIG_IGN __sig_ignore

#define SIG_ERR NULL

/* signal sending */

int raise(int signum);
int __raise(int caller, int signum);

int kill(int pid, int signum);

#endif/*__RLIBC_SIGNAL_H*/
