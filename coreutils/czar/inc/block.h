#ifndef CZAR_BLOCK_H
#define CZAR_BLOCK_H

#include <stream.h>

typedef struct cz_block {
	int size;		// Size of the data block in bytes
	char *data;		// Data block
	int mtype[8];	// Type of each element of the metadata list
	void *meta[8];	// Metadata pointer list
} cz_block_t;

cz_block_t *cz_block_alloc(cz_block_t *block, int size);
cz_block_t *cz_block_fill(cz_block_t *block, FILE *stream, int size);
cz_block_t *cz_block_dump(cz_block_t *block, FILE *stream);

#endif
