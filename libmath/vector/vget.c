/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <math/vector.h>

vector_t *vget(vector_t *v, void *data, size_t n) {
	
	memcpy(data, &(v->v[v->size * n]), v->size);

	return v;
}
