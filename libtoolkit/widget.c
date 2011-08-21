/*
 * Copyright (C) 2011 Jaagup Repän <jrepan at gmail.com>
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

#include "widget.h"
#include <graph.h>
#include <natio.h>
#include <mutex.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "window.h"
#include "private.h"

int __rtk_call_lua_function(lua_State *L, int args, int ret) {
	if (lua_pcall(L, args, ret, 0)) {
		lua_pop(L, -1);
		return 1;
	}
	return 0;
}

struct widget *__rtk_add_widget(const char *name, struct widget *parent, struct window *window, int x, int y, int w, int h) {
	struct widget *widget = malloc(sizeof(struct widget));
	char *filename;

	if (!widget) {
		return NULL;
	}

	widget->window = window;
	widget->parent = parent;
	widget->children = NULL;
	widget->prev = widget->next = NULL;
	widget->width = w;
	widget->height = h;
	widget->name = NULL;
	widget->fonts = NULL;
	set_position(widget, x, y);

	widget->L = lua_open();
	luaL_openlibs(widget->L);
	lua_pushlightuserdata(widget->L, widget);
	lua_rawseti(widget->L, LUA_REGISTRYINDEX, 1);
	__rtk_init_library(widget->L);
	__rtk_init_drawing_functions(widget->L);
	__rtk_init_image_functions(widget->L);

	filename = saprintf("/etc/widgets/%s.lua", name);
	if (luaL_loadfile(widget->L, filename)) {
		lua_close(widget->L);
		free(widget);
		free(filename);
		return NULL;
	}
	free(filename);

	if (parent) {
		if (parent->children) {
			parent->children->prev = widget;
		}
		widget->next = parent->children;
		parent->children = widget;
	}

	if (__rtk_call_lua_function(widget->L, 0, 0)) {
		__rtk_free_widget(widget);
		return NULL;
	}

	set_size(widget, w, h);

	return widget;
}

void __rtk_free_widget(struct widget *widget) {
	struct widget *ptr;
	
	for (ptr = widget->children; ptr; ptr = ptr->next) {
		__rtk_free_widget(ptr);
	}

	if (widget->parent) {
		widget->parent->children = widget->next;
	}
	if (widget->next) {
		widget->next->prev = widget->prev;
	}
	if (widget->prev) {
		widget->prev->next = widget->next;
	}

	lua_close(widget->L);
	free(widget->name);
	if (widget->fonts) {
		__rtk_free_font(widget->fonts);
	}
	free(widget);
}

int __rtk_draw_widget(struct widget *widget, bool force) {
	struct widget *ptr;

	if (force || widget->dirty) {
		widget_call(widget, "draw", NULL);
	}

	if (force || widget->child_dirty) {
		for (ptr = widget->children; ptr; ptr = ptr->next) {
			if (force || ptr->dirty || ptr->child_dirty) {
				__rtk_draw_widget(ptr, force);
			}
		}
	}

	widget->dirty = false;
	widget->child_dirty = false;

	return 0;
}

void __rtk_update_widget(struct widget *widget) {
	struct widget *ptr;
	int parent_width, parent_height;

	if (widget->parent) {
		widget->realx = widget->parent->realx + widget->x;
		widget->realy = widget->parent->realy + widget->y;
		parent_width = widget->parent->width;
		parent_height = widget->parent->height;
	}
	else {
		widget->realx = widget->x;
		widget->realy = widget->y;
		get_window_size(widget->window, &parent_width, &parent_height);
	}
#define MIN(a,b) ((a) < (b) ? (a) : (b))
	widget->realwidth = MIN(parent_width - widget->x, widget->width);
	widget->realheight = MIN(parent_height - widget->y, widget->height);
#undef MIN
	widget->dirty = true;
	widget->child_dirty = true;

	for (ptr = widget->children; ptr; ptr = ptr->next) {
		__rtk_update_widget(ptr);
	}
}

static int __widget_event(struct widget *widget, const char *event, int argc, char **argv, int mousex, int mousey) {
	struct widget *ptr;

	for (ptr = widget->children; ptr; ptr = ptr->next) {
		if ((ptr->realx <= mousex && mousex <= ptr->realx + ptr->realwidth) &&
				(ptr->realy <= mousey && mousey <= ptr->realy + ptr->realheight)) {
			if (!widget_event(ptr, event, argc, argv)) {
				return 0;
			}
		}
	}

	lua_getglobal(widget->L, event);
	lua_pushnumber(widget->L, mousex - widget->realx);
	lua_pushnumber(widget->L, mousey - widget->realy);
	for (int i = 0; i < argc; i++) {
		lua_pushstring(widget->L, argv[i]);
	}
	return __rtk_call_lua_function(widget->L, argc + 2, 0);
}

int widget_event(struct widget *widget, const char *event, int argc, char **argv) {
	int mousex, mousey;
	char *ret = rcall(widget->window->fb->rp, "getmouse");

	if (!ret) {
		return 1;
	}

	if (sscanf(ret, "%i %i", &mousex, &mousey) != 2) {
		return 1;
	}

	return __widget_event(widget, event, argc, argv, mousex, mousey);
}

int widget_call(struct widget *widget, const char *func, ...) {
	va_list vl;
	const char *arg;
	int argc = 0;

	lua_getglobal(widget->L, func);

	va_start(vl, func);
	do {
		arg = va_arg(vl, const char*);
		if (arg) {
			lua_pushstring(widget->L, arg);
			argc++;
		}
	} while (arg);
	va_end(vl);

	return __rtk_call_lua_function(widget->L, argc, 0);
}

struct widget *find_child(struct widget *widget, const char *name) {
	struct widget *ptr, *ret;

	if (widget->name && !strcmp(widget->name, name)) {
		return widget;
	}

	for (ptr = widget->children; ptr; ptr = ptr->next) {
		ret = find_child(ptr, name);
		if (ret) {
			return ret;
		}
	}

	return NULL;
}

void set_position(struct widget *widget, int x, int y) {
	widget->x = x;
	widget->y = y;
	__rtk_update_widget(widget);
}

void get_position(struct widget *widget, int *x, int *y) {
	*x = widget->x;
	*y = widget->y;
}

int set_size(struct widget *widget, int width, int height) {
	widget->width = width;
	widget->height = height;

	__rtk_update_widget(widget);

	return set_attribute_int(widget, "width", width) ||
	       set_attribute_int(widget, "height", height);
}

void get_size(struct widget *widget, int *width, int *height) {
	*width = widget->width;
	*height = widget->height;
	__rtk_update_widget(widget);
}

void set_name(struct widget *widget, const char *name) {
	free(widget->name);
	widget->name = strdup(name);
}

char *get_name(struct widget *widget) {
	return widget->name;
}

struct widget *__rtk_get_widget(lua_State *L) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, 1);
		return lua_touserdata(L, -1);
}

int __rtk_set_attribute(struct widget *widget) {
	lua_getglobal(widget->L, "set_attribute");
	lua_insert(widget->L, -3);
	return __rtk_call_lua_function(widget->L, 2, 0);
} 

int __rtk_get_attribute(struct widget *widget) {
	lua_getglobal(widget->L, "get_attribute");
	lua_insert(widget->L, -2);
	return __rtk_call_lua_function(widget->L, 1, 1);
}

#define ATTRIBUTE_FUNCS(ctype,typename,luatype) \
	int set_attribute_##typename(struct widget *widget, const char *name, ctype value) { \
		lua_pushstring(widget->L, name); \
		lua_push##luatype(widget->L, value); \
		if (__rtk_set_attribute(widget)) { \
			return 1; \
		} \
		return 0; \
	} \
	\
	ctype get_attribute_##typename(struct widget *widget, const char *name) { \
		ctype ret; \
		\
		lua_pushstring(widget->L, name); \
		if (__rtk_get_attribute(widget)) { \
			return 0; \
		} \
		if (!lua_is##luatype(widget->L, -1)) { \
			lua_pop(widget->L, -1); \
			return 0; \
		} \
		ret = lua_to##luatype(widget->L, -1); \
		\
		return ret; \
	}

ATTRIBUTE_FUNCS(int, int, number)
ATTRIBUTE_FUNCS(bool, bool, boolean)
ATTRIBUTE_FUNCS(double, double, number)
ATTRIBUTE_FUNCS(const char*, string, string)
