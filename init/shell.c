#include <stdlib.h>
#include <string.h>
#include <driver.h>
#include <stdarg.h>
#include <stdio.h>

#include <driver/pci.h>

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
static void pci(int argc, char **argv);
static void scan(int argc, char **argv);
static void cd(int argc, char **argv);
static void ls(int argc, char **argv);
static void halt(int argc, char **argv);
static void class(int argc, char **argv);

static const char *cmdlist[] = {
	"echo",
	"pci",
	"scan",
	"cd",
	"ls",
	"halt",
	"class",
	NULL,
};

static void (*cmd[])(int, char**) = {
	echo,
	pci,
	scan,
	cd,
	ls,
	halt,
	class,
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

	printf("Flux 0.2a\n");

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

static void pci(int argc, char **argv) {
	uint32_t bus, slot, func, off, dev;

	if (argc != 5) {
		printf("pci <bus> <slot> <function> <offset>\n");
		return;
	}

	func = atoi(argv[3]);
	bus = atoi(argv[1]);
	slot = atoi(argv[2]);
	off = atoi(argv[4]);

	dev = pci_address_dev(bus, slot, func);
	
	printf("bus %d slot %d function %d offset 0x%x: %x\n", 
		bus, slot, func, off, pci_config_inw(dev, off));
}

static void scan(int argc, char **argv) {
	uint32_t bus, slot, func, dev, i;

	if (argc != 4) {
		printf("scan <bus> <slot> <func>\n");
		return;
	}

	bus = atoi(argv[1]);
	slot = atoi(argv[2]);
	func = atoi(argv[3]);

	dev = pci_address_dev(bus, slot, func);

	if (!pci_check_dev(dev)) {
		printf("bus %d slot %d func %d: no such device\n", bus, slot, func);
		return;
	}

	for (i = 0; i < 16; i += 4) {
		printf("0x%x: \t%x   \t%x\n", i, 
			pci_config_inw(dev, i + 2), pci_config_inw(dev, i));
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

static void class(int argc, char **argv) {
	uint8_t class, subclass;
	uint32_t dev;

	switch (argc) {
	case 2:
		class = atoi(argv[1]);
		dev = 0;

		while (1) {
			dev = pci_find_class(class, dev);
			if (dev) {
				printf("found at %x\n", dev);
			}
			else {
				break;
			}
		}

		break;
	case 3:
		class = atoi(argv[1]);
		subclass = atoi(argv[2]);
		dev = 0;

		while (1) {
			dev = pci_find_subclass(class, subclass, dev);
			if (dev) {
				printf("found at %x\n", dev);
			}
			else {
				break;
			}
		}

		break;
	default:
		printf("class <class> | class <class> <sublclass>\n");
	}
}
