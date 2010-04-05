/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#include <lib.h>
#include <task.h>
#include <int.h>

/****************************************************************************
 * thread_drop
 *
 * Kills the given thread and switches to another runnable thread. If legacy
 * mode is enabled, the other runnable thread is the next thread on the 
 * thread image stack of the current process.
 */

thread_t *thread_drop(thread_t *image) {
	thread_t *old_image;

	old_image = image;
	image->proc->image = image->tis;
	image = thread_switch(image, image->tis);
	thread_free(old_image);

	return image;
}
