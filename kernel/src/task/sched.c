// Copyright 2009 Nick Johnson

#include <lib.h>
#include <task.h>
#include <trap.h>

struct sched_queue queue;

void insert_sched(u16int pid) {
	if (!queue.next) {
		queue.next = pid;
		queue.last = pid;
		task_t *t = get_task(pid);
		t->next_task = 0;
	}
	else {
		task_t *t = get_task(queue.last);
		t->next_task = pid;
		queue.last = pid;
	}
}

void init_sched() {
	queue.next = 0;
	queue.last = 0;
}

u16int next_task(u8int flags) {
	if (!queue.next) return 0;
	u32int pid = queue.next;
	task_t *t = get_task(pid);
	queue.next = t->next_task;
	t->next_task = 0;
	insert_sched(pid);
	return pid;
}

/*
struct sched_queue sched_queue[4];

static void add_to_queue(u8int queue, u16int pid) {
	if (queue > 3) return;
	if (!sched_queue[queue].last) {
		sched_queue[queue].next = pid;
		task_t *t = get_task(pid);
		t->next_task = 0;
	}
	else {
		task_t *t = get_task(sched_queue[queue].last);
		t->next_task = pid;
	}
	task_t *n = get_task(pid);
	n->quanta = 1 << queue;
}

void init_sched() {
	memclr(sched_queue, sizeof(struct sched_queue) * 4);
}

void insert_sched(u16int pid) {
	add_to_queue(0, pid);
}

static u8int meta_tick = 0;

u16int next_task(u8int flags) {
	task_t *t = get_task(curr_pid);
	if (t->quanta--) return curr_pid;

	u8int lbit = (meta_tick & -meta_tick); // Finds lowest set bit
	u8int queue = 0;
//	if (lbit == 0x02) queue = 1;
//	if (lbit == 0x04) queue = 2;
//	if (lbit == 0x08) queue = 3;

//	while (!sched_queue[queue].next) {
//		queue--;
//		if (queue == 0xFF) return 0;
//	}
	if (!sched_queue[queue].next) return next_task(flags);

	u16int pid = sched_queue[queue].next;
	task_t *n = get_task(pid);
	sched_queue[queue].next = n->next_task;
	if (!sched_queue[queue].next)
		sched_queue[queue].last = sched_queue[queue].next = 0;

	u8int next_queue = queue;
	if (flags == SF_FORCED) next_queue = (next_queue < 3) ? queue + 1 : queue;
	if (flags == SF_VOLUNT) next_queue = (next_queue > 0) ? queue - 1 : queue;
	add_to_queue(next_queue, pid);

	return pid;
} */
