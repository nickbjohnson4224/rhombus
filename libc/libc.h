#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>
#include <stdbool.h>

/*** Heap ***/

struct _heap_allocator {
	size_t max_size;
	void (*init)(void);		/* Initialize allocator */
	void*(*alloc)(size_t);	/* Allocate chunk */
	void (*free)(void*);	/* Free chunk */
	bool (*cont)(void*);	/* Check if is in charge of chunk */
	size_t(*size)(void*);	/* Check size of chunk */
};

void   _heap_init(void);
void  *_heap_alloc(size_t size);
void   _heap_free(void *ptr);
size_t _heap_size(void *ptr);

/*** File Handles ***/

typedef uint32_t _file_des_t;

typedef struct _file {
	uint32_t magic;
	uint32_t target;
	uint32_t resource;
	size_t position;
} _file_t;

_file_des_t new_file(void);
void del_file(_file_des_t fd);
_file_t *get_file(_file_des_t fd);

#endif
