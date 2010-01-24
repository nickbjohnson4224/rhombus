#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <stdarg.h>

extern size_t console_read(char *, size_t);
extern size_t console_write(char *, size_t);

/***** HELPER ROUTINES *****/

static void pint(int n, int b) {
	const char d[] = "0123456789ABCDEF";
	char a[11];
	int i;

	for (i = 0; i < 11; i++) {
		a[i] = d[n % b];
		n = n / b;
		if (!n) break;
	}

	for (; i >= 0; i--) {
		console_write(&a[i], 1);
	}
}

static void printf(const char *fmt, ...) {
	va_list nv;
	size_t i, v;

	va_start(nv, fmt);

	for (v = 0, i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			switch (fmt[i+1]) {
			case 'x':
				pint(va_arg(nv, int), 16);
				break;
			case 'd':
				pint(va_arg(nv, int), 10);
				break;
			case 'o':
				pint(va_arg(nv, int), 8);
				break;
			case 's':
				printf((const char*) nv[v++]);
				break;
			}
			i += 2;
		}
		console_write((void*) &fmt[i], 1);
	}
}

static char *gets(char *buffer) {
	char ch;
	size_t i = 0;

	while (1) {
		console_read(&ch, 1);
		console_write(&ch, 1);
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
static void pci(int argc, char **argv);
static void scan(int argc, char **argv);
static void cd(int argc, char **argv);
static void ls(int argc, char **argv);
static void halt(int argc, char **argv);

static const char *cmdlist[] = {
	"echo",
	"pci",
	"scan",
	"cd",
	"ls",
	"halt",
	NULL,
};

static void (*cmd[])(int, char**) = {
	echo,
	pci,
	scan,
	cd,
	ls,
	halt,
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

	printf("Flux 0.2a\n");

	while (1) {
		printf("fish $ ");
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

static uint16_t pci_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t off) {
	uint32_t addr;

	addr = (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000;
	outd(0xCF8, addr);

	return (ind(0xCFC) >> ((off & 0x2) * 8));
}

static void pci(int argc, char **argv) {
	uint32_t bus, slot, func, off;

	if (argc != 5) {
		printf("pci <bus> <slot> <function> <offset>\n");
		return;
	}

	func = atoi(argv[3]);
	bus = atoi(argv[1]);
	slot = atoi(argv[2]);
	off = atoi(argv[4]);
	
	printf("bus %d slot %d function %d offset 0x%x: %x\n", 
		bus, slot, func, off, pci_read(bus, slot, func, off));
}

static void scan(int argc, char **argv) {
	uint32_t bus, slot, i;

	if (argc != 3) {
		printf("scan <bus> <slot>\n");
		return;
	}

	bus = atoi(argv[1]);
	slot = atoi(argv[2]);

	if (pci_read(bus, slot, 0, 0) == 0xFFFF) {
		printf("bus %d slot %d: no such device\n", bus, slot);
		return;
	}

	for (i = 0; i < 16; i += 4) {
		printf("0x%x: \t%x   \t%x\n", i, 
			pci_read(bus, slot, 0, i + 2), pci_read(bus, slot, 0, i));
	}
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
