#include <flux/io.h>
#include <flux/arch.h>
#include <flux/mmap.h>

static struct file *fdtable = (void*) FD_TABLE;

void fdinit(void) {
	int i;

	mmap(fdtable, sizeof(struct file) * 256, PROT_READ | PROT_WRITE);
	arch_memclr(fdtable, sizeof(struct file) * 256);

	for (i = 0; i < 8; i++) {
		fdtable[i].status = 1;
	}
}

int fdalloc(void) {
	int i;

	for (i = 0; i < 256; i++) {
		if (fdtable[i].status == 0) {
			fdtable[i].status = 1;
			break;
		}
	}

	if (i == 256) {
		return -1;
	}

	return i;
}

void fdfree(int fd) {
	fdtable[fd].status = 0;
}

int fdsetup(uint32_t target, uint32_t resource) {
	int fd;

	fd = fdalloc();

	fdtable[fd].target = target;
	fdtable[fd].resource = resource;

	return fd;
}

struct file *fdget(int fd) {
	if (!fdtable) return NULL;
	return &fdtable[fd];
}

void fdset(int fd, uint32_t target, uint32_t resource) {
	fdtable[fd].status = 1;
	fdtable[fd].target = target;
	fdtable[fd].resource = resource;
}
