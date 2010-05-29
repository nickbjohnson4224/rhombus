#ifndef STDIO_H
#define STDIO_H

#include <flux/io.h>

/*** Type Definitions ***/

#include <stdint.h>
typedef size_t fpos_t;

/*** File Structure ***/

typedef struct _file {
	int filedes;			/* File descriptor */	

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

/*** Constants ***/

#define EOF 		(-1)

#define SEEK_CUR	0
#define SEEK_END	1
#define SEEK_SET	2

#define _IONBF		0
#define _IOLBF		1
#define _IOFBF		2

#define BUFSIZ		(PAGESZ - 256)

/*** Standard Streams ***/

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
extern FILE *stdvfs;
extern FILE *stddev;
extern FILE *stdpmd;
extern FILE *extin;
extern FILE *extout;

/*** File Operations ***/

int   fclose(FILE *stream);
FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);

int  fflush(FILE *stream);
int  setvbuf(FILE *stream, char *buf, int mode, size_t size);
void setbuf(FILE *stream, char *buf);

/*** I/O Operations ***/

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

/*** File Control Operations ***/

int    fseek(FILE *stream, fpos_t offset, int whence);
fpos_t ftell(FILE *stream);
void   rewind(FILE *stream);
int    fgetpos(FILE *stream, fpos_t *pos);
int    fsetpos(FILE *stream, fpos_t *pos);

void   clearerr(FILE *stream);
int    feof(FILE *stream);
int    ferror(FILE *stream);

#endif
