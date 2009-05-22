// Copyright 2009 Nick Johnson

#ifndef TASK_H
#define TASK_H

#include <lib.h>
#include <trap.h>
#include <mem.h>

/***** SIGNALS *****/

// Signals (system) (sig < 16) (esi = sig #)
#define S_GEN 0	// Generic fault 	(ebx = fault #, ecx = errcode)
#define S_ENT 1	// Reentrance		(ebx = tick)
#define S_PAG 2	// Page fault		(ebx = address, ecx = flags)
#define S_IRQ 3	// Registered IRQ	()
#define S_KIL 4	// Kill signal		(ebx = identifier)
#define S_IMG 5	// Image overflow	()
#define S_FPE 6 // FP exception		(ebx = eip)
#define S_DTH 7 // Child death		(eax = child pid, ebx = exit value)

#define ENOTASK (-1)
#define ENOSIG (-2)
#define EPERMIT (-3)

/* This is the most complex, and most important, system call. It sends signal
type to task task with various arguments and is controlled by flags. Arguments
to this function are in the order shown as registers */

image_t *ksignal(u32int sender, u32int task, u32int sig, u32int arg0, u32int arg1, u32int arg2);
image_t *signal(u32int task, u32int sig, u32int arg0, u32int arg1, u32int arg2, u32int flags);
image_t *sret();

// The signal table is mapped in all address spaces
u32int *signal_table;

/***** PERMISSIONS *****/

// Ring level provides general permissions
// 0 = Hardware level (kernel, drivers) 	(can use ports)
// 1 = System level (filesystem, graphics)
// 2 = Superuser level (root)
// 3+ = User level

// Every level has override power over all greater levels
// This means a program can access memory, send privileged
// signals, etc. if it is at a higher level or has the same
// user id as the target task.

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
	u8int flags;
	u8int quanta;
	u16int next_task;
	u16int parent;
	u16int magic;
	u32int tss_esp;
	u32int reserved;
} task_t;

#define TF_READY 0x0000
#define TF_BLOCK 0x0001

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
