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

void remove_sched(u16int pid) {
	task_t *t, *t2;

	if (queue.next) {
		t = get_task(queue.next);
		while (t->next_task != pid && t->next_task) 
			t = get_task(t->next_task);
		t2 = get_task(t->next_task);
		t->next_task = t2->next_task;
	}
}

task_t *next_task(u8int flags) {
	u32int pid, wasblocked = 0;
	task_t *t;
	flags = 0;

	retry:
	if (!queue.next) goto idle;
	pid = queue.next;
	t = get_task(pid);
	queue.next = t->next_task;
	t->next_task = 0;
	if (t->magic == 0x4224) insert_sched(pid);
	if (t->flags & TF_BLOCK || t->magic != 0x4224) {
		if (queue.next == queue.last) goto idle; // If the only task left is blocked, idle
		goto retry; // If errors occur, redo
	}

	idle:
	printk("IDLE\n");
	asm volatile ("sti");
	for(;;) asm volatile ("hlt");

	return t;
}
