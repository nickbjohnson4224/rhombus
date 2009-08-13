// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

struct sched_queue queue;

void insert_sched(pid_t pid) {
	task_t *t;

	if (!queue.next) {
		t = get_task(pid);
		t->next_task = pid;
		queue.next = pid;
		queue.last = pid;
	}
	else {
		t = get_task(queue.last);
		t->next_task = pid;
		queue.last = pid;
		t = get_task(pid);
		t->next_task = queue.next;
	}
}

void remove_sched(pid_t pid) {
	task_t *t, *t2;

	if (queue.next) {
		t = get_task(queue.next);
		while (t->next_task != pid && t->next_task)
			t = get_task(t->next_task);
		t2 = get_task(t->next_task);
		t->next_task = t2->next_task;
	}
}

task_t *next_task(uint8_t flags) {
	pid_t pid;
	task_t *t;
	flags = 0;

	do {
		pid = queue.next;
		t = get_task(pid);
		queue.next = t->next_task;
		insert_sched(pid);
	} while (t->flags & TF_BLOCK);

	return t;
}
