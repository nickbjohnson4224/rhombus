/*
 * Copyright (C) 2009, 2010 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
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

#include <rho/mutex.h>

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
			break;
		}
	}

	if (!done) {
		if (value) {
			i = environ_size;
			environ_size++;
			environ = realloc(environ, sizeof(char*) * (environ_size + 1));
			environ[environ_size] = NULL;
			
			if (!environ) {
				errno = ENOMEM;
				err = 1;
			}
			else {
				environ[i] = NULL;
			}
		}
		else {
			err = 0;
		}
	}

	if (value) {
		old = environ[i];
		new = malloc(strlen(name) + 1 + strlen(value) + 1);
		sprintf(new, "%s=%s", name, value);
		*strchr(new, '=') = '\0';
		environ[i] = new;
		if (old) free(old);
	}
	else {
		free(environ[i]);
		environ[i] = strdup("\0");
	}

	mutex_free(&m_environ);
	
	return err;
}

/****************************************************************************
 * packenv
 *
 * Converts the environment variable vector <envp> into a flat environment
 * variable vector. If <envp> is null, the current environment is packed
 * instead. The returned pointer is page aligned.
 *
 * The format of the returned vector is this: each pair is separated by a
 * null character, as well as the pairs from each other (parity is used to
 * figure out what goes where); the entire vector is terminated by a single
 * backspace character.
 */

char *packenv(const char **envp) {
	char *pack, *top;
	size_t i, size;

	if (!envp) {
		return NULL;
	}

	size = 1;
	for (i = 0; envp[i]; i++) {
		while (envp[i][0] == '\0') i++;
		size += strlen(envp[i]) + 1;
		size += strlen(&envp[i][strlen(envp[i]) + 1]) + 1;
	}

	pack = aalloc(size, PAGESZ);

	top = pack;
	for (i = 0; envp[i]; i++) {
		while (envp[i][0] == '\0') i++;
		strcpy(top, envp[i]);
		top += strlen(top) + 1;
		strcpy(top, &envp[i][strlen(envp[i]) + 1]);
		top += strlen(top) + 1;
	}
	top[0] = '\b';

	return pack;
}

/****************************************************************************
 * loadenv
 *
 * Unpacks the flat environment variable vector <pack> into the current
 * environment.
 */

void loadenv(const char *pack) {
	const char *top_key, *top_val;

	if (!pack) {
		return;
	}

	top_key = pack;
	while (1) {
		if (top_key[0] == '\b') {
			break;
		}

		top_val = &top_key[strlen(top_key) + 1];
		setenv(top_key, top_val);
		top_key = &top_val[strlen(top_val) + 1];
	}
}
