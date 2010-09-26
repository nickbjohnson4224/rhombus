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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <mutex.h>
#include <pack.h>

/****************************************************************************
 * environ
 *
 * Array of environment variable strings. Strings are of the form 
 * "name\0value", and the array is NULL terminated.
 */

char **environ;
size_t environ_size;
bool m_environ;

/****************************************************************************
 * getenv
 *
 * Searches the current environment for the key <name>. Returns the found
 * value on success, or NULL on failure. This function is thread-safe.
 */

const char *getenv(const char *name) {
	size_t i;
	const char *value;

	mutex_spin(&m_environ);

	value = NULL;

	if (environ) {
		for (i = 0; i < environ_size; i++) {
			if (!strcmp(environ[i], name)) {
				value = &environ[i][strlen(environ[i]) + 1];
				break;
			}
		}
	}

	mutex_free(&m_environ);

	return value;
}

/****************************************************************************
 * setenv
 *
 * Sets the value of the environment key <key> to the value <value>. If value
 * is NULL, the key <key> is deleted from the environment. Returns 0 on 
 * success, or nonzero on failure. This function is thread-safe.
 */

int setenv(const char *name, const char *value) {
	size_t i;
	int err = 0;
	char *old;
	char *new;
	bool done = false;

	mutex_spin(&m_environ);

	for (i = 0; i < environ_size; i++) {
		if (!strcmp(environ[i], name)) {
			done = true;
		}
	}

	if (!done){
		if (value) {
			i = environ_size;
			environ_size++;
			environ = realloc(environ, sizeof(char*) * environ_size);
			environ[i] = NULL;
		}
		else {
			return 0;
		}
	}

	if (!environ) {
		errno = ENOMEM;
		err = 1;
	}
	else if (value) {
		old = environ[i];
		new = malloc(strlen(name) + 1 + strlen(value) + 1);
		sprintf(new, "%s=%s", name, value);
		*strchr(new, '=') = '\0';
		environ[i] = new;
	}
	else {
		free(environ[i]);
		environ[i] = "\0";
	}

	mutex_free(&m_environ);
	
	return err;
}

/****************************************************************************
 * __saveenv
 *
 * Packs all environment variables into exec-persistent space.
 */

void __saveenv(void) {
	size_t i, length;

	mutex_spin(&m_environ);

	for (i = 0; i < environ_size; i++) {
		length =  strlen(environ[i]) + 1;
		length += strlen(&environ[i][length]) + 1;
		__pack_add(PACK_KEY_ENV | i, environ[i], length);
	}

	mutex_free(&m_environ);
}

/****************************************************************************
 * __loadenv
 *
 * Loads all environment variables from exec-persistent space.
 */

void __loadenv(void) {
	const char *value;
	size_t length;
	size_t i;

	for (i = 0;; i++) {
		value = __pack_load(PACK_KEY_ENV | i, &length);

		if (!value) {
			break;
		}

		printf("unpacked: %s=%s\n", value, &value[strlen(value) + 1]);

		setenv(value, &value[strlen(value) + 1]);
	}
}
