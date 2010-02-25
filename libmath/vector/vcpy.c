/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <stdint.h>
#include <stdlib.h>

#include <math/vector.h>

vector_t *vcpy(vector_t *vd, const vector_t *vs) {
	size_t i;

	for (i = 0; i < vd->vdim && i < vs->vdim; i++) {
		vd->v[i] = vs->v[i];
	}

	return vd;
}
