/*
 * Copyright (C) 2012 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <rho/natio.h>
#include <rho/exec.h>
#include <rho/path.h>

bool is_file(const char *path) {
	int fd;

	fd = ropen(-1, fs_find(path), STAT_READER);

	if (fd < 0) {
		return false;
	}

	if (!rp_type(fd_rp(fd), "file")) {
		close(fd);
		return false;
	}

	close(fd);
	return true;
}

char *path_resolve(const char *file) {
	size_t i;
	char *path;
	char *temp;
	char **paths;

	if (file[0] == '/' || file[0] == '@') {
		path = strdup(file);

		if (is_file(path)) {
			return path;
		}
		else {
			free(path);
			return NULL;
		}
	}
	else {
		paths = strparse(getenv("PATH"), ":");

		for (i = 0; paths[i]; i++) {
			path = strvcat(paths[i], "/", file, NULL);

			temp = path;
			path = path_simplify(temp);
			free(temp);

			if (is_file(path)) {
				for (; paths[i]; i++) {
					free(paths[i]);
				}
				free(paths);
				return path;
			}
			
			free(path);
			free(paths[i]);
		}

		free(paths);
		return NULL;
	}
}

char *ldpath_resolve(const char *soname) {
	size_t i;
	char *path;
	char *temp;
	char **paths;

	if (file[0] == '/' || file[0] == '@') {
		path = strdup(file);

		if (is_file(path)) {
			return path;
		}
		else {
			free(path);
			return NULL;
		}
	}
	else {
		paths = strparse(getenv("LD_LIBRARY_PATH"), ":");

		for (i = 0; paths[i]; i++) {
			path = strvcat(paths[i], "/", file, NULL);

			temp = path;
			path = path_simplify(temp);
			free(temp);

			if (is_file(path)) {
				for (; paths[i]; i++) {
					free(paths[i]);
				}
				free(paths);
				return path;
			}
			
			free(path);
			free(paths[i]);
		}

		free(paths);

		temp = strvcat("/lib/", file, NULL);
		path = path_simplify(temp);
		free(temp);

		if (is_file(path)) {
			return path;
		}

		free(path);
		return NULL;
	}
	return NULL;
}
