/* 
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <driver/pci.h>

extern FILE *disk;

/***** HELPER ROUTINES *****/

static char *gets(char *buffer) {
	char ch;
	size_t i = 0;

	while (1) {
		ch = getchar();
		if (ch == '\n') break;
		if (ch == '\b') {
			i--;
			continue;
		}
		buffer[i++] = ch;
	}

	buffer[i] = '\0';
	return buffer;
}

/***** COMMANDS *****/

static void echo(int argc, char **argv);
static void cd(int argc, char **argv);
static void ls(int argc, char **argv);
static void halt(int argc, char **argv);
static void ata_read(int argc, char **argv);
static void seek(int argc, char **argv);
static void gen(int argc, char **argv);
static void tar(int argc, char **argv);

static const char *cmdlist[] = {
	"echo",
	"cd",
	"ls",
	"halt",
	"read",
	"seek",
	"gen",
	"tar",
	NULL,
};

static void (*cmd[])(int, char**) = {
	echo,
	cd,
	ls,
	halt,
	ata_read,
	seek,
	gen,
	tar,
};

static int vexec(char *name, int argc, char **argv) {
	size_t i;

	for (i = 0; cmdlist[i]; i++) {
		if (strcmp(name, cmdlist[i]) == 0) {
			cmd[i](argc, argv);
			return 0;
		}
	}
	return 1;
}

/***** SHELL *****/

void shell(void) {
	char lnbuffer[1000];
	char *argv[10];
	size_t i, n;

	char *pwd = (char*) "/";

	printf("\nLaunching Flux Init SHell\n");

	while (1) {
		printf("fish %s $ ", pwd);
		fgets(lnbuffer, 1000, stdin);
		lnbuffer[strlen(lnbuffer) - 1] = '\0';

		argv[0] = lnbuffer;

		for (n = 1, i = 0; lnbuffer[i]; i++) {
			if (lnbuffer[i] == ' ') {
				lnbuffer[i] = '\0';
				argv[n++] = &lnbuffer[i+1];
			}
		}
		argv[n] = NULL;

		if (vexec(argv[0], n, argv)) printf("%s: command not found\n", argv[0]);
	}
}

static void echo(int argc, char **argv) {
	int i;
	for (i = 1; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
}

static void cd(int argc, char **argv) {
	if (argc > 1) {
		printf("cd: %s: no such directory\n", argv[1]);
	}
}

static void ls(int argc, char **argv) {
	printf("\n");
}

static void halt(int argc, char **argv) {
	outb(0x64, 0xFE);
}

static void ata_read(int argc, char **argv) {
	size_t size, rsize;
	char buffer[257];

	if (argc != 2) {
		printf("read <bytes>\n");
		return;
	}

	size = atoi(argv[1]);

	while (size) {

		rsize = fread(buffer, sizeof(char), (size > 256) ? 256 : size, disk);

		buffer[rsize] = '\0';
		printf(buffer);

		size -= (size > rsize) ? rsize: size;
	}

	printf("\n");
}

static void seek(int argc, char **argv) {
	size_t pos;
	
	if (argc <= 1) return;

	pos = atoi(argv[1]);

	fseek(disk, pos, SEEK_SET);
}

static void gen(int argc, char **argv) {
	size_t i, n;

	n = atoi(argv[1]);

	for (i = 0; i < n; i++) {
		putchar('!');
	}

	printf("\n");
}

static void read_tar_header(char *block) {
	
	printf("name:  %s\n", &block[  0]);
	printf("mode:  %s\n", &block[100]);
	printf("uid:   %s\n", &block[108]);
	printf("gid:   %s\n", &block[116]);
	printf("size:  %s\n", &block[124]);
	printf("mtime: %s\n", &block[136]);
	printf("cksum: %s\n", &block[148]);
	printf("link:  %s\n", &block[157]);

}

static bool tar_nullp(char *block) {
	size_t i, s;

	for (s = 0, i = 0; i < 512; i++) {
		if (block[i]) s++;
	}

	return (s) ? false : true;
}

static void tar(int argc, char **argv) {
	char buffer[513];
	size_t size, tsize;
	
	fread(buffer, sizeof(char), 512, disk);
	read_tar_header(buffer);
	printf("\n");

	size = atoi(&buffer[124]) - 512;

	if (size % 512) {
		size = (size - (size % 512) + 512);
	}

	printf("size %d\n", size);

	fseek(disk, size, SEEK_CUR);

	printf("seek %d\n", disk->position);
}
