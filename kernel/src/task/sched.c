// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

struct sched_queue queue;

void insert_sched(u16int pid) {
	task_t *t;

	if (!queue.next) {
		queue.next = pid;
		queue.last = pid;
		t = get_task(pid);
		t->next_task = 0;
	}
	else {
		t = get_task(queue.last);
		t->next_task = pid;
		queue.last = pid;
	}
}

__attribute__ ((section(".ttext")))
void init_sched() {
	queue.next = 0;
	queue.last = 0;
}

u16int next_task(u8int flags) {
	u32int pid;
	task_t *t;

	if (!queue.next) return 0;
	pid = queue.next;
	t = get_task(pid);
	queue.next = t->next_task;
	t->next_task = 0;
	insert_sched(pid);
	if (t->flags & TF_BLOCK) return next_task(0);

	return pid;
}
