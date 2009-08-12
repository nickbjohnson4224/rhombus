#ifndef CALL_H
#define CALL_H

#include <lib.h>

/***** INTERNAL CALLS *****/
extern int fork_call();
extern void exit_call(int value);
extern int sint_call(int task, u32int sig, u32int argv[4], u32int flags);
extern void sret_call(u32int flags);
extern int mmap_call(addr_t addr, u32int size, u32int flags);
extern int umap_call(addr_t addr, u32int size);
extern int rmap_call(addr_t dest, addr_t src, u32int size, u32int flags);
extern int fmap_call(int target, addr_t dest, addr_t src, u32int size, u32int flags);
extern int rsig_call(u32int sig, addr_t value);

extern void eout(char *message);
extern void rirq(u32int irq);

#define IF_BLOCK 0x00000001
#define IF_UNBLK 0x00000002
#define MF_USER  0x00000004
#define MF_WRITE 0x00000002

/***** EXTERNAL CALLS *****/
int fork();
int exit(int value);
int ssend(int dest, int type, char buffer[12]);
int asend(int dest, int type, char buffer[12]);
int recv(int *src, char buffer[12]);
int kill(int target);

void *call_table[32];

#endif/*CALL_H*/
