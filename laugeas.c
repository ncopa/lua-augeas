#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <augeas.h>

#define LIBNAME "augeas"
#define PAUG_META "augeas"

#ifndef VERSION
#define VERSION "unknown"
#endif

static int Paug_init(lua_State *L)
{
	augeas **a;
	a = (augeas **) lua_newuserdata(L, sizeof(augeas *));
	luaL_getmetatable(L, PAUG_META);
	lua_setmetatable(L, -2);

	*a = aug_init(NULL, NULL, 0);
	if (*a == NULL)
		luaL_error(L, "aug_init failed");
	return 1;
}

static augeas **Paug_checkarg_index(lua_State *L, int index)
{
	augeas **a;
	luaL_checktype(L, index, LUA_TUSERDATA);
	a = (augeas **) luaL_checkudata(L, index, PAUG_META);
	if (a == NULL)
		luaL_typerror(L, index, PAUG_META);
	return a;
}

static augeas **Paug_checkarg(lua_State *L)
{
	return Paug_checkarg_index(L, 1);
}

static int Paug_close(lua_State *L)
{
	augeas **a = Paug_checkarg(L);
	if (*a)
		aug_close(*a);
	return 0;
}

static int Paug_get(lua_State *L)
{
	augeas **a;
	const char *path;
	const char *value = NULL;

	a = Paug_checkarg(L);
	path = luaL_checkstring(L, 2);
	lua_pushinteger(L, aug_get(*a, path, &value));
	lua_pushstring(L, value);
	return 2;
}

static int Paug_print(lua_State *L)
{
	augeas **a;
	const char *path;
	a = Paug_checkarg(L);
	path = luaL_checkstring(L, 2);
	lua_pushinteger(L, aug_print(*a, stdout, path));
	return 1;
}

static const luaL_reg Paug_methods[] = {
	{"init",	Paug_init},
	{"get",		Paug_get},
	{"print",	Paug_print},
	{NULL,		NULL}
};

static const luaL_reg Luag_meta_methods[] = {
	{"__gc",	Paug_close},
	{NULL,		NULL}
};

LUALIB_API int luaopen_augeas(lua_State *L)
{
	luaL_register(L, LIBNAME, Paug_methods);
	lua_pushliteral(L, "version");
	lua_pushliteral(L, VERSION);
	lua_settable(L, -3);

	luaL_newmetatable(L, PAUG_META);
	luaL_register(L, NULL, Luag_meta_methods);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, -3);
	lua_rawset(L, -3);
	lua_pop(L, 1);

	return 1;
}

