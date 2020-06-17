#pragma once

#include <lua/src/lua.hpp>
#include <string>

struct lua_State;

namespace Typhoon::LUA {

bool toString(lua_State* ls, int numArg, const char*& s);
bool toString(lua_State* ls, int numArg, std::string& s);
bool toBool(lua_State* ls, int numArg, bool& i);
bool toInteger(lua_State* ls, int numArg, int& i);
bool toFloat(lua_State* ls, int numArg, float& d);
bool toDouble(lua_State* ls, int numArg, double& d);

} // namespace Typhoon::LUA
