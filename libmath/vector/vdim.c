/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>

#include <math/vector.h>

vector_t *vdim(vector_t *v, size_t vdim) {
	return realloc(v, sizeof(vector_t) + v->size * vdim);
}
