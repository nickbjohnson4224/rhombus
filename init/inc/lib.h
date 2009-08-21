// Copyright 2009 Nick Johnson

#ifndef LIB_H
#define LIB_H

typedef unsigned int u32int;
typedef unsigned short u16int;
typedef unsigned char u8int;
typedef unsigned int addr_t;

#define PAGESZ 0x1000

int *signal_table;
char *wrtbuffer;

#define min(a,b) (((a) > (b)) ? (b) : (a))

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
void cwrite(char c);
void colork(u8int color);
void cursek(u8int x, u8int y);
void sync();
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
extern int push(int target, addr_t dest, addr_t src, u32int size);
extern int pull(int target, addr_t src, addr_t dest, u32int size);
extern void eout(const char *message);
extern void rirq(u32int irq);
extern int rsig(u32int sig, u32int handler);
extern int lsig(u32int sig);

extern u8int inb(u16int port);
extern void outb(u16int port, u8int val);
extern void outw(u16int port, u16int val);

/***** DRIVERS *****/
void init_cdrv();

/***** GRAPHICS *****/
int vga_init(u32int width, u32int height, int chain4);
int vga_write(int x, int y, u32int buffer, u16int size);
int vga_plot(int x, int y, u8int color, u8int *buffer);
int vga_flip(u32int buffer);

#endif
