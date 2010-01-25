/* Copyright 2010 Nick Johnson */

#include <stdio.h>
#include <stdlib.h>
#include <flux.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>

FILE *stdin  = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

int fclose(FILE *stream) {
	if (stream->buffer) {
		free(stream->buffer);
	}
	free(stream);

	return 0;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	struct request *req, *res;
	uint8_t *data = (void*) ptr;
	uint16_t datasize;
	size_t oldsize, i = 0;

	size *= nmemb;
	oldsize = size;

	sighold(SIG_REPLY);

	req = req_alloc();

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req->resource = stream->resource;
		req->datasize = datasize;
		req->transid  = i;
		req->format   = REQ_READ;
		req->fileoff[0] = stream->position;
		req->fileoff[1] = 0;
		req->fileoff[2] = 0;
		req->fileoff[3] = 0;

		fire(stream->target, stream->rport, req_checksum(req));

		res = sigpull(SIG_REPLY);

		memcpy(data, res->reqdata, res->datasize);

		data = &data[res->datasize];
		size -= res->datasize;
		req_free(res);

		i++;
	}

	req_free(req);
	sigfree(SIG_REPLY);

	return oldsize;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	struct request *req, *res;
	uint8_t *data = (void*) ptr;
	uint16_t datasize;
	size_t oldsize, i = 0;

	size *= nmemb;
	oldsize = size;

	sighold(SIG_REPLY);

	req = req_alloc();

	while (size) {
		datasize = (size > REQSZ) ? REQSZ : size;

		req->resource = stream->resource;
		req->datasize = datasize;
		req->transid  = i;
		req->dataoff  = 0;
		req->format   = REQ_WRITE;
		req->fileoff[0] = stream->position;
		req->fileoff[1] = 0;
		req->fileoff[2] = 0;
		req->fileoff[3] = 0;

		memcpy(req->reqdata, data, datasize);

		fire(stream->target, stream->wport, req_checksum(req));

		res = sigpull(SIG_REPLY);

		data = &data[res->datasize];
		size -= res->datasize;
		req_free(res);
		i++;
	}

	req_free(req);

	sigfree(SIG_REPLY);

	return oldsize;
}

int fputc(int c, FILE *stream) {
	fwrite(&c, 1, sizeof(char), stream);
	return 0;
}

int fgetc(FILE *stream) {
	char buffer[1];

	fread(buffer, 1, sizeof(char), stream);

	return buffer[0];
}

static void itoa(char *buffer, int n, int b) {
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

	if (n < 0) {
		buffer[0] = '-';
		buffer = &buffer[1];
		n = -n;
	}

	for (i = 0; i < 11 && n; i++) {
		buffer[i] = d[n % b];
		n /= b;
	}

	buffer[i] = '\0';
	size = i;

	for (i = 0; i < (size / 2); i++) {
		temp = buffer[size - i];
		buffer[size - i] = buffer[i];
		buffer[i] = temp;
	}
}

int fprintf(FILE *stream, const char *format, ...) {
	va_list nv;
	size_t i;
	char buffer[12];
	const char *str;
	
	va_start(nv, format);

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			switch (format[i+1]) {
			case 'x':
			case 'X':
				itoa(buffer, va_arg(nv, int), 16);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'd':
			case 'i':
				itoa(buffer, va_arg(nv, int), 10);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 'o':
			case 'O':
				itoa(buffer, va_arg(nv, int), 8);
				fwrite(buffer, strlen(buffer), sizeof(char), stream);
				break;
			case 's':
				str = va_arg(nv, const char*);
				fwrite(str, strlen(str), sizeof(char), stream);
			}
			i += 2;
		}
		fputc(format[i], stream);
	}

	va_end(nv);

	return 0;
}

int printf(const char *format, ...) {
	va_list nv;
	size_t i;
	char buffer[12];
	const char *str;
	
	va_start(nv, format);

	for (i = 0; format[i]; i++) {
		if (format[i] == '%') {
			switch (format[i+1]) {
			case 'x':
			case 'X':
				itoa(buffer, va_arg(nv, int), 16);
				fwrite(buffer, strlen(buffer), sizeof(char), stdout);
				break;
			case 'd':
			case 'i':
				itoa(buffer, va_arg(nv, int), 10);
				fwrite(buffer, strlen(buffer), sizeof(char), stdout);
				break;
			case 'o':
			case 'O':
				itoa(buffer, va_arg(nv, int), 8);
				fwrite(buffer, strlen(buffer), sizeof(char), stdout);
				break;
			case 's':
				str = va_arg(nv, const char*);
				fwrite(str, strlen(str), sizeof(char), stdout);
			}
			i += 2;
		}
		fputc(format[i], stdout);
	}

	va_end(nv);

	return 0;
}
