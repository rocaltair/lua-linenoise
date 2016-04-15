#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include "linenoise.h"

#if LUA_VERSION_NUM < 502 && (!defined(luaL_newlib))
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

static lua_State *runningL = NULL;
static int completionRef = LUA_NOREF;

static int lua__line(lua_State *L)
{
	const char * prompt = luaL_checkstring(L, 1);
	char * line = linenoise(prompt);
	if (line != NULL) {
		lua_pushstring(L, line);
		free(line);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int lua__historyAdd(lua_State *L)
{
	const char * line = luaL_checkstring(L, 1);
	int rc = linenoiseHistoryAdd(line);
	lua_pushboolean(L, rc);
	return 1;
}

static int lua__historySave(lua_State *L)
{
	const char * filename = luaL_checkstring(L, 1);
	int rc = linenoiseHistorySave(filename);
	lua_pushboolean(L, !rc);
	return 1;
}

static int lua__historyLoad(lua_State *L)
{
	const char * filename = luaL_checkstring(L, 1);
	int rc = linenoiseHistoryLoad(filename);
	lua_pushboolean(L, !rc);
	return 1;
}

static int lua__historySetMaxLen(lua_State *L)
{
	int n = luaL_checkint(L, 1);
	int rc = linenoiseHistorySetMaxLen(n);
	lua_pushboolean(L, rc);
	return 1;
}

static int lua__setMultiLine(lua_State *L)
{
	int mlMode = lua_toboolean(L, 1);
	linenoiseSetMultiLine(mlMode);
	return 0;
}

static int lua__clearScreen(lua_State *L)
{
	linenoiseClearScreen();
	return 0;
}

void completion(const char *buf, linenoiseCompletions *lc) {
	lua_State * L = runningL;
	if (completionRef == LUA_NOREF) {
		return;
	}
	lua_rawgeti(L, LUA_REGISTRYINDEX, completionRef);
	if (!lua_isfunction(L, -1)) {
		return;
	}
	lua_pushlightuserdata(L, lc);
	lua_pushstring(L, buf);
	lua_pcall(L, 2, 0, 0);
}

static int lua__setCompletor(lua_State *L)
{
	if (!lua_isfunction(L, 1)) {
		return luaL_argerror(L, 1, "function requried");
	}
	runningL = L;
	if (completionRef != LUA_NOREF) {
		luaL_unref(L, LUA_REGISTRYINDEX, completionRef);
	}
	lua_pushvalue(L, 1);
	completionRef = luaL_ref(L, LUA_REGISTRYINDEX);
	linenoiseSetCompletionCallback(completion);
	return 0;
}

static int lua__addComplete(lua_State *L)
{
	linenoiseCompletions *lc = NULL;
	const char *completeLine = NULL;
	if (!lua_islightuserdata(L, 1)) {
		return luaL_argerror(L, 1, "lightuserdata");
	}
	lc = (linenoiseCompletions *)lua_touserdata(L, 1);
	completeLine = luaL_checkstring(L, 2);
	linenoiseAddCompletion(lc, completeLine);
	return 0;
}

int luaopen_linenoise(lua_State* L)
{
	luaL_Reg lfuncs[] = {
		{"line", lua__line},
		{"historyAdd", lua__historyAdd},
		{"historySave", lua__historySave},
		{"historyLoad", lua__historyLoad},
		{"historySetMaxLen", lua__historySetMaxLen},
		{"setCompletor", lua__setCompletor},
		{"addComplete", lua__addComplete},
		{"setMultiLine", lua__setMultiLine},
		{"clearScreen", lua__clearScreen},
		{NULL, NULL},
	};
	luaL_newlib(L, lfuncs);
	return 1;
}
