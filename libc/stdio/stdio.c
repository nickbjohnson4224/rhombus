/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <flux/signal.h>
#include <flux/request.h>
#include <flux/heap.h>

FILE *stdin  = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

/***** File Operations *****/

int fclose(FILE *stream) {

	if (stream->buffer) {
		free(stream->buffer);
	}

	free(stream);

	return 0;
}

FILE *fsetup(uint32_t targ, uint32_t resource, const char *mode) {
	FILE *new = malloc(sizeof(FILE));

	new->file.target   = targ;
	new->file.resource = resource;
	new->position      = 0;
	new->size          = -1;
	new->buffer        = NULL;
	new->buffsize      = 0;
	new->buffpos       = 0;
	new->revbuf        = EOF;
	new->flags         = FILE_NBF | FILE_READ | FILE_WRITE;

	return new;
}

int fflush(FILE *stream) {
	fwrite(stream->buffer, stream->buffpos, sizeof(char), stream);
	stream->buffpos = 0;

	return 0;
}

int setvbuf(FILE *stream, char *buf, int mode, size_t size) {
	
	fflush(stream);

	switch (mode) {
	case _IONBF:
		if (stream->buffer) {
			free(stream->buffer);
			stream->buffer = NULL;
		}
		stream->buffsize = 0;
		break;
	case _IOLBF:
	case _IOFBF:
		if (stream->buffer) {
			free(stream->buffer);
		}
		if (buf) {
			stream->buffer = (uint8_t*) buf;
		}
		else {
			stream->buffer = malloc(size);
		}
		stream->buffsize = size;
		stream->buffpos = 0;
		break;
	default:
		return -1;
	}

	stream->flags &= ~(FILE_FBF | FILE_LBF | FILE_NBF);

	switch (mode) {
	case _IONBF:
		stream->flags |= FILE_NBF;
		break;
	case _IOLBF:
		stream->flags |= FILE_LBF;
		break;
	case _IOFBF:
		stream->flags |= FILE_FBF;
		break;
	}

	return 0;
}

void setbuf(FILE *stream, char *buf) {
	setvbuf(stream, buf, (buf) ? _IOFBF : _IONBF, BUFSIZ);
}

/***** File Control Operations *****/

int fseek(FILE *stream, fpos_t offset, int whence) {
	
	switch (whence) {
	case SEEK_CUR:
		stream->position += offset;
		break;
	case SEEK_END:
		stream->position = stream->size - offset;
		break;
	case SEEK_SET:
		stream->position = offset;
		break;
	}

	return 0;
}

fpos_t ftell(FILE *stream) {
	return stream->position;
}

void rewind(FILE *stream) {
	stream->position = 0;
}

int fgetpos(FILE *stream, fpos_t *pos) {
	*pos = stream->position;

	return 0;
}

int fsetpos(FILE *stream, fpos_t *pos) {
	stream->position = *pos;

	return 0;
}

void clearerr(FILE *stream) {
	stream->flags &= ~(FILE_EOF | FILE_ERROR);
}

int feof(FILE *stream) {
	return (stream->flags & FILE_EOF);
}

int ferror(FILE *stream) {
	return (stream->flags & FILE_ERROR);
}

/***** Standard I/O Operations *****/

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t ret;

	ret = read(&stream->file, ptr, size * nmemb, stream->position);
	stream->position += ret;

	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	const uint8_t *data = ptr;
	size_t i, ret;

	if (stream->flags & FILE_NBF) {
		ret = write(&stream->file, (void*) ptr, size * nmemb, stream->position);
		stream->position += ret;
		return ret;
	}

	for (i = 0; i < size * nmemb; i++) {
		stream->buffer[stream->buffpos++] = data[i];

		if (stream->flags & FILE_LBF) {
			if (data[i] == '\n') fflush(stream);
		}

		if (stream->flags & FILE_FBF) {
			if (stream->buffpos >= stream->buffsize) fflush(stream);
		}
	}

	return (size * nmemb);
}

/***** High Level Input *****/

int fgetc(FILE *stream) {
	char buffer[1];

	if (fread(buffer, 1, sizeof(char), stream) == 0) {
		return EOF;
	}

	return buffer[0];
}

char *fgets(char *s, int size, FILE *stream) {
	size_t i;
	int ch;

	for (i = 0; i < size; i++) {
		ch = fgetc(stream);
		s[i] = ch;

		if (ch == EOF || ch == '\n') break;
	}

	s[i+1] = '\0';
	return s;
}

int ungetc(int c, FILE *stream) {

	if (stream->revbuf != EOF) {
		return -1;;
	}

	stream->revbuf = c;
	return 0;
}

/***** High Level Output *****/

int fputc(int c, FILE *stream) {
	if (fwrite(&c, 1, sizeof(char), stream) == 0) {
		return -1;
	}
	return 0;
}

int fputs(const char *s, FILE *stream) {
	if (fwrite(s, strlen(s), sizeof(char), stream) == 0) {
		return -1;
	}
	return 0;
}

int puts(const char *s) {
	return fputs(s, stdout);
}

static void itoa(char *buffer, unsigned int n, int b) {
	const char d[] = "0123456789ABCDEF";
	char temp;
	size_t i, size;
	
	if (n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	if (b > 16) {
		buffer[0] = '\0';
		return;
	}

/*	if (n < 0) {
		buffer[0] = '-';
		buffer = &buffer[1];
		n = -n;
	} */

	for (i = 0; i < 11 && n; i++) {
		buffer[i] = d[n % b];
		n /= b;
	}

	buffer[i] = '\0';
	size = i;

	for (i = 0; i < (size / 2); i++) {
		temp = buffer[size - i - 1];
		buffer[size - i - 1] = buffer[i];
		buffer[i] = temp;
	}
}

int vfprintf(FILE *stream, const char *format, va_list ap) {
	size_t i;
	char buffer[13];
	const char *str;

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			switch (format[i+1]) {
			case 'x':
			case 'X':
				itoa(buffer, va_arg(ap, int), 16);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'd':
			case 'i':
				itoa(buffer, va_arg(ap, int), 10);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'o':
			case 'O':
				itoa(buffer, va_arg(ap, int), 8);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 's':
				str = va_arg(ap, const char*);
				fwrite(str, strlen(str), sizeof(char), stream);
			}
			i += 2;
		}
		fputc(format[i], stream);
	}

	return 0;
}

int fprintf(FILE *stream, const char *format, ...) {
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vfprintf(stream, format, ap);
	va_end(ap);

	return ret;
}

int vprintf(const char *format, va_list ap) {
	return vfprintf(stdout, format, ap);
}

int printf(const char *format, ...) {
	va_list nv;
	int ret;
	
	va_start(nv, format);
	ret = vfprintf(stdout, format, nv);
	va_end(nv);

	return ret;
}
