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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <natio.h>
#include <errno.h>
#include <proc.h>
#include <exec.h>
#include <ipc.h>

#include "inc/tar.h"
#include "initrd.h"

const char *splash ="\
Flux Operating System 0.7 Alpha\n\
Written by Nick Johnson\n\
\n";

void panic(const char *message) {
	printf("INIT PANIC: %s\n", message);
	for(;;);
}

static uint64_t start(struct tar_file *file, char const **argv) {
	int32_t pid;

	pid = fork();

	setenv("NAME", "unknown");

	if (pid < 0) {
		execiv(file->start, file->size, argv);
		for(;;);
	}

	setenv("NAME", "init");

	mwaits(PORT_CHILD, pid);

	return RP_CONS(pid, 0);
}

int main() {
	struct tar_file *boot_image, *file;
	char const **argv;
	uint64_t temp, temp1, temp2;

	argv = malloc(sizeof(char*) * 4);

	/* Boot Image */
	boot_image = tar_parse((void*) BOOT_IMAGE);

	/* Dynamic Linker */
	file = tar_find(boot_image, "lib/dl.so");
	dl_load(file->start);

	/* Initial Root Filesystem / Device Filesystem / System Filesystem (tmpfs) */
	argv[0] = "tmpfs";
	argv[1] = NULL;
	file = tar_find(boot_image, "sbin/tmpfs");
	fs_root = start(file, argv);
	io_cons("/dev", FOBJ_DIR);
	io_cons("/sys", FOBJ_DIR);

	/* Logfile */
	io_cons("/dev/stderr", FOBJ_FILE);

	/* Init control file */
	io_link("/sys/init", RP_CONS(getpid(), 1));

	/* Terminal Driver */
	argv[0] = "term";
	argv[1] = NULL;
	file = tar_find(boot_image, "sbin/term");
	temp = start(file, argv);
	io_link("/dev/term", temp);
	io_link("/dev/stdout", temp);

	/* Splash */
	stderr = stdout = fopen("/dev/stdout", "w");
	printf(splash);

	/* Initrd */
	initrd_init();
	io_link("/dev/initrd", RP_CONS(getpid(), 0));
	
	/* Root filesystem (tarfs) */
	argv[0] = "tarfs";
	argv[1] = "/dev/initrd";
	argv[2] = NULL;
	file = tar_find(boot_image, "sbin/tarfs");
	temp = start(file, argv);

	/* Link /dev and /sys and change root */
	temp1 = io_find("/dev");
	temp2 = io_find("/sys");
	fs_root = temp;
	io_link("/dev", temp1);
	io_link("/sys", temp2);

	/* Temporary filesystem */
	argv[0] = "tmpfs";
	argv[1] = NULL;	
	file = tar_find(boot_image, "sbin/tmpfs");
	io_link("/tmp", start(file, argv));

	/* Keyboard Driver */
	argv[0] = "kbd";
	file = tar_find(boot_image, "sbin/kbd");
	io_link("/dev/kbd", start(file, argv));
	io_link("/dev/stdin", io_find("/dev/kbd"));

	/* Stdin */
	stdin = fopen("/dev/stdin", "r");

	/* Time Driver */
	argv[0] = "time";
	argv[1] = NULL;
	file = tar_find(boot_image, "sbin/time");
	io_link("/dev/time", start(file, argv));

	/* Serial Driver */
	argv[0] = "serial";
	argv[1] = NULL;
	file = tar_find(boot_image, "sbin/serial");
	io_link("/dev/serial", start(file, argv));

	/* Path */
	setenv("PATH", "/bin");

	/* Flux Init Shell */
	argv[0] = "fish";
	file = tar_find(boot_image, "bin/fish");
	if (!file) {
		printf("critical error: no init shell found\n");
		for(;;);
	}

	if (fork() < 0) {
//		setcuser(1);
		execiv(file->start, file->size, argv);
	}

	setenv("NAME", "init");
	
	mwaits(PORT_CHILD, 0);

	printf("INIT PANIC: system daemon died\n");
	for(;;);
	return 0;
}
