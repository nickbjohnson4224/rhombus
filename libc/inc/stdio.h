/*
 * Copyright (C) 2009-2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

/* type definitions *********************************************************/

typedef uint64_t fpos_t;

/* file structure ***********************************************************/

typedef struct _file {
	uint64_t fd;			/* File descriptor */

	bool mutex;				/* Mutex for buffers/position */

	fpos_t position;		/* File position */
	fpos_t size;			/* File size */

	uint8_t *buffer;		/* Stream buffer */
	size_t buffpos;			/* Position in stream buffer */
	size_t buffsize;		/* Size of stream buffer */

	int revbuf;				/* ungetc() reverse buffer */

	uint8_t flags;			/* EOF/Error/various flags */
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

int   remove (const char *path);
int   rename (const char *oldpath, const char *newpath);
FILE *tmpfile(void);
char *tmpnam (char *s);

/* file access *************************************************************/

int   fclose (FILE *stream);
FILE *fopen  (const char *path, const char *mode);
FILE *fdopen (uint64_t fd, const char *mode);
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

char *__format(const char **fmt, va_list *argp);

int __parse_format(const char **format, int *width, int *precision);

#define FMT_FLAG_SIGN	0x00000001
#define FMT_FLAG_LEFT	0x00000002
#define FMT_FLAG_ALT	0x00000004
#define FMT_FLAG_FILL	0x00000008
#define FMT_FLAG_WIDTH	0x00000010
#define FMT_FLAG_PREC	0x00000020
#define FMT_FLAG_UCASE	0x00000040

#define FMT_LEN(n) (((n) >> 8) & 0xFF)
#define FMT_LEN_INT		0x00000000
#define FMT_LEN_BYTE	0x00000100
#define FMT_LEN_SHORT	0x00000200
#define FMT_LEN_LONG	0x00000300
#define FMT_LEN_LLONG	0x00000400
#define FMT_LEN_LDBL	0x00000500

#define FMT_TYPE_INT	0x00010000
#define FMT_TYPE_UINT	0x00020000
#define FMT_TYPE_FLT	0x00040000
#define FMT_TYPE_CHAR	0x00080000
#define FMT_TYPE_STR	0x00100000
#define FMT_TYPE_DEC	0x00200000
#define FMT_TYPE_OCT	0x00400000
#define FMT_TYPE_HEX	0x00800000
#define FMT_TYPE_PTR	0x01000000
#define FMT_TYPE_COUNT	0x02000000
#define FMT_TYPE_LIT	0x08000000
#define FMT_TYPE_EXP	0x10000000

size_t __fformat(FILE *stream, int flags, int width, int precision, void *datum);
size_t __sformat(char *buffer, int flags, int width, int precision, void *datum, size_t size);

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
