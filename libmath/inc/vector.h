#ifndef MATH_VECTOR_H
#define MATH_VECTOR_H

#include <stdint.h>

typedef struct vector {
	size_t vdim;	/* Vector dimension */
	size_t size;	/* Element size in bytes */
	uint8_t v[];	/* Element vector */
} vector_t;

/* basic vector operations */

typedef void (*vop)(void *element, size_t size);

vector_t *vnew(size_t vdim, size_t size);
vector_t *vdim(vector_t *v, size_t vdim);
vector_t *vcpy(vector_t *vd, const vector_t *vs);
vector_t *vact(vector_t *v, vop op);

vector_t *vset(vector_t *v, const void *data, size_t n);
vector_t *vget(vector_t *v, void *data, size_t n);

#endif/*MATH_VECTOR_H*/
