/* 
 * Copyright 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <io.h>
#include <arch.h>

/***** File Operations *****/

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

	ret = read(stream->filedes, ptr, size * nmemb, stream->position);
	stream->position += ret;

	return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	const uint8_t *data = ptr;
	size_t i, ret;

	if (stream->flags & FILE_NBF) {
		ret = write(stream->filedes, (void*) ptr, size * nmemb, stream->position);
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

	for (i = 0; i < (size_t) size; i++) {
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
	size_t i, fbt;
	char buffer[13];
	char *fmtbuffer;
	const char *str;
	char c;

	fmtbuffer = malloc(sizeof(char) * strlen(format));
	fbt = 0;

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			fwrite(fmtbuffer, fbt, sizeof(char), stream);
			fbt = 0;

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
				if (str) {
					fwrite(str, strlen(str), sizeof(char), stream);
				}
				else {
					fprintf(stream, "(null)");
				}
				break;
			case 'c':
				c = va_arg(ap, int);
				fwrite(&c, 1, sizeof(char), stream);
				break;
			}
			i++;
		}
		else {
			fmtbuffer[fbt++] = format[i];
		}
	}
	
	fwrite(fmtbuffer, fbt, sizeof(char), stream);
	free(fmtbuffer);

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

int vsprintf(char *str, const char *format, va_list ap) {
	size_t i;
	char m[2], buffer[13];

	strcpy(str, "");
	m[1] = '\0';

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			switch (format[i+1]) {
			case 'x':
			case 'X':
				itoa(buffer, va_arg(ap, int), 16);
				strcat(str, buffer);
				break;
			case 'd':
			case 'i':
				itoa(buffer, va_arg(ap, int), 10);
				strcat(str, buffer);
				break;
			case 'o':
			case 'O':
				itoa(buffer, va_arg(ap, int), 8);
				strcat(str, buffer);
				break;
			case 's':
				strcat(str, va_arg(ap, const char*));
				break;
			case 'c':
				m[0] = va_arg(ap, int);
				strcat(str, m);
			}
			i++;
		}
		else {
			m[0] = format[i];
			strcat(str, m);
		}
	}

	return i;
}

int sprintf(char *str, const char *format, ...) {
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vsprintf(str, format, ap);
	va_end(ap);

	return ret;
}
