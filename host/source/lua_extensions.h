#pragma once

#include <stdbool.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

/// a callback type for pushing arguments to the lua stack.
/// lua_State	: the lua_State to act on.
/// returns		: MUST return the number of arguments pushed to the lua stack.
typedef int (*PushArgsFunc)(lua_State *);

/// a callback type for attempting coercion of lua types.
/// lua_State	: the lua_State to act on.
///	int			: context sensitive passthrough to the lua call. (TODO is this necessary, considering the type is defined as 'try return'?
/// void **		: the result of the attempted coercion.
/// returns		: true on complete success; false if any sub-operation failed;
typedef bool (*TryReturnFunc)(lua_State *, int, void **);

bool lua_loadtable(lua_State *L, char *filePath);

/// a function primarily intended to suit the TryReturnFunc callback type.
/// result	: true if the value at i is a string; false otherwise.
/// returns	: on success, the char * at i.
bool lua_trystring(lua_State *L, int i, void **result);

/// a function primarily intended to suit the TryReturnFunc callback type.
/// result	: true if the value at i is a number; false otherwise.
/// returns	: on success, the double at i.
bool lua_trynumber(lua_State *L, int i, void **result);

void lua_printcallfailed(lua_State *L);

/// a function to encapsulate basic lua function call procedure.
/// L 			: the lua_State to act on.
/// name 		: the name of the function to call.
/// pushArgs	: a callback to provide arguments for the functio call; NULL for a call with 0 arguments.
/// tryReturn	: a callback to attempt coercion of the function's return value; NULL for call with NO arguments.
/// result 		: the result of the attempted coercion of the function's return value, if successful.
/// returns		: true on complete success; false if any sub-operation failed.
bool lua_callfunc(
	lua_State *L,
	char *name,
	PushArgsFunc pushArgs,
	TryReturnFunc tryReturn,
	void **result);

void lua_gettablefield(lua_State *L, int i, char *name, void (*callback)());

void lua_pushtablefield(lua_State *L, char *key, void (*pushValue)());