/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <int.h>
#include <mem.h>
#include <task.h>

thread_t *thread_bind(thread_t *image, signal_t *signal) {

	memcpy(&image->signal, signal, sizeof(signal_t));

	return image;
}
