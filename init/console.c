/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>
#include <flux.h>
#include <driver.h>
#include <signal.h>
#include <stdio.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

void console_init() {
	int32_t pid;

	sighold(SIG_REPLY);

	pid = fork();
	if (pid < 0) {
		terminal.init(0);
		fire(-pid, SIG_REPLY, NULL);
		block();
		for(;;);
	}

	stdout = fsetup(pid, 0, "a");
	sigpull(SIG_REPLY);

	pid = fork();
	if (pid < 0) {
		keyboard.init(0);
		fire(-pid, SIG_REPLY, NULL);
		block();
		for(;;);
	}

	stdin = fsetup(pid, 0, "r");
	sigpull(SIG_REPLY);

	sigfree(SIG_REPLY);
}

size_t console_write(char *buffer, size_t size) {
	return fwrite(buffer, sizeof(char), size, stdout);
}

size_t console_read(char *buffer, size_t size) {
	return fread(buffer, sizeof(char), size, stdin);
}
