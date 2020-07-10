#pragma once

#include "config.h"

#include "reference.h"
#include "detail.h"
#include "freeFunctionWrapper.h"
#include <core/typeId.h>

struct lua_State;

namespace Typhoon::LuaBind::detail {

Reference registerCppClass(lua_State* ls, const char* className, TypeId classID, TypeId baseClassID);
void      registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure);
void      registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize);

template <typename retType, typename... argType>
inline void registerNewOperator(lua_State* ls, int tableStackIndex, retType (*functionPtr)(argType...)) {
	lua_CFunction luaFunc = freeFunctionWrapper<retType, argType...>;
	registerNewOperator(ls, tableStackIndex, luaFunc, functionPtr, sizeof functionPtr);
}

template <typename T>
void registerDefaultNewOperator(lua_State* ls, int tableIndex) {
	registerNewOperator(ls, tableIndex, newObject<T>);
}

// Called from Lua, it allows registration of Lua classes visible to C++
int registerLuaClass(lua_State* ls);

// Called from Lua, it returns the metatable of a registered class
int getClassMetatable(lua_State* ls);

} // namespace Typhoon::LuaBind::detail
