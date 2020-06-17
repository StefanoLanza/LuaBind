#pragma once

#include "private.h"
#include <core/typeId.h>

namespace Typhoon::LUA::detail {

Reference registerCppClass(lua_State* ls, const char* className, TypeId classID, TypeId baseClassID);
void      registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure);
void      registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize);

template <typename Type>
void registerDefaultNewOperator(lua_State* ls, int tableIndex) {
	registerNewOperator(ls, tableIndex, newObject<Type>);
}

// Called from Lua, it allows registration of Lua classes visible to C++
int registerLuaClass(lua_State* ls);

// Called from Lua, it returns the metatable of a registered class
int getClassMetatable(lua_State* ls);

} // namespace Typhoon::LUA::detail
