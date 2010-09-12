/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <arch.h>

/*** Type Definitions ***/

typedef uint64_t fpos_t;

/*** File Structure ***/

struct _file_ext {
	bool mutex;				/* Mutex for buffers/position */

	fpos_t position;		/* File position */
	fpos_t size;			/* File size */

	uint8_t *buffer;		/* Stream buffer */
	size_t buffpos;			/* Position in stream buffer */
	size_t buffsize;		/* Size of stream buffer */

	int revbuf;				/* ungetc() reverse buffer */

	uint8_t flags;			/* EOF/Error/various flags */
};

typedef struct _file {
	uint32_t server;		/* Server PID */
	uint32_t inode;			/* File inode on server */
	
	struct _file_ext *ext;	/* Extensions for stream operations */
} FILE;

#define FILE_EOF	0x01	/* End Of File */
#define FILE_ERROR	0x02	/* File Error */
#define FILE_FBF	0x04	/* Fully Buffered */
#define FILE_LBF	0x08	/* Line Buffered */
#define FILE_NBF	0x10	/* Not Buffered */
#define FILE_READ	0x20	/* Read Access */
#define FILE_WRITE	0x40	/* Write Access */

/* constants ***************************************************************/

#define EOF 		(-1)

#define SEEK_CUR	0
#define SEEK_END	1
#define SEEK_SET	2

#define _IONBF		0
#define _IOLBF		1
#define _IOFBF		2

#define BUFSIZ		(PAGESZ - 256)

#define TMP_MAX		0xFFFFFFFF
#define L_tmpnam	24

/* standard streams ********************************************************/

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

/* file operations *********************************************************/

int   remove (const char *path);	// XXX - not implemented
int   rename (const char *oldpath, const char *newpath);	// XXX - n impl
FILE *tmpfile(void);
char *tmpnam (char *s);

/* file access *************************************************************/

FILE *__fload(const char *name);
int   __fsave(const char *name, FILE *fd);
FILE *__fcons(uint32_t server, uint32_t inode, FILE *stream);
FILE *__fstrip(FILE *stream);
FILE *__fsetup(FILE *stream);

int   fclose (FILE *stream);
FILE *fopen  (const char *path, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

int  fflush  (FILE *stream);
int  setvbuf (FILE *stream, char *buf, int mode, size_t size);
void setbuf  (FILE *stream, char *buf);

/* direct input/output *****************************************************/

size_t fread (void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/* character input/output **************************************************/

int   fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int   ungetc(int c, FILE *stream);

#define getc fgetc
#define getchar() (fgetc(stdin))

int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int puts(const char *s);

#define putc fputc
#define putchar(c) (fputc(c, stdout))
#define fputchar(c) (fputc(c, stdout))

/* formatted input/output **************************************************/

int __utoa(char *buffer, unsigned int n, int base, bool ucase);
int __itoa(char *buffer, int n, int base, bool ucase);
int __ftoa(char *buffer, double n, int precision);
int __etoa(char *buffer, double n, int precision);

int printf   (const char *format, ...);
int vprintf  (const char *format, va_list ap);
int fprintf  (FILE *stream, const char *format, ...);
int vfprintf (FILE *stream, const char *format, va_list ap);
int sprintf  (char *str, const char *format, ...);
int vsprintf (char *str, const char *format, va_list ap);

int scanf    (const char *format, ...);
int vscanf   (const char *format, va_list ap);
int fscanf   (FILE *stream, const char *format, ...);
int vfscanf  (FILE *stream, const char *format, va_list ap);
int sscanf   (const char *str, const char *format, ...);
int vsscanf  (const char *str, const char *format, va_list ap);

/* file positioning ********************************************************/

int    fseek  (FILE *stream, fpos_t offset, int whence);
fpos_t ftell  (FILE *stream);
void   rewind (FILE *stream);
int    fgetpos(FILE *stream, fpos_t *pos);
int    fsetpos(FILE *stream, fpos_t *pos);

/* error handling **********************************************************/

void   clearerr(FILE *stream);
int    feof    (FILE *stream);
int    ferror  (FILE *stream);
void   perror  (const char *s);

#endif/*STDIO_H*/
