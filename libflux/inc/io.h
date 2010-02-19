#ifndef FLUX_IO_H
#define FLUX_IO_H

#include <flux/arch.h>

struct file {
	uint32_t target;
	uint32_t resource;
};

size_t read (struct file *fd, void *buf, size_t size, uint64_t offset);
size_t write(struct file *fd, void *buf, size_t size, uint64_t offset);

size_t finfo(struct file *fd, void *buf, size_t size, size_t selector);
size_t fctrl(struct file *fd, void *buf, size_t size, size_t selector);

#endif
