// Copyright 2009 Nick Johnson

#ifndef LIB_H
#define LIB_H

typedef unsigned int u32int;
typedef unsigned short u16int;
typedef unsigned char u8int;
typedef unsigned int addr_t;

/***** STRING FUNCTIONS *****/
void *memcpy(void *dest, void *src, u32int size);
void *memset(void *dest, u8int src, u32int size);
void *memclr(void *dest, u32int size);
void *pgclr(u32int *base);
char *strcpy(char *dest, char *src);
int strcmp(char *s1, char *s2);
u32int strlen(char *str);
u32int atoi(char *str, u8int base);
char *itoa(u32int n, char *buf, u8int base);

/***** OUTPUT FUNCTIONS *****/
void cleark();
void printk(char *fmt, ...);
void colork(u8int color);
void cursek(u8int x, u8int y);
#define printf printk
#define colorf colork
#define curse cursek

/***** SYSCALL STUBS *****/
extern int fork();
extern void exit(int value);
extern int sint(int task, u32int sig, u32int arg0, u32int arg1, u32int arg2, u32int arg3, u32int flags);
extern void sret(u32int flags);
extern int mmap(addr_t addr, u32int size, u32int flags);
extern int umap(addr_t addr, u32int size);
extern int rmap(addr_t dest, addr_t src, u32int size, u32int flags);
extern int fmap(int target, addr_t dest, addr_t src, u32int size, u32int flags);
extern void eout(char *message);
extern void rirq(u32int irq);

extern u8int inb(u16int port);
extern void outb(u16int port, u8int val);

#endif
