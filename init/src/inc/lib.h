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

/***** SYSCALL STUBS *****/
extern int fork();
extern void exit(int value);
extern int sint(u32int target, int argv[3], u32int sig, u32int flags);
extern void sret(u32int flags);
extern int mmap(u32int addr, u32int flags);
extern int umap(u32int addr);
extern int rmap(u32int src, u32int dest, u32int flags);
extern int fmap(u32int target, u32int src, u32int dest);
extern int rirq(u32int irq);

extern u8int inb(u16int port);
extern void outb(u16int port, u8int value);

#endif
