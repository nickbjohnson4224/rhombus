/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
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

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	uint32_t pid;
	int signum;

	if (argc < 2) {
		fprintf(stderr, "%s: pid not supplied", argv[0]);
		return 1;
	}

	if (argc == 3) {
		signum = atoi(&argv[1][1]);
		pid = atoi(argv[2]);
	}
	else {
		signum = SIGTERM;
		pid = atoi(argv[1]);
	}

	if (kill(pid, signum)) {
		printf("Error sending signal.\n");
		return 1;
	}

	if (signum == SIGNUKE) {
		if (pid > 0) {
			printf("Nuked PID %d.\n", pid);
		}
		else {
			printf("Nuked PID %d and children.\n", -pid);
		}
	}

	return 0;
}
