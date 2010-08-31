/* Copyright 2009, 2010 Nick Johnson */

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>
#include <arch.h>

/***** I/O, ETC. *****/
void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a < b) ? a : b)
void sleep(uint32_t cycles);

/***** STRING FUNCTIONS *****/
void *memcpy(void *dest, void *src, uint32_t size);
void *memset(void *dest, uint8_t src, uint32_t size);
void *memclr(void *dest, uint32_t size);
void *pgclr(uint32_t *base);
char *strcpy(char *dest, char *src);
int strcmp(char *s1, char *s2);
uint32_t strlen(char *str);
int atoi(char *str, uint8_t base);
char *itoa(uint32_t n, char *buf, uint8_t base);

#define ERROR	((uint32_t) -1)
#define ENOTASK ((uint32_t) -1)
#define ENOSIG  ((uint32_t) -2)
#define EPERMIT ((uint32_t) -3)
#define EREPEAT ((uint32_t) -4)

#endif/*UTIL_H*/
