/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <flux.h>
#include <stdio.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

void console_init() {
	int32_t tpid, kpid;
	device_t dev;

	sighold(SIG_REPLY);

	tpid = fork();
	if (tpid < 0) {
		sigfree(SIG_REPLY);
		terminal.init(dev);
		fire(-tpid, SIG_REPLY, NULL);
		block(true);
		for(;;);
	}	
	sigpull(SIG_REPLY);
	stdout = fsetup(tpid, 0, "a");

	kpid = fork();
	if (kpid < 0) {
		sigfree(SIG_REPLY);
		keyboard.init(dev);
		fire(-kpid, SIG_REPLY, NULL);
		block(true);
		for(;;);
	}
	sigpull(SIG_REPLY);

	stdin = fsetup(kpid, 0, "r");

	sigfree(SIG_REPLY);
}

size_t console_write(char *buffer, size_t size) {
	return fwrite(buffer, sizeof(char), size, stdout);
}

size_t console_read(char *buffer, size_t size) {
	return fread(buffer, sizeof(char), size, stdin);
}
