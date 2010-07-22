/*
 * Copyright 2010 Nick Johnson
 * ISC Licensed, see LICENSE for details
 */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <mutex.h>

/* Memory allocation *******************************************************/

void   *calloc(size_t nmemb, size_t size);
void   *valloc(size_t size);
void   *malloc(size_t size);
void   *aalloc(size_t size, size_t align);
size_t msize(void *ptr);
void   free(void *ptr);
void   *realloc(void *ptr, size_t size);

/* Type conversion *********************************************************/

int      atoi(const char *nptr);
double   atof(const char *nptr);
int32_t  atol(const char *nptr);
double   strtod(const char *nptr);
int32_t  strtol(const char *nptr);
uint32_t strtoul(const char *nptr);

/* Random number generaton *************************************************/

#define RAND_MAX UINT32_MAX

void    srand(uint32_t seed);
uint32_t rand(void);

/* Environment variables and system ****************************************/

char *getenv(const char *name);
int   setenv(const char *name, const char *value);

int   system(const char *s);

/* Process Control *********************************************************/

#define EXIT_FAILURE	1
#define EXIT_SUCCESS	0

void exit(int status);
void abort(void);
int  atexit(void (*function)(void));

/* Math functions **********************************************************/

typedef struct {
	short int quot;
	short int rem;
} div_t;

typedef struct {
	long int quot;
	long int rem;
} ldiv_t;

short int abs(short int n);
long int labs(long int n);

div_t   div(short int num, short int denom);
ldiv_t ldiv(long int num,  long int denom);

/* Searching and sorting ***************************************************/

void *bsearch(const void *key, const void *base, size_t n, size_t size,
	int (*cmp)(const void *keyval, const void *datum));

void qsort(void *base, size_t n, size_t size, 
	int (*cmp)(const void *keyval, const void *datum));

#endif/*STDLIB_H*/
