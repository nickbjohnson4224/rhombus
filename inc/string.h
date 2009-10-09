#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#define NULL ((void*) 0)

void	*memccpy	(void *, const void *, uint8_t, size_t);
void	*memchr		(const void *, uint8_t, size_t n);
int		memcmp		(const void *, const void *, size_t);
void	*memcpy		(void *, const void *, size_t);
void	*memmove	(void *, const void *, size_t);
void	*memset		(void *, uint8_t, size_t);
void	*memclr		(void *, size_t);
char	*strcat		(char *, const char *);
char	*strchr		(const char *, char);
int		strcmp		(const char *, const char *);
int		strcoll		(const char *, const char *);
char	*strcpy		(char *, const char *);
size_t	strcspn		(const char *, const char *);
char	*strdup		(const char *);
char	*strerror	(int);
size_t	strlen		(const char *);
char	*strncat	(char *, const char *, size_t);
int		strncmp		(const char *, const char *, size_t);
char	*strncpy	(char *, const char *, size_t);
char	*strpbrk	(const char *, const char *);
char	*strrchr	(const char *, int);
size_t	strspn		(const char *, const char *);
char	*strstr		(const char *, const char *);
char	*strtok		(char *, const char *);
char	*strtok_r	(char *, const char *, char **);
size_t	strxfrm		(char *, const char *, size_t);

#endif
