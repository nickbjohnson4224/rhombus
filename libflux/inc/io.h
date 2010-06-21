#ifndef FLUX_IO_H
#define FLUX_IO_H

#include <flux/arch.h>

struct vfs_query {
	uint32_t command;
	uint32_t server;
	uint64_t inode;
	uint8_t  naddr[16];
	char path0[1000];
	char path1[1000];
};

struct info_query {
	char field[100];
	char value[1000];
};

struct call {
	char   name[16];
	size_t size;
	char   args[];
};

struct file {
	uint32_t status;
	uint32_t server;
	uint32_t inode;
	uint8_t  naddr[16];
};

size_t psend(int fd, char *r, char *s, size_t size, uint64_t off, uint8_t port);
size_t read (int fd, void *buf, size_t size, uint64_t offset);
size_t write(int fd, void *buf, size_t size, uint64_t offset);
size_t query(int fd, void *rbuf, void *sbuf, size_t size);
bool   info (int fd, char *value, const char *field);
bool   ctrl (int fd, char *value, const char *field);
char  *call (int fd, const char *name, const char *args);

struct file *fdget(int fd);
void         fdset(int fd, uint32_t target, uint32_t resource);

int  fdsetup(uint32_t target, uint32_t resource);
int  fdalloc(void);
void fdfree(int fd);

#define VFS_CMD_FIND  0
#define VFS_CMD_ADD   1
#define VFS_CMD_LIST  2
#define VFS_CMD_LINK  3

#define VFS_CMD_REPLY 10
#define VFS_CMD_ERROR 11

int find(const char *path);
int fadd(const char *path, uint32_t server, uint64_t inode);
int list(const char *path, char *buffer);

#define creat fdsetup
#define close fdfree

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
