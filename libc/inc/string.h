#ifndef STRING_H
#define STRING_H

#include <stdint.h>

/* memory functions ********************************************************/

void  *memchr (const void *, uint8_t, size_t n);
int	   memcmp (const void *, const void *, size_t);

void  *memcpy (void *, const void *, size_t);
void  *memmove(void *, const void *, size_t);

void  *memset (void *, uint8_t, size_t);
void  *memclr (void *, size_t);

/* basic string functions **************************************************/

char  *strcpy (char *, const char *);
char  *strncpy(char *, const char *, size_t);
size_t strlcpy(char *, const char *, size_t);

char  *strcat (char *, const char *);
char  *strncat(char *, const char *, size_t);
size_t strlcat(char *, const char *, size_t);

int    strcmp (const char *, const char *);
int    strncmp(const char *, const char *, size_t);

size_t strlen (const char *);

/* advanced string functions ***********************************************/

char  *strchr (const char *, char);
char  *strrchr(const char *, char);

char  *strstr (const char *, const char *);
char  *strpbrk(const char *, const char *);

size_t strspn (const char *, const char *);		// XXX - not implemented
size_t strcspn(const char *, const char *);		// XXX - not implemented

char  *strtok (char *, const char *); 			// XXX - not implemented
char  *strtok_r(char *, const char *, char **); // XXX - not implemented

int    strcoll(const char *, const char *); 	// XXX - not implemented
size_t strxfrm(char *, const char *, size_t); 	// XXX - not implemented

char  *strerror(int);

#endif
