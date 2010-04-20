#ifndef FLUX_IO_H
#define FLUX_IO_H

#include <flux/arch.h>

struct file {
	uint32_t status;
	uint32_t target;
	uint32_t resource;
};

size_t read (int fd, void *buf, size_t size, uint64_t offset);
size_t write(int fd, void *buf, size_t size, uint64_t offset);

size_t finfo(int fd, void *buf, size_t size, size_t selector);
size_t fctrl(int fd, void *buf, size_t size, size_t selector);

struct file *fdget(int fd);

int  fdsetup(uint32_t target, uint32_t resource);
int  fdalloc(void);
void fdfree(int fd);

#endif
