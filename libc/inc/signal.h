/*
 * Copyright (C) 2009-2012 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#define SIGQUIT ACTION_QUIT
#define SIGTERM	ACTION_TERM
#define SIGABRT ACTION_ABORT
#define SIGKILL ACTION_KILL
#define SIGSTOP ACTION_STOP
#define SIGCONT ACTION_CONT
#define SIGTRAP ACTION_TRAP
#define SIGINT  ACTION_INT
#define SIGIRQ	ACTION_IRQ
#define SIGALRM ACTION_ALARM
#define SIGCHLD ACTION_CHILD
#define SIGFPE  ACTION_FLOAT
#define SIGSEGV ACTION_PAGE
#define SIGILL  ACTION_ILL
#define SIGUSR1 ACTION_USER1
#define SIGUSR2 ACTION_USER2
#define SIGNUKE 0xFF

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
