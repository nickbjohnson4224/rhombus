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

#ifndef __RLIBC_UNISTD_H
#define __RLIBC_UNISTD_H

#include <rhombus.h>
#include <rho/exec.h>
#include <rho/proc.h>
#include <rho/natio.h>

#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#define _POSIX_VERSION  200112L
#define _POSIX2_VERSION 200112L

unsigned int alarm(unsigned int seconds);
int chdir(const char *path);
size_t confstr(int, char *, size_t);

/* File access **************************************************************/

int access(const char *pathname, int mode); 				// TODO

int chown (const char *path, uid_t owner, gid_t group); 	// TODO
int fchown(int, uid_t owner, gid_t group); 					// TODO
int lchown(const char *path, uid_t owner, gid_t group); 	// TODO

/* File descriptor operations ***********************************************/

// Implemented in libc/rhombus (see <rhombus.h>)
int close (int fd);
int dup   (int fd);
int dup2  (int fd, int newfd);
int pipe  (int pipefd[2]);

int isatty(int); 											// TODO

/* Links ********************************************************************/

int link   (const char *oldpath, const char *newpath); 		// TODO
int symlink(const char *oldpath, const char *newpath); 		// TODO
int rmdir  (const char *pathname); 							// TODO
int unlink (const char *pathname); 							// TODO

ssize_t readlink(const char * restrict, char * restrict, size_t); // TODO

/* I/O **********************************************************************/

off_t   lseek (int fd, off_t offset, int whence);
ssize_t read  (int fd, void *buf, size_t count);
ssize_t pread (int fd, void *buf, size_t count, off_t offset);
ssize_t write (int fd, const void *buf, size_t count);
ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);

/* Execution ****************************************************************/

int execl (const char *path, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execlp(const char *path, const char *arg, ...);
int execv (const char *path, char * const argv[]);
int execve(const char *path, char * const argv[], char * const envp[]);
int execvp(const char *path, char * const argv[]);

/* Process control **********************************************************/

void _exit(int);
pid_t fork(void); // Implemented in libc/proc (see <rho/proc.h>)

/* PIDs, UIDs, and GIDs *****************************************************/

gid_t getgroups(int size, gid_t *list);
gid_t getgid (void);
int   setgid (gid_t gid);
gid_t getegid(void);
int   setegid(gid_t gid);

uid_t getuid (void);
int   setuid (uid_t);
uid_t geteuid(void);
int   seteuid(uid_t);

pid_t getpgrp(void);
int   setpgrp(void);
int   setpgid(pid_t pid, pid_t pgid);
pid_t getpid (void);
pid_t getppid(void);
pid_t setsid (void);

/* Unsorted *****************************************************************/

long int fpathconf(int, int);
int ftruncate(int, off_t);
char *getcwd(char *, size_t);
int gethostname(char *, size_t);
char *getlogin(void);
int getlogin_r(char *, size_t);
int getopt(int, char * const [], const char *);

long pathconf(const char *, int);
int pause(void);

unsigned sleep(unsigned);
long int sysconf(int);
pid_t tcgetpgrp(int);
char *ttyname(int);
int ttyname_r(int, char *, size_t);

#endif/*__RLIBC_UNISTD_H*/
