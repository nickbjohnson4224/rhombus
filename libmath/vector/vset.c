/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <math/vector.h>

vector_t *vset(vector_t *v, const void *data, size_t n) {

	memcpy(&(v->v[v->size * n]), data, v->size);

	return v;
}
