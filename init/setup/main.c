#include <lib.h>

int *signal_table = (void*) 0xF7FFF000;

/*#define CDRV 0
#define DDRV 1
int driverlist[10];

void print(char *message) {
	sint(driverlist[CDRV], 17, 0, 0, strlen(message), (u32int) message, 1);
}

void gets(char *buffer) {
	sint(driverlist[CDRV], 16, 0, 0, 0, (u32int) buffer, 1);
}

void sleep(int cycles) {
	volatile int i;
	for (i = 0; i < cycles; i++);
}

int echo(char **argv) {
	int i;
	for (i = 1; argv[i]; i++) {
		print(argv[i]);
		print(" ");
	}
	print("\n");
	return 0;
}

int halt(char **argv) {
	print("It is now safe to turn off your computer\n");
	sint(driverlist[CDRV], 4, 0, 0, 0, 0, 0);
	exit(0);
	return 1;
}

char *cmdlist[] = {
"echo",
"halt",
"help",
(void*) 0};

int help(char **argv) {
	int i;
	for (i = 0; cmdlist[i]; i++) {
		print(cmdlist[i]);
		print("\t");
	}
	print("\n");
	return 1;
}

typedef int (*cmd_t) (char **argv);
cmd_t cmd[] = {
echo,
halt,
help,
};

void death_handler() {
}

char buffer[1024];
char *slice[8];
int init() {
	int pid, i, n;

	if ((pid = fork()) < 0) init_cdrv();
	driverlist[CDRV] = pid;

	signal_table[7] = (int) death_handler;

	sleep(10000);
	while(1) {
		print("knish $ ");
		gets(buffer);

		slice[0] = buffer;
		for (i = 0, n = 1; buffer[i]; i++) {
			if (buffer[i] == ' ' || buffer[i] == '\t') {
				buffer[i] = '\0';
				slice[n++] = &buffer[++i];
			}
		}
		slice[n] = (void*) 0;

		for (i = 0; cmdlist[i]; i++) if (!strcmp(slice[0], cmdlist[i])) break;
		if (!cmdlist[i]) {
			print("command not found: ");
			print(slice[0]);
			print("\n");
		}
		else cmd[i](slice);
	}

	for(;;);
	return 0;
} */

char keymap[128] = "\0\0331234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\001\\zxcvbnm,./\001*\0 ";
char upkmap[128] = "\0\033!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\001|ZXCVBNM<>?\001*\0 ";

int up;
void kb_handler() {
	char c = inb(0x60);
	if (keymap[c & 0x7F] == '\001') up ^= 1;
	if (c & 0x80 || keymap[(int) c] == '\001') sret(0);
	c = (up) ? upkmap[(int) c] : keymap[(int) c];
	if (c) printf("%c", c);
	sret(0);
}

void death() {
	printf("!");
}

short *vmem = (void*) 0x100000;
int init() {
	fmap(0, (u32int) vmem, 0xB8000, 4000, 0x7);
	cleark();
	curse(0, 0);

	signal_table[3] = (int) kb_handler;
	signal_table[7] = (int) death;
	rirq(1);

	printf("Fork stress test:\n");
	while (1) {
		printf("i");
		if (fork() < 0) {
			exit(0);
		}
	}

	for(;;);
	return 0;
}
