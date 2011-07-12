#include "widget.h"
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include "private.h"

struct widget *__rtk_curwidget;

static int update(lua_State *__rtk_L) {
	fb_flip(__rtk_fb);
	return 0;
}

static int write_text(lua_State *__rtk_L) {
	int x, y, size, fg, bg;
	const char *text;
	int ret = 0;

	if (!lua_isnumber(__rtk_L, 1)) ret = 1;
	if (!lua_isnumber(__rtk_L, 2)) ret = 1;
	if (!lua_isnumber(__rtk_L, 3)) ret = 1;
	if (!lua_isstring(__rtk_L, 4)) ret = 1;
	if (!lua_isnumber(__rtk_L, 5)) ret = 1;
	if (!lua_isnumber(__rtk_L, 6)) ret = 1;

	if (!ret) {
		x = lua_tonumber(__rtk_L, 1);
		y = lua_tonumber(__rtk_L, 2);
		size = lua_tonumber(__rtk_L, 3);
		text = lua_tostring(__rtk_L, 4);
		fg = lua_tonumber(__rtk_L, 5);
		bg = lua_tonumber(__rtk_L, 6);

		ret = fb_write(__rtk_fb, __rtk_curwidget->x + x, __rtk_curwidget->y + y, size, text, strlen(text), fg, bg);
	}
	
	lua_pushboolean(__rtk_L, ret);
	return 1;
}

#define EXPORT_FUNC(x) \
    lua_pushcfunction(__rtk_L, x); \
    lua_setglobal(__rtk_L, #x);

void __rtk_init_drawing_functions() {
	EXPORT_FUNC(update);
	EXPORT_FUNC(write_text);
}
