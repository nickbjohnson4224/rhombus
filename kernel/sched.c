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
	task_t *t, *t2;

	if (!queue_next) return;

	t2 = task_get(pid);
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

	queue_last->next_task = NULL;
}

/* Get the next task from the scheduling queue, 
 * skipping blocked tasks and moving the schedulable task to the end */
task_t *task_next(uint8_t flags) {
	extern void idle(void);
	task_t *t;
	flags = 0;

	while (1) {
		t = queue_next;
		if (!t) {
			printk("idling\n");
			idle();
		}
		sched_rem(t->pid);
		sched_ins(t->pid);
		if ((t->flags & TF_BLOCK) == 0 && t->magic == 0x4224 
			&& (t->pid != curr_pid || t->next_task == NULL)) {
			return t;
		}
	}
}

void list_sched(void) {
	task_t *t, *f;

	f = queue_next;
	for (t = queue_next; t && t->next_task != f; t = t->next_task);
	if (t && t->next_task == f) {
		printk("scheduler loop detected!\n");
		return;
	}

	printk("sched contents: ");
	for (t = queue_next; t; t = t->next_task) {
		printk("%d ", t->pid);
	}
	printk("\n");
}
