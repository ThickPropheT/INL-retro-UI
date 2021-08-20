#include "lua_extensions.h"

bool lua_loadtable(lua_State *L, char *filePath)
{
	// these calls return error codes, not success codes.
	// the return value of the calling function indicates success.
	return !(luaL_loadfile(L, filePath) || lua_pcall(L, 0, 0, 0));
}

bool lua_trystring(lua_State *L, int i, void **result)
{
	if (!lua_isstring(L, i))
		return false;
	
	*result = (void *)lua_tostring(L, i);
	return true;
}

bool lua_trynumber(lua_State *L, int i, void **result)
{
	if (!lua_isnumber(L, i))
		return false;
	
	double r = lua_tonumber(L, i);
	*result = &r;
	
	return true;
}

void lua_printcallfailed(lua_State *L)
{
	printf("Failed to invoke lua function: %s%c", lua_tostring(L, -1), '\0');
}

bool lua_callfunc(
	lua_State *L,
	char *name,
	PushArgsFunc pushArgs,
	TryReturnFunc tryReturn,
	void **result)
{
	lua_getglobal(L, name);
	
	int argCount = 0;
	
	if (pushArgs != NULL)
		argCount = pushArgs(L);
	
	int returnCount = tryReturn != NULL ? 1 : 0;
	
	// this call returns an error code, not a success code.
	// the return value of the calling function indicates success.
	if (lua_pcall(L, argCount, returnCount, 0))
	{
		lua_printcallfailed(L);
		return false;
	}

	if (tryReturn == NULL)
		return true;
	
	if (!tryReturn(L, -1, result))
	{
		printf("Invocation of lua function returned unexpected type: %s%c", lua_tostring(L, -1), '\0');
		return false;
	}
	
	lua_pop(L, 1);
	
	return true;
}

void lua_gettablefield(lua_State *L, int i, char *name, void (*callback)())
{
	lua_getfield(L, i, name);
	callback();
	lua_pop(L, 1);
}

void lua_pushtablefield(lua_State *L, char *key, void (*pushValue)())
{
	lua_pushstring(L, key);
	pushValue();
	lua_settable(L, -3);
}