/* Copyright 2009 Nick Johnson */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <mmap.h>
#include <flux.h>

#include <driver.h>
#include <config.h>

void swrite(const char *message) {
	extern size_t console_write(char*, size_t);
	console_write((char*) message, strlen(message));
}

static void print_bootsplash() {
	swrite("Welcome to Flux 0.2a\n");
	swrite("Written by Nick Johnson\n\n");
}

void xwrite(uint32_t addr) {
	char m[9];
	const char *d = "0123456789ABCDEF";
	size_t i;

	for (i = 7; (int) i >= 0; i--) {
		m[i] = d[addr & 0xF];
		addr >>= 4;
	}

	m[8] = '\0';
	swrite((const char*) m);
}

void segfault(uint32_t source, void *grant) {
	swrite("\nSegmentation Fault\n");

	exit(1);
}

void writehand(uint32_t source, void *grant) {
	void *addr = malloc(0x1000);
	emap(addr, (uint32_t) grant, PROT_READ | PROT_WRITE);
	swrite(addr);
	free(addr);
}

int main() {
	extern void console_init(void);
	extern size_t console_read(char*, size_t);
	extern size_t console_write(char*, size_t);
	size_t i, j;
	int32_t pid;
	static uint32_t *mb[1024];
	char buffer[10];
	size_t bufsize;

	sigregister(SSIG_FAULT, segfault);
	sigregister(SSIG_PAGE, segfault);
	sigregister(SIG_WRITE, writehand);

	console_init();
	print_bootsplash();

	swrite("Fork test:\n");
	swrite("\tspawning 16 tasks:\t");
	for (i = 0; i < 16; i++) {
		pid = fork();
		if (pid < 0) {
			wait(SSIG_KILL);
			exit(0);
		}
		else {
			mb[i] = (void*) pid;
			swrite(".");
			if (i % 2 == 0) {
				mb[i] = NULL;
				fire(pid, SSIG_KILL, NULL);
				wait(SSIG_DEATH);
			}
		}
	}
	swrite("Done.\n");

	swrite("\tkilling 16 tasks:\t");
	for (i = 0; i < 16; i++) {
		if (mb[i]) {
			fire((uint32_t) mb[i], SSIG_KILL, NULL);
			swrite("..");
		}
	}
	swrite("Done.\n");

	swrite("\n");

	swrite("Signals test:\n");
	wreset(SIG_PING);
	sigblock();
	if (fire(info(0), SIG_PING, NULL)) {
		swrite("\tblocked\n");
		sigunblock();
		fire(info(0), SIG_PING, NULL);
	}
	wait(SIG_PING);
	swrite("\tunblocked\n");
	mb[0] = malloc(0x1000);
	strcpy((char*) mb[0], "\tgrants\n");
	fire(info(0), SIG_WRITE, mb[0]);

	swrite("\n");

	swrite("Allocator test:\n");
	for (i = 0; i < 1; i++) {
		swrite("\tAllocing 64 Blocks\t");
		for (j = 0; j < 64; j++) {
			mb[j] = malloc(0x1000);
			if (j % 4 == 0) swrite(".");
		}
		swrite("Done. \n");

		swrite("\tTesting 64 Blocks\t");
		for (j = 0; j < 64; j++) {
			mb[j][1023] = 0;
			if (j % 4 == 0) swrite(".");
		}
		swrite("Done. \n");

		swrite("\tFreeing 64 Blocks\t");
		for (j = 0; j < 64; j++) {
			free(mb[j]);
			if (j % 4 == 0) swrite(".");
		}
		swrite("Done. \n");
	}
	
	for (i = 0; i < 1; i++) {
		swrite("\tAllocing 1024 Cells\t");
		for (j = 0; j < 1024; j++) {
			mb[j] = malloc(13);
			if (j % 64 == 0) swrite(".");
		}
		swrite("Done. \n");

		swrite("\tTesting 1024 Cells\t");
		for (j = 0; j < 1024; j++) {
			memclr(mb[j], 13);
			if (j % 64 == 0) swrite(".");
		}
		swrite("Done. \n");

		swrite("\tFreeing 1024 Cells\t");
		for (j = 0; j < 1024; j++) {
			free(mb[j]);
			if (j % 64 == 0) swrite(".");
		}
		swrite("Done. \n");
	}

	swrite("\tCycling 2^16 Cells\t");
	for (i = 0; i < 256; i++) {
		for (j = 0; j < 256; j++) {
			mb[j] = malloc(8);
		}
		for (j = 0; j < 256; j++) {
			free(mb[j]);
		}
		if (i % 16 == 0) swrite(".");
	}
	swrite("Done. \n");

	swrite("\tCycling 2^12 Blocks\t");
	for (i = 0; i < 16; i++) {
		for (j = 0; j < 256; j++) {
			mb[j] = malloc(0x1000);
		}
		for (j = 0; j < 256; j++) {
			free(mb[j]);
		}
		swrite(".");
	}
	swrite("Done. \n");

	swrite("\nAll tests passed.\n");

	while (1) {
		bufsize = console_read(buffer, 10);
		console_write(buffer, bufsize);
	}

	for(;;);
	return 0;
}
