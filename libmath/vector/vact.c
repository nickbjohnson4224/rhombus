/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>

#include <math/vector.h>

vector_t *vact(vector_t *v, vop op) {
	size_t i;

	for (i = 0; i < v->vdim; i++) {
		op(&(v->v[v->size * i]), v->size);
	}

	return v;
}
