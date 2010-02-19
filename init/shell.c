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
static void read(int argc, char **argv);
static void seek(int argc, char **argv);

static const char *cmdlist[] = {
	"echo",
	"cd",
	"ls",
	"halt",
	"read",
	"seek",
	NULL,
};

static void (*cmd[])(int, char**) = {
	echo,
	cd,
	ls,
	halt,
	read,
	seek,
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
	char lnbuffer[100];
	char *argv[10];
	size_t i, n;

	char *pwd = "/";

	printf("\nLaunching Flux Init SHell\n");

	while (1) {
		printf("fish %s $ ", pwd);
		gets(lnbuffer);

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

static void read(int argc, char **argv) {
	size_t size;
	char buffer[257];

	if (argc != 2) {
		printf("read <bytes>\n");
		return;
	}

	size = atoi(argv[1]);

	while (size) {

		fread(buffer, sizeof(char), (size > 256) ? 256 : size, disk);

		buffer[256] = '\0';
		printf(buffer);

		size -= (size > 256) ? 256: size;
	}

	printf("\n");
}

static void seek(int argc, char **argv) {
	size_t pos;
	
	if (argc <= 1) return;

	pos = atoi(argv[1]);

	fseek(disk, pos, SEEK_SET);
}
