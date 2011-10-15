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

#include <struct.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <event.h>
#include <natio.h>
#include <mutex.h>
#include <proc.h>
#include <abi.h>

#include <robject.h>

/*
 * various field manipulations
 */

void robject_set_call(struct robject *ro, const char *call, rcall_t hook, int status) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->call_table = s_table_set(ro->call_table, call, (void*) (uintptr_t) hook);
		ro->call_stat_table = s_table_set(ro->call_stat_table, call, (void*) status);
		mutex_free(&ro->mutex);
	}
}

rcall_t robject_get_call(struct robject *ro, const char *call) {
	rcall_t hook;
	
	if (ro) {
		mutex_spin(&ro->mutex);

		// read hook
		hook = (rcall_t) (uintptr_t) s_table_get(ro->call_table, call);

		// if not defined, fall back to parent
		if (!hook) hook = robject_get_call(ro->parent, call);

		mutex_free(&ro->mutex);

		return hook;
	}
	else {
		return NULL;
	}
}

void robject_set_data(struct robject *ro, const char *field, void *data) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->data_table = s_table_set(ro->data_table, field, data);
		mutex_free(&ro->mutex);
	}
}

void *robject_get_data(struct robject *ro, const char *field) {
	void *data;

	if (ro) {
		mutex_spin(&ro->mutex);
		data = s_table_get(ro->data_table, field);
		mutex_free(&ro->mutex);

		return data;
	}
	else {
		return NULL;
	}
}

/*
 * basic interface
 */

static void _iter(void *arg0, const char *key, void *value) {
	if ((int) value & STAT_EVENT) event(ator(key), arg0);
}

void robject_event(struct robject *ro, const char *event) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		s_table_iter(ro->open_table, (void*) event, _iter);
		mutex_free(&ro->mutex);
	}
}

char *robject_call(struct robject *ro, rp_t source, const char *args) {
	rcall_t call;
	int argc;
	int status;
	char **argv;
	char *rets;

	// parse argument list
	argv = strparse(args, " ");
	if (!argv) return NULL;

	for (argc = 0; argv[argc]; argc++);

	call = robject_get_call(ro, argv[0]);

	if (!call) {
		for (argc = 0; argv[argc]; argc++) free(argv[argc]);
		free(argv);

		return NULL;
	}

	// check access and status restrictions
	mutex_spin(&ro->mutex);
	status = (int) s_table_get(ro->call_stat_table, argv[0]);
	mutex_free(&ro->mutex);

	if (status) {
		if (!robject_check_status(ro, source, status) 
			&& !robject_check_status(ro, RP_HEAD(source), status)) {
			return strdup("! denied");
		}
	}

	rets = call(ro, source, argc, argv);

	for (argc = 0; argv[argc]; argc++) free(argv[argc]);
	free(argv);

	return rets;
}

void *robject_data(struct robject *ro, const char *field) {
	return robject_get_data(ro, field);
}

int robject_open(struct robject *ro, rp_t source, int status) {

	if (ro) {
		mutex_spin(&ro->mutex);
		ro->open_table = s_table_setv(ro->open_table, (void*) status, "%r", source);
		mutex_free(&ro->mutex);
	}

	return 0;
}

int robject_stat(struct robject *ro, rp_t source) {
	int status = 0;

	if (ro) {
		mutex_spin(&ro->mutex);
		status = (int) s_table_getv(ro->open_table, "%r", source);
		mutex_free(&ro->mutex);
	}

	return status;
}

int robject_check_status(struct robject *ro, rp_t source, int status) {
	return ((status & robject_stat(ro, source)) == status);
}

void robject_close(struct robject *ro, rp_t source) {

	if (ro) {
		mutex_spin(&ro->mutex);
		ro->open_table = s_table_setv(ro->open_table, NULL, "%r", source);
		mutex_free(&ro->mutex);
	}
}

int robject_get_access(struct robject *ro, rp_t source) {
	int access_level = 0;

	if (ro) {
		mutex_spin(&ro->mutex);
		access_level = (int) s_table_getv(ro->accs_table, "uid-%u", getuser(source));
		if ((access_level & 0x100) == 0) {
			access_level = (int) s_table_get(ro->accs_table, "default");
		}
		access_level &= ~0x100;
		mutex_free(&ro->mutex);
	}

	return access_level;
}

int robject_check_access(struct robject *ro, rp_t source, int access) {
	return ((access & robject_get_access(ro, source)) == access);
}

void robject_set_access(struct robject *ro, rp_t source, int access) {
	
	access |= 0x100;

	if (ro) {
		mutex_spin(&ro->mutex);
		ro->accs_table = s_table_setv(ro->accs_table, (void*) access, "uid-%u", getuser(source));
		mutex_free(&ro->mutex);
	}
}

void robject_set_default_access(struct robject *ro, int access) {
	
	if (ro) {
		mutex_spin(&ro->mutex);
		ro->accs_table = s_table_set(ro->accs_table, "default", (void*) access);
		mutex_free(&ro->mutex);
	}
}
