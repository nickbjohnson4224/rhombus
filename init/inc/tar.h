#ifndef TAR_H
#define TAR_H

#define TAR_BLOCKSIZE 512

struct tar_file {
	char *name;
	void *start;
	size_t size;
};

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

size_t           tar_size(uint8_t *base);
struct tar_file *tar_parse(uint8_t *base);
struct tar_file *tar_find(struct tar_file *archive, char *name);

#endif/*TAR_H*/
