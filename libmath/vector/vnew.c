/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>

#include <math/vector.h>

vector_t *vnew(size_t vdim, size_t size) {
	vector_t *v;

	v = malloc(sizeof(vector_t) + vdim * size);

	v->vdim = vdim;
	v->size = size;

	return v;
}
