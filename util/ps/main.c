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

#include <stdlib.h>
#include <stdio.h>

#include <rho/proc.h>

struct proc_info {
	uint32_t pid;
	uint32_t uid;
	uint32_t gid;
	char *name;
};

struct proc_info *get_proc_info(uint32_t pid) {
	struct proc_info *pi;

	if (getuser(pid) == (uint32_t) -1) {
		return NULL;
	}

	pi = malloc(sizeof(struct proc_info));
	pi->pid  = pid;
	pi->uid  = getuser(pid);
	pi->gid  = getparent(pid);
	pi->name = getname(pid);

	return pi;
}

int main(int argc, char **argv) {
	struct proc_info *pi;
	uint32_t pid;
	int options = 0;

	if (argc > 1) {
		switch (argv[1][0]) {
		case 'e': options = 1; break;
		}
	}

	printf("PID\tUID\tPPID\tCMD\n");
	for (pid = 1; pid < MAX_PID; pid++) {
		pi = get_proc_info(pid);

		if (!pi) continue;

		if (options == 1) {
			printf("%d\t%d\t%d\t%s\n", pi->pid, pi->uid, pi->gid, pi->name);
		}
		else if (pi->uid == getuser(getpid())) {
			printf("%d\t%d\t%d\t%s\n", pi->pid, pi->uid, pi->gid, pi->name);
		}

		free(pi->name);
		free(pi);
	}

	return 0;
}
