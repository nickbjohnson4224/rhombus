/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
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
#include <proc.h>
#include <exec.h>
#include <ipc.h>

#include "inc/tar.h"
#include "initrd.h"

const char *splash ="\
Flux Operating System 0.6 Alpha\n\
Written by Nick Johnson\n\
\n";

void panic(const char *message) {
	printf("INIT PANIC: %s\n", message);
	for(;;);
}

static uint64_t start(struct tar_file *file, char const **argv) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		execiv(file->start, file->size, argv);
		for(;;);
	}

	mwaits(PORT_CHILD, pid);

	return ((uint64_t) pid << 32);
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
	fs_cons(fs_find(0, "/"), "dev", FOBJ_DIR);
	fs_cons(fs_find(0, "/"), "sys", FOBJ_DIR);

	/* Init control file */
	fs_bind((uint64_t) getpid() << 32 + 1, "/sys/init");

	/* Terminal Driver */
	argv[0] = "term";
	argv[1] = NULL;
	file = tar_find(boot_image, "sbin/term");
	temp = start(file, argv);
	fs_bind(temp, "/dev/term");

	/* Splash */
	stderr = stdout = fdopen(temp, "w");
	printf(splash);

	/* Initrd */
	driver_init(&initrd_driver, 0, NULL);
	setenv("NAME", "unknown");
	fs_bind((uint64_t) getpid() << 32, "/dev/initrd");

	/* Root filesystem (tarfs) */
	argv[0] = "tarfs";
	argv[1] = "/dev/initrd";
	argv[2] = NULL;
	file = tar_find(boot_image, "sbin/tarfs");
	temp = start(file, argv);

	/* Bind /dev and /sys and change root */
	temp1 = fs_find(0, "/dev");
	temp2 = fs_find(0, "/sys");
	fs_root = temp;
	fs_link(fs_find(0, "/dev"), temp1);
	fs_link(fs_find(0, "/sys"), temp2);

	/* Shared object daemon (sod) */
	argv[0] = "sod";
	argv[1] = "/lib";
	argv[2] = NULL;
	file = tar_find(boot_image, "sbin/sod");
	fs_bind(start(file, argv), "/sys/lib");
	setenv("LDPATH", "/sys/lib");

	/* Temporary filesystem */
	argv[0] = "tmpfs";
	argv[1] = NULL;	
	file = tar_find(boot_image, "sbin/tmpfs");
	fs_bind(start(file, argv), "/tmp");

	/* Keyboard Driver */
	argv[0] = "kbd";
	file = tar_find(boot_image, "sbin/kbd");
	temp = start(file, argv);
	fs_bind(temp, "/dev/kbd");
	stdin = fdopen(temp, "r");

	/* Time Driver */
	argv[0] = "time";
	file = tar_find(boot_image, "sbin/time");
	fs_bind(start(file, argv), "/dev/time");

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
		setcuser(1);
		execiv(file->start, file->size, argv);
	}

	setenv("NAME", "init");
	
	mwaits(PORT_CHILD, 0);

	printf("INIT PANIC: system daemon died\n");
	for(;;);
	return 0;
}
