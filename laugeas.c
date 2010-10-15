#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <augeas.h>

#define LIBNAME "augeas"
#define PAUG_META "augeas"

#ifndef VERSION
#define VERSION "unknown"
#endif

struct aug_flagmap {
	const char *name;
	int value;
};

struct aug_flagmap Taug_flagmap[] = {
	{ "none",		AUG_NONE },
	{ "save_backup",	AUG_SAVE_BACKUP },
	{ "save_newfile",	AUG_SAVE_NEWFILE },
	{ "typecheck",		AUG_TYPE_CHECK },
	{ "no_stdinc",		AUG_NO_STDINC },
	{ "save_noop",		AUG_SAVE_NOOP },
	{ "no_load",		AUG_NO_LOAD },
	{ "no_modl_autoload",	AUG_NO_MODL_AUTOLOAD },
	{ NULL, 0 }
};

static const char *get_opt_string_field(lua_State *L, int index,
				        const char *key, const char *def)
{
	const char *value;
	lua_getfield(L, index, key);
	value = luaL_optstring(L, -1, def);
	lua_pop(L, 1);
	return value;
}

static int get_boolean_field(lua_State *L, int index, const char *key)
{
	int value;
	lua_getfield(L, index, key);
	value = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return value;
}

static int Paug_init(lua_State *L)
{
	augeas **a;
	struct aug_flagmap *f;
	const char *root = NULL, *loadpath = NULL;
	int flags = 0;

	a = (augeas **) lua_newuserdata(L, sizeof(augeas *));
	luaL_getmetatable(L, PAUG_META);
	lua_setmetatable(L, -2);

	if (lua_istable(L, 1)) {
		root = get_opt_string_field(L, 1, "root", NULL);
		loadpath = get_opt_string_field(L, 1, "loadpath", NULL);
		for (f = Taug_flagmap; f->name != NULL; f++)
			if (get_boolean_field(L, 1, f->name))
				flags |= f->value;
	} else {
		root = luaL_optstring(L, 1, NULL);
		loadpath = luaL_optstring(L, 2, NULL);
		flags = luaL_optinteger(L, 3, AUG_NONE);
	}
	*a = aug_init(root, loadpath, flags);
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
	struct aug_flagmap *f = Taug_flagmap;
	luaL_register(L, LIBNAME, Paug_methods);
	lua_pushliteral(L, "version");
	lua_pushliteral(L, VERSION);
	lua_settable(L, -3);

	while (f->name != NULL) {
		lua_pushstring(L, f->name);
		lua_pushinteger(L, f->value);
		lua_settable(L, -3);
		f++;
	}

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

