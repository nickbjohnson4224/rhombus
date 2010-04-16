/* 
 * Copyright 2009 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <time.h>

/* Scheduling queue */
static process_t *queue_next = NULL;
static process_t *queue_last = NULL;

/* Insert a task into the scheduling queue */
void sched_ins(pid_t pid) {
	process_t *t;

	t = process_get(pid);
	if (!t) return;
	t->next_task = NULL;

	if (!queue_next) {
		queue_next = t;
		queue_last = t;
	}
	else {
		queue_last->next_task = t;
		queue_last = t;
	}
}

/* Remove a task from the scheduling queue */
void sched_rem(pid_t pid) {
	process_t *t, *t2;

	if (!queue_next) return;

	t2 = process_get(pid);
	if (!t2) return;

	if (queue_next->pid == pid) {
		queue_next = queue_next->next_task;
		return;
	}

	t = queue_next;
	while (t && t->next_task != t2) {
		t = t->next_task;
	}

	if (t && t->next_task == t2) {
		t->next_task = t2->next_task;
		if (queue_last == t2) {
			queue_last = t;
		}
	}
}

/* Get the next task from the scheduling queue, 
 * skipping blocked tasks and moving the schedulable task to the end */
process_t *task_next(uint8_t flags) {
	extern void idle(void);
	process_t *t;
	flags = 0;

	while (1) {
		t = queue_next;
		if (!t) idle();
		sched_rem(t->pid);
		sched_ins(t->pid);
		if ((t->flags & CTRL_BLOCK) == 0 && 
			(t->next_task == NULL)) {
			return t;
		}
	}
}
