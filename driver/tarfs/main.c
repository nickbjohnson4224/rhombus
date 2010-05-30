/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <flux/arch.h>
#include <flux/driver.h>
#include <flux/proc.h>
#include <flux/ipc.h>
#include <flux/io.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char name[100];
char path[100];

int device;
FILE *devicefile;

static void tar_parse(FILE *driver);

static struct tarfs_inode {
	char name[100];
	uint32_t offset;
	uint32_t size;
} inode_table[256];

static void tarfs_info(uint32_t source, struct packet *packet) {
	struct info_query *query;
	
	if (!packet) {
		return;
	}

	query = packet_getbuf(packet);

	if (!strcmp(query->field, "size")) {
		if (packet->target_inode == 0) {
			strcpy(query->value, "");
		}
		else {
			sprintf(query->value, "%d", inode_table[packet->target_inode].size);
		}
	}
	else {
		strcpy(query->value, "");
	}

	send(PORT_REPLY, source, packet);
	packet_free(packet);
}

static void tarfs_read(uint32_t source, struct packet *packet) {
	uint32_t offset;

	offset = packet->offset + inode_table[packet->target_inode].offset;

	if (packet->offset + packet->data_length > 
		inode_table[packet->target_inode].size) {
		packet_setbuf(&packet, inode_table[packet->target_inode].size
			- packet->offset);
	}

	fseek(devicefile, offset, SEEK_SET);
	fread(packet_getbuf(packet), packet->data_length, 1, devicefile);

	send(PORT_REPLY, source, packet);
	packet_free(packet);
}

static void getname(char *name, char *path) {
	int i;
	
	for (i = strlen(path); i >= 0; i--) {
		if (path[i] == '/') {
			i++;
			break;
		}
	}

	strcpy(name, &path[i]);
}

int main(int argc, char **argv) {

	if (argc < 2) {
		return 1;
	}

	getname(name, argv[1]);
	strcat(name, ".");
	strcat(name, argv[0]);

	if (argc >= 3) {
		strcpy(path, argv[2]);
	}
	else {
		strcpy(path, "/");
		strcat(path, name);
	}

	fadd(path, getpid(), 1);

	device = find(argv[1]);
	devicefile = fdopen(device, "r");

	tar_parse(devicefile);

	when(PORT_INFO, tarfs_info);
	when(PORT_READ, tarfs_read);

	printf("%s: ready\n", name);

	send(PORT_SYNC, getppid(), NULL);
	_done();

	return 0;
}

struct tar_block {
	char filename[100];
	char mode[8];
	char owner[8];
	char group[8];
	char filesize[12];
	char timestamp[12];
	char checksum[8];
	char link[1];
	char linkname[100];
};

static uintptr_t getvalue(char *field, size_t size) {
	uintptr_t sum, i;

	sum = 0;

	for (i = 0; i < size && field[i]; i++) {
		sum *= 8;
		sum += field[i] - '0';
	}

	return sum;
}

static void tar_parse(FILE *driver) {
	struct tar_block *block;
	char fpath[100];
	uintptr_t i, n;

	block = malloc(512);

	i = 0;
	n = 2;

	while (1) {
		fread(block, 1, 512, driver);

		if (block->filename[0] == '\0' || block->filename[0] == ' ') {
			break;
		}

		strcpy(fpath, path);
		strcat(fpath, "/");
		strcat(fpath, block->filename);
		fadd(fpath, getpid(), n);

		printf("file %s at %d\n", block->filename, i + 512);

		strcpy(inode_table[n].name, block->filename);

		inode_table[n].offset = i + 512;
		inode_table[n].size = getvalue(block->filesize, 12);

		if (inode_table[n].size % 512) {
			i += ((inode_table[n].size / 512) + 2) * 512;
		}
		else {
			i += inode_table[n].size + 512;
		}

		fseek(driver, i, SEEK_SET);

		n++;
	}

	free(block);
}
