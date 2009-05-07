// Copyright 2009 Nick Johnson

#ifndef TASK_H
#define TASK_H

#include <lib.h>
#include <trap.h>
#include <mem.h>

/***** SYSTEM CALLS *****/

u32int exit(u32int val);				// End current task
u32int fork(u32int flags);				// Create new task as clone
u32int wait(u32int task);				// Wait for task to end
u32int gpid(u32int flags);				// Get PID of relative task

/***** PERMISSIONS *****/

// Ring level provides general permissions
// 0 = Hardware level (kernel, drivers) 	(can use ports / remap memory)
// 1 = System level (filesystem, graphics) 	(can remap memory)
// 2 = Superuser level (root) 				(can kill anything / etc.)
// 3+ = User level
// Every level has override power over all greater levels

typedef struct {
	u16int ring;
	u16int id;
} id_t;

void init_task();

/***** TASK TABLE *****/
typedef struct {
	id_t user;
	map_t map;
	image_t *image;
	u32int sigmask[2];
	u16int flags;
	u16int next_task;
	u16int quanta;
	u16int parent;
	u32int reserved;
} task_t;

#define TF_READY 0x00
#define TF_BLOCK 0x01

task_t *get_task(u16int pid);
u16int new_task(u16int src_pid);
u32int rem_task(u16int pid);
image_t *task_switch(u16int pid);

pool_t *tmap;		// Pool allocator for task structures
task_t *task[128]; 	// 2D array (128*128) of task structures
u16int curr_pid;	// Currently loaded task ID

/***** SYSTEM MAP *****/

// Information about a driver task
struct driv_id {
	u32int magic;
	u16int pid;
	u16int device;
	u16int creator;
	u16int model;
	u16int type;
	u16int version;
};

// Information about a device
struct device {
	u16int num;
	u16int irq;
	u16int port_base;
	u16int port_limit;
};

#define DRV_TYPE_BLOCK 1
#define DRV_TYPE_CHAR 2
#define DRV_TYPE_FS 3

struct sysmap {
	struct device device[256];
	struct driv_id driver[256];
} *sysmap;
	
void init_sysmap();

/***** SCHEDULER *****/

struct sched_queue {
	u16int next;
	u16int last;
};

void init_sched();
void insert_sched(u16int pid);
u16int next_task(u8int flags);

#define SF_FORCED 0x00
#define SF_VOLUNT 0x01

#endif /*TASK_H*/
