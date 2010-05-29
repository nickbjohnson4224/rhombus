#ifndef FLUX_IO_H
#define FLUX_IO_H

#include <flux/arch.h>

struct info_query {
	char field[100];
	char value[100];
};

struct file {
	uint32_t status;
	uint32_t target;
	uint32_t resource;
};

size_t read (int fd, void *buf, size_t size, uint64_t offset);
size_t write(int fd, void *buf, size_t size, uint64_t offset);
size_t query(int fd, void *rbuf, void *sbuf, size_t size);

bool info(int fd, char *value, const char *field);
bool ctrl(int fd, char *value, const char *field);

struct file *fdget(int fd);
void         fdset(int fd, uint32_t target, uint32_t resource);

int  fdsetup(uint32_t target, uint32_t resource);
int  fdalloc(void);
void fdfree(int fd);

void fdinit(void);

#define FD_STDIN	0
#define FD_STDOUT	1
#define FD_STDERR	2
#define FD_STDVFS	3
#define FD_STDDEV	4
#define FD_STDPMD	5
#define FD_EXTIN	6
#define FD_EXTOUT	7

#endif
