// Copyright 2009 Nick Johnson

#include <lib.h>

// Optimized memory clearing (modified from jgaref's memset)
void *memclr(void *dest, u32int size) {
	u32int num_words, num_bytes, *dest32, i;
	u8int *dest8;

	num_words = size/4;
	num_bytes = size%4;
	dest32 = (u32int*) dest;
	dest8  = ((u8int*) dest) + num_words*4;
	for (i = 0; i < num_words; i++) dest32[i] = 0;
	for (i = 0; i < num_bytes; i++) dest8[i] = 0;
	return dest;
}

// Extremely fast memory clearing for page aligned things
void *pgclr(register u32int *base) {

/* I don't like this unrolled loop... space > time for now */
//	register u32int i;
//	for (i = 0; i < 1024; i += 32) {
//		base[i+0x00] = base[i+0x01] = base[i+0x02] = base[i+0x03] = 0;
//		base[i+0x04] = base[i+0x05] = base[i+0x06] = base[i+0x07] = 0;
//		base[i+0x08] = base[i+0x09] = base[i+0x0A] = base[i+0x0B] = 0;
//		base[i+0x0C] = base[i+0x0D] = base[i+0x0E] = base[i+0x0F] = 0;
//		base[i+0x10] = base[i+0x11] = base[i+0x12] = base[i+0x13] = 0;
//		base[i+0x14] = base[i+0x15] = base[i+0x16] = base[i+0x17] = 0;
//		base[i+0x18] = base[i+0x19] = base[i+0x1A] = base[i+0x1B] = 0;
//		base[i+0x1C] = base[i+0x1D] = base[i+0x1E] = base[i+0x1F] = 0;
//	}
	return memclr(base, 0x1000);
}

char *strcpy(char *dest, char *src) {
	u32int i;
	for (i = 0; src[i]; i++) dest[i] = src[i];
	dest[i] = 0;
	return dest;
}

u32int strlen(char *str) {
	u32int i;
	for (i = 0; str[i]; i++);
	return i;
}

u32int atoi(char *str, u8int base) {
	int n = 0, i, neg;
	if (str[0] == '-') neg = 1;
	else neg = 0;
	for (i = neg; str[i]; i++) {
		if (str[i] > '9' || str[i] < '0') break;
		n *= base;
		n += str[i] - '0';
	}
	return n;
}

int strcmp(char *s1, char *s2) {
	while (*s1 == *s2 && *s1 && *s2) {
		s1++;
		s2++;
	}
	if (*s1 == *s2) return 0;
	return 1;
}
	

char *itoa(u32int n, char *buf, u8int base) {
	char *p1, *p2;
	static char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i = 0;
	if (base > 36 || base < 2) return 0;
	do buf[i++] = digits[n % base];
	while ((n /= base) > 0);
	buf[i] = '\0';
	for (p1 = buf, p2 = buf + strlen(buf) - 1; p2 > p1; ++p1, --p2) {
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return buf;
}
