// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

struct sched_queue queue;

void insert_sched(pid_t pid) {
	task_t *t;

	if (!queue.next) {
		queue.next = pid;
		queue.last = pid;
		t = get_task(pid);
		t->next_task = pid;
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

	retry:
	pid = queue.next;
	t = get_task(pid);
	queue.next = t->next_task;
	insert_sched(pid);
	if (t->flags & TF_BLOCK) {
		if (queue.next == queue.last) goto idle; // If the only task left is blocked, idle
		goto retry; // If this task is blocked, try the next one
	}

	printk("switching to %d %d : %d\n", (uint32_t) queue.next, (uint32_t) queue.last, t->pid);
	return t;

	idle:
	printk("IDLE %d %d\n", queue.next, queue.last);
	asm volatile ("sti");
	for(;;) asm volatile ("hlt");
}
