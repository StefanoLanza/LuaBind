#pragma once

struct lua_State;

namespace Typhoon::LuaBind {

// traceback function, adapted from lua.c
// when a runtime error occurs, this will append the call stack to the error message
int traceback(lua_State* ls);

void dumpStack(lua_State* ls);

} // namespace Typhoon::LuaBind
