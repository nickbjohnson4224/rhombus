/*
 * Copyright (C) 2011-2012 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>

#include <rho/mutex.h>
#include <rho/ipc.h>

struct mqueue_msg {
	struct mqueue_msg *next;
	struct mqueue_msg *prev;

	struct msg *msg;
};

struct mqueue {
	struct mqueue_msg *back;
	struct mqueue_msg *front;

	uint32_t wl_tid;

	bool mutex;
};

static struct mqueue mqueue[256];
static bool   mqueue_policy[256];

/*****************************************************************************
 * mqueue_set_policy
 *
 * Set the policy for queueing messages for a specified action. If <do_queue>
 * is true, messages from the action port are queued. If <do_queue> is false, 
 * messages from the action port are not queued. By default, all action ports 
 * are set to queue.
 *
 * Note that mqueue_policy[action] is set to the NOT of <do_queue>.
 */

void mqueue_set_policy(uint8_t action, bool do_queue) {	
	mqueue_policy[action] = (do_queue) ? false : true;
}

/*****************************************************************************
 * mqueue_push
 *
 * Add a message to the message queue. The message's header is used to sort
 * it into the proper queue. <msg> may not be NULL. Returns zero on success,
 * nonzero on error.
 */

int mqueue_push(struct msg *msg) {
	struct mqueue_msg *node;
	uint32_t wl_tid;
	uint8_t action;

	if (!msg) {
		return 1;
	}

	action = msg->action;

	if (mqueue_policy[action] == true) {
		return 0;
	}

	mutex_spin(&mqueue[action].mutex);

	node = malloc(sizeof(struct mqueue_msg));

	if (!node) {
		return 1;
	}
	
	node->next = NULL;
	node->prev = mqueue[action].back;
	node->msg  = msg;

	if (!mqueue[action].front) mqueue[action].front = node;
	if (mqueue[action].back)   mqueue[action].back->next = node;
	mqueue[action].back = node;

	wl_tid = mqueue[action].wl_tid;
	mqueue[action].wl_tid = 0;

	mutex_free(&mqueue[action].mutex);

	if (wl_tid) {
		wake(wl_tid - 1);
	}

	return 0;
}

/*****************************************************************************
 * mqueue_pull
 *
 * Find the first message in the message queue with action <action> and source
 * <source>. If <source> is zero, any source matches. Returns the found
 * message on success, NULL on failure.
 */

struct msg *mqueue_pull(uint8_t action, uint64_t source) {
	struct mqueue_msg *node;
	struct msg *msg;
	
	mutex_spin(&mqueue[action].mutex);

	if (source) {
		for (node = mqueue[action].front; node; node = node->next) {
			if (node->msg->source == source) {
				break;
			}
		}
	}
	else {
		node = mqueue[action].front;
	}
	
	if (!node) {
		mutex_free(&mqueue[action].mutex);
		return NULL;
	}

	if (node->prev) node->prev->next = node->next;
	else mqueue[action].front = node->next;

	if (node->next) node->next->prev = node->prev;
	else mqueue[action].back = node->prev;

	mutex_free(&mqueue[action].mutex);

	msg = node->msg;
	free(node);
	return msg;
}

/*****************************************************************************
 * mqueue_wait
 *
 * Find the first message in the message queue with action <action> and source
 * <source. If <source> is zero, any source matches. If there is no match,
 * this function blocks until a match is found by mqueue_push(). Returns the
 * found message on success, waits forever on failure.
 */

struct msg *mqueue_wait(uint8_t action, uint64_t source) {
	struct msg *msg;

	msg = mqueue_pull(action, source);

	while (!msg) {
		
		mutex_spin(&mqueue[action].mutex);
		if (mqueue[action].wl_tid) {
			mutex_free(&mqueue[action].mutex);
			sleep();
			msg = mqueue_pull(action, source);
			continue;
		}
		mqueue[action].wl_tid = gettid() + 1;
		mutex_free(&mqueue[action].mutex);

		stop();

		msg = mqueue_pull(action, source);
	}

	return msg;
}
