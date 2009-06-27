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

task_t *next_task(u8int flags) {
	u32int pid;
	task_t *t;

	retry:
	if (!queue.next) {
		asm volatile ("sti");
		for(;;) asm volatile ("hlt");
	}
	pid = queue.next;
	t = get_task(pid);
	queue.next = t->next_task;
	t->next_task = 0;
	if (t->magic == 0x4224) insert_sched(pid);
	if (t->flags & TF_BLOCK || t->magic != 0x4224)
		goto retry; // If errors occur, redo 

	return t;
}
