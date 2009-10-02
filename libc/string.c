#include <string.h>

void *memccpy (void *d, const void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *dst = (uint8_t*) d; 
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
		if (src[i] == c) {
			return &dst[i+1];
		}
	}

	return NULL;
}

void *memchr (const void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		if (src[n] == c) return &src[n];
	}
	
	return NULL;
}

int		memcmp		(const void *, const void *, size_t);

void *memcpy (void *d, const void *s, size_t n) {
	size_t i;
	uint8_t *dst = (uint8_t*) d;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		dst[i] = src[i];
	}

	return dst;
}

void *memmove (void *d, const void *s, size_t n) {
	size_t i;
	uint8_t *dst = (uint8_t*) d;
	uint8_t *src = (uint8_t*) s;

	if (src == dst) return dst;
	if (src  > dst) {
		for (i = 0; i < n; i++) {
			dst[i] = src[i];
		}
	}
	if (src  < dst) {
		for (i = 0; i < n; i++) {
			dst[n - i - 1] = src[n - i - 1];
		}
	}

	return dst;
} 

void *memset (void *s, uint8_t c, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		src[i] = c;
	}

	return src;
}

void *memclr (void *s, size_t n) {
	size_t i;
	uint8_t *src = (uint8_t*) s;

	for (i = 0; i < n; i++) {
		src[i] = 0;
	}

	return src;
}

char *strcat (char *d, const char *s) {
	size_t i, e;

	for (e = 0; d[e] != '\0'; e++);

	for (i = 0; s[i] != '\0'; i++) {
		d[e + i] = s[i];
	}

	d[e + i] = '\0';

	return d;
}

char *strchr (const char *s, char c) {
	size_t i;

	for (i = 0; s[i] != c; i++);

	return (char*) &s[i];
}

int strcmp (const char *s1, const char *s2) {
	size_t i;

	for (i = 0;; i++) {
		if (s1[i] == s2[i]) {
			if (s1[i] == '\0') return 0;
			continue;
		}
		if (s1[i] == '\0') return -1;
		if (s2[i] == '\0') return 1;
		if (s1[i] < s2[i]) return -1;
		else return 1;
	}
}

char *strcpy (char *d, const char *s) {
	size_t i;

	for (i = 0; s[i] != '\0'; i++) {
		d[i] = s[i];
	}

	return d;
}

size_t	strcspn		(const char *, const char *);
char	*strdup		(const char *);
char	*strerror	(int);

size_t strlen (const char *s) {
	size_t i;

	for (i = 0; s[i] != '\0'; i++);

	return i;
}

char *strncat (char *d, const char *s, size_t n) {
	size_t i, e;

	for (e = 0; d[e] != '\0'; e++);

	for (i = 0; s[i] != '\0' && i < n; i++) {
		d[e + i] = s[i];
	}

	d[e + i] = '\0';

	return d;
}

int	strncmp (const char *s1, const char *s2, size_t n) {
	size_t i;

	for (i = 0; i < n; i++) {
		if (s1[i] == s2[i]) {
			if (s1[i] == '\0') return 0;
			continue;
		}
		if (s1[i] == '\0') return -1;
		if (s2[i] == '\0') return 1;
		if (s1[i] < s2[i]) return -1;
		else return 1;
	}

	return 0;
}

char *strncpy (char *d, const char *s, size_t n) {
	size_t i;

	for (i = 0; s[i] != '\0' && i < n; i++) {
		d[i] = s[i];
	}

	return d;
}

char	*strpbrk	(const char *, const char *);
char	*strrchr	(const char *, int);
size_t	strspn		(const char *, const char *);
char	*strstr		(const char *, const char *);
char	*strtok		(char *, const char *);
char	*strtok_r	(char *, const char *, char **);
size_t	strxfrm		(char *, const char *, size_t);
