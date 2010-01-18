/* Copyright 2010 Nick Johnson */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mmap.h>
#include <flux.h>
#include <driver.h>
#include <signal.h>

#include <driver/terminal.h>
#include <driver/keyboard.h>

static uint32_t console_pid;
static volatile struct request *reply = NULL;

void console_hand(uint32_t caller, void *grant) {
	if (caller != console_pid) return;
	if (grant) reply = req_catch(grant);
}

void console_init() {
	int32_t pid = fork();

	sigregister(SIG_REPLY, console_hand);

	if (pid < 0) {
		terminal.init(0);
		keyboard.init(0);
		fire(-pid, SIG_REPLY, NULL);
		for(;;);
	}
	else {
		console_pid = pid;
		wait(SIG_REPLY);
	}
}

size_t console_write(char *buffer, size_t size) {
	struct request *r = req_alloc();

	r->datasize = size;
	r->dataoff 	= 0;
	r->format 	= REQ_WRITE;

	memcpy(r->reqdata, buffer, size);

	r = req_checksum(r);

	reply = NULL;
	fire(console_pid, SIG_WRITE, r);
	req_free(r);
	while (!reply);
	size = reply->datasize;
	req_free((void*) reply);

	return size;
}
