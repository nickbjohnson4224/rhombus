#ifndef TARFS_H
#define TARFS_H

#include <stdint.h>
#include <ipc.h>

struct tarfs_inode {
	char     name[100];
	uint64_t offset;
	uint32_t size;
};

extern struct tarfs_inode inode[256];
extern char name[100];
extern char root[100];

extern bool m_parent;
extern FILE *parent;

void tarfs_init(void);
void tarfs_info(uint32_t source, struct packet *packet);
void tarfs_read(uint32_t source, struct packet *packet);

#endif/*TARFS_H*/
