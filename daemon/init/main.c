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

const char *splash_art = "\n\
    \xb2\xb2\xb2\xb2\n\
  \xb2\xb2\xb2\xb2     Flux OS 0.6 Alpha\n\
    \xb2\xb2\n\
\n";

const char *splash ="\
Flux Operating System 0.6 Alpha\n\
Written by Nick Johnson\n\
\n";

void panic(const char *message) {
	printf("INIT PANIC: %s\n", message);
	for(;;);
}

uint64_t daemon_start(void *image, size_t image_size, char const **argv) {
	int32_t pid;

	pid = fork();

	if (pid < 0) {
		execiv(image, image_size, argv);
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
	boot_image = tar_parse((uint8_t*) BOOT_IMAGE);

	/* Initial Root Filesystem / Device Filesystem / System Filesystem (tmpfs) */
	argv[0] = "tmpfs";
	argv[1] = NULL;
	if (!(file = tar_find(boot_image, (char*) "sbin/tmpfs"))) panic("no tmpfs found");
	fs_root = daemon_start(file->start, file->size, argv);
	fs_cons(fs_find(0, "/"), "dev", FOBJ_DIR);
	fs_cons(fs_find(0, "/"), "sys", FOBJ_DIR);

	/* Init control file */
	fs_link(fs_cons(fs_find(0, "/sys"), "init", FOBJ_DIR), ((uint64_t) getpid() << 32) + 1);

	/* Terminal Driver */
	argv[0] = "term";
	argv[1] = NULL;
	if (!(file = tar_find(boot_image, (char*) "sbin/term"))) panic("no term found");
	temp = daemon_start(file->start, file->size, argv); 
	stderr = stdout = fdopen(temp, "w");
	printf(splash);
	fs_link(fs_cons(fs_find(0, "/dev"), "term", FOBJ_DIR), temp);

	/* Initrd */
	driver_init(&initrd_driver, 0, NULL);
	setenv("NAME", "unknown");
	temp = ((uint64_t) getpid() << 32) + 0;
	fs_link(fs_cons(fs_find(0, "/dev"), "initrd", FOBJ_DIR), temp);

	/* Root filesystem (tarfs) */
	argv[0] = "tarfs";
	argv[1] = "/dev/initrd";
	argv[2] = NULL;
	if (!(file = tar_find(boot_image, (char*) "sbin/tarfs"))) panic("no tarfs found");
	temp = daemon_start(file->start, file->size, argv);
	temp1 = fs_find(0, "/dev");
	temp2 = fs_find(0, "/sys");
	fs_root = temp;
	fs_link(fs_find(0, "/dev"), temp1);
	fs_link(fs_find(0, "/sys"), temp2);

	/* Shared object daemon (sod) */
	argv[0] = "sod";
	argv[1] = "/lib";
	argv[2] = NULL;
	if (!(file = tar_find(boot_image, (char*) "sbin/sod"))) panic("no sod found");
	temp = daemon_start(file->start, file->size, argv);
	fs_link(fs_cons(fs_find(0, "/sys"), "lib", FOBJ_DIR), temp);

	/* Temporary filesystem */
	argv[0] = "tmpfs";
	argv[1] = NULL;	
	if (!(file = tar_find(boot_image, (char*) "sbin/tmpfs"))) panic("no tmpfs found");
	temp = daemon_start(file->start, file->size, argv);
	fs_link(fs_find(0, "/tmp"), temp);

	/* Keyboard Driver */
	argv[0] = "kbd";
	if (!(file = tar_find(boot_image, (char*) "sbin/kbd"))) panic("no kbd found");
	temp = daemon_start(file->start, file->size, argv);
	fs_link(fs_cons(fs_find(0, "/dev"), "kbd", FOBJ_DIR), temp);
	stdin = fdopen(temp, "r");

	/* Time Driver */
	argv[0] = "time";
	if (!(file = tar_find(boot_image, (char*) "sbin/time"))) panic("no time found");
	temp = daemon_start(file->start, file->size, argv);
	fs_link(fs_cons(fs_find(0, "/dev"), "time", FOBJ_DIR), temp);

	/* Path */
	setenv("PATH", "/bin");

	/* Flux Init Shell */
	argv[0] = "fish";
	file = tar_find(boot_image, (char*) "bin/fish");
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
