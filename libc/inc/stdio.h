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

#include <stdint.h>
#include <stdbool.h>

/*** Type Definitions ***/

typedef size_t fpos_t;

/*** File Structure ***/

typedef struct _file {
	uint32_t server;		/* Server PID */
	uint64_t inode;			/* File inode on server */
	uint8_t naddr[16];		/* Network address of server node */

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

/* standard streams ********************************************************/

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
extern FILE *stdvfs;

/* file operations *********************************************************/

int   fclose(FILE *stream);
FILE *fopen(const char *path, const char *mode);

FILE *fload(const char *name);
int   fsave(const char *name, FILE *fd);
FILE *fcons(uint32_t server, uint64_t inode);

int  fflush(FILE *stream);
int  setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);

/* I/O operations **********************************************************/

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

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

int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int sprintf(char *str, const char *format, ...);

/* file control operations *************************************************/

int    fseek(FILE *stream, fpos_t offset, int whence);
fpos_t ftell(FILE *stream);
void   rewind(FILE *stream);
int    fgetpos(FILE *stream, fpos_t *pos);
int    fsetpos(FILE *stream, fpos_t *pos);

void   clearerr(FILE *stream);
int    feof(FILE *stream);
int    ferror(FILE *stream);

/* native I/O routines *****************************************************/

size_t ssend(FILE *file, void *r, void *s, size_t size, uint64_t off, uint8_t port);
size_t read (FILE *file, void *buf, size_t size, uint64_t offset);
size_t write(FILE *file, void *buf, size_t size, uint64_t offset);
size_t query(FILE *file, void *rbuf, void *sbuf, size_t size);
bool   info (FILE *file, char *value, const char *field);
bool   ctrl (FILE *file, char *value, const char *field);

#define VFS_CMD_FIND  0
#define VFS_CMD_ADD   1
#define VFS_CMD_LIST  2
#define VFS_CMD_LINK  3

#define VFS_CMD_REPLY 10
#define VFS_CMD_ERROR 11

struct vfs_query {
	uint32_t command;
	uint32_t server;
	uint64_t inode;
	uint8_t  naddr[16];
	char path0[1000];
	char path1[1000];
};

int find(const char *path, uint32_t *server, uint64_t *inode);
int fadd(const char *path, uint32_t server, uint64_t inode);
int list(const char *path, char *buffer);

struct info_query {
	char field[100];
	char value[1000];
};

#endif/*STDIO_H*/
