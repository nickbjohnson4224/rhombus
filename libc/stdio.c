#include <stdio.h>
#include <stdlib.h>

FILE *stdin = NULL;
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
	return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	return 0;
}
