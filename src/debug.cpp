#include "debug.h"
#include <iostream>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind {

// traceback function, adapted from lua.c
// when a runtime error occurs, this will append the call stack to the error message
int traceback(lua_State* ls) {
	// look up Lua's 'debug.traceback' function
	lua_getglobal(ls, "debug");
	if (! lua_istable(ls, -1)) {
		lua_pop(ls, 1);
		return 1;
	}
	lua_getfield(ls, -1, "traceback");
	if (! lua_isfunction(ls, -1)) {
		lua_pop(ls, 2);
		return 1;
	}
	lua_pushvalue(ls, 1);   /* pass error message */
	lua_pushinteger(ls, 2); /* skip this function and traceback */
	lua_call(ls, 2, 1);     /* call debug.traceback */
	return 1;
}

void dumpStack(lua_State* ls) {
	const int n = lua_gettop(ls);
	for (int i = 1; i <= n; ++i) {
		switch (lua_type(ls, i)) {
		case LUA_TNIL:
			std::cout << "nil" << std::endl;
			break;
		case LUA_TNUMBER:
			std::cout << "number: " << lua_tonumber(ls, i) << std::endl;
			break;
		case LUA_TTABLE:
			std::cout << "table: " << std::endl;
			break;
		case LUA_TSTRING:
			std::cout << "string: " << lua_tostring(ls, i) << std::endl;
			break;
		case LUA_TBOOLEAN:
			std::cout << "bool: " << lua_toboolean(ls, i) << std::endl;
			break;
		case LUA_TLIGHTUSERDATA:
			std::cout << "light user data :" << std::endl;
			break;
		case LUA_TUSERDATA:
			std::cout << "user data :" << std::endl;
			break;
		case LUA_TFUNCTION:
			std::cout << "function :" << std::endl;
			break;
		case LUA_TTHREAD:
			std::cout << "thread :" << std::endl;
			break;

		default:
			std::cout << "unknown param" << std::endl;
			break;
		}
	}
}

} // namespace Typhoon::LuaBind
