/* Copyright 2009 Nick Johnson */

#include <lib.h>
#include <task.h>
#include <int.h>

/* Scheduling queue */
static task_t *queue_next = NULL;
static task_t *queue_last = NULL;

/* Insert a task into the scheduling queue */
void sched_ins(pid_t pid) {
	task_t *t;

	t = task_get(pid);
	if (!t) return;

	if (!queue_next) {
		queue_next = t;
		queue_last = t;
	}
	else {
		queue_last->next_task = t;
		queue_last = t;
	}

	t->next_task = NULL;
}

/* Remove a task from the scheduling queue */
void sched_rem(pid_t pid) {
	task_t *t, *t2;

	if (!queue_next) return;

	t2 = task_get(pid);
	if (!t2) return;

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

	if (queue_next->pid == pid) {
		queue_next = queue_next->next_task;
	}
}

/* Get the next task from the scheduling queue, 
 * skipping blocked tasks and moving the schedulable task to the end */
task_t *task_next(uint8_t flags) {
	extern void idle(void);
	task_t *t, *f;
	flags = 0;

	f = queue_next;
	while (1) {
		t = queue_next;
		if (!t || t->next_task == f) {
			printk("idling\n");
			idle();
		}
		sched_rem(t->pid);
		sched_ins(t->pid);
		if ((t->flags & TF_BLOCK) == 0) {
			return t;
		}
	}
}

void list_sched(void) {
	task_t *t;

	printk("sched contents: ");
	for (t = queue_next; t; t = t->next_task) {
		printk("%d ", t->pid);
	}
	printk("\n");
}
