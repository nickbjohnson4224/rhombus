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

#include <string.h>
#include <lua.h>
#include "private.h"

static void mark_child_dirty(struct widget *widget) {
	widget->child_dirty = true;
	if (widget->parent) {
		mark_child_dirty(widget->parent);
	}
}
static int request_redraw(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);

	widget->dirty = true;
	if (widget->parent) {
		mark_child_dirty(widget->parent);
	}
	return 0;
}

static int send_event(lua_State *L) {
	struct widget *widget = __rtk_get_widget(L);

	if (lua_isstring(L, 1) && widget->window->handler) {
		widget->window->handler(widget, lua_tostring(L, 1));
	}
	return 0;
}

static int add_child(lua_State *L) {
	bool error = false;
	struct widget *child = NULL;
	struct widget *widget = __rtk_get_widget(L);
	const char *type;
	int x, y, width, height;

	if (lua_gettop(L) < 5) error = true;
	if (!lua_isstring(L, 1)) error = true;
	if (!lua_isnumber(L, 2)) error = true;
	if (!lua_isnumber(L, 3)) error = true;
	if (!lua_isnumber(L, 4)) error = true;
	if (!lua_isnumber(L, 5)) error = true;

	if (!error) {
		type = lua_tostring(L, 1);
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
		width = lua_tonumber(L, 4);
		height = lua_tonumber(L, 5);

		child = __rtk_add_widget(type, widget, widget->window, x, y, width, height);
	}

	if (!child) {
		lua_pushnil(L);
	}
	else {
		lua_pushlightuserdata(L, child);
	}
	return 1;
}

static int call_child(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *func = lua_tostring(L, 2);
	const char *arg;

	if (lua_gettop(L) < 2) {
		lua_pushboolean(L, true);
		return 1;
	}

	lua_getglobal(child->L, func);

	for (int i = 3; i <= lua_gettop(L); i++) {
		arg = lua_tostring(L, i);
		lua_pushstring(child->L, arg);
	}

	int ret = __rtk_call_lua_function(child->L, lua_gettop(L) - 2, 0);
	lua_pushboolean(L, ret);
	return 1;
}

static int set_child_attribute(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *name, *data;
	int value = 0;
	int ret = 0;

	if (lua_gettop(L) < 3 || !lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}

	name = lua_tostring(L, 2);
	data = lua_tostring(L, 3);
	if (lua_isnumber(L, 3)) {
		value = lua_tonumber(L, 3);
	}
	
	if (!strcmp(name, "x")) {
		child->x = value;
		__rtk_update_widget(child);
	}
	else if (!strcmp(name, "y")) {
		child->y = value;
		__rtk_update_widget(child);
	}
	else {
		lua_pushstring(child->L, name);
		lua_pushstring(child->L, data);
		ret = __rtk_set_attribute(child);
	}

	if (!ret) {
		if (!strcmp(name, "width")) {
			child->width = value;
			__rtk_update_widget(child);
		}
		if (!strcmp(name, "height")) {
			child->height = value;
			__rtk_update_widget(child);
		}
	}

	lua_pushboolean(L, ret);
	return 1;
}

static int get_child_attribute(lua_State *L) {
	struct widget *child = lua_touserdata(L, 1);
	const char *name, *value;

	if (lua_gettop(L) < 2 || !lua_isstring(L, 2)) {
		lua_pushnumber(L, 0);
		return 1;
	}
	name = lua_tostring(L, 2);

	if (!strcmp(name, "x")) {
		lua_pushnumber(L, child->x);
	}
	else if (!strcmp(name, "y")) {
		lua_pushnumber(L, child->y);
	}
	else if (!strcmp(name, "width")) {
		lua_pushnumber(L, child->width);
	}
	else if (!strcmp(name, "height")) {
		lua_pushnumber(L, child->height);
	}
	else {
		lua_pushstring(child->L, name);
		if (__rtk_get_attribute(child)) {
			lua_pushnil(L);
		}
		else {
			value = lua_tostring(child->L, -1);
			lua_pushstring(L, value);
		}
	}
	return 1;
}

void __rtk_init_library(lua_State *L) {
	EXPORT_FUNC(request_redraw);
	EXPORT_FUNC(send_event);

	EXPORT_FUNC(add_child);
	EXPORT_FUNC(call_child);
	EXPORT_FUNC(set_child_attribute);
	EXPORT_FUNC(get_child_attribute);
}
