#ifndef KERNEL_PRINT_H
#define KERNEL_PRINT_H

void cleark(void);
void printk(const char *fmt, ...);
void colork(uint8_t color);
void cursek(int8_t x, int8_t y);
void panic(const char *message);

#endif
