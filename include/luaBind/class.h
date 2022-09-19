#pragma once

#include "config.h"

#include "reference.h"
#include <core/typeId.h>

struct lua_State;

namespace Typhoon::LuaBind::detail {

Reference registerCppClass(lua_State* ls, const char* className, TypeId classId, TypeId baseClassId);
void      registerNewAndDeleteOperators(lua_State* ls, int tableIndex, lua_CFunction newFunction, lua_CFunction deleteFunction);
void      registerNewOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize);
void      registerDeleteOperator(lua_State* ls, int tableIndex, lua_CFunction closure, const void* functionPtr, size_t functionPtrSize);

// Called from Lua, it allows registration of Lua classes visible to C++
int registerLuaClass(lua_State* ls);

// Called from Lua, it returns the metatable of a registered class
int getClassMetatable(lua_State* ls);

template <class T>
Reference registerCppClass(lua_State* ls, const char* className, TypeId baseClassId);

template <typename retType, typename... argType>
void registerNewOperator(lua_State* ls, int tableStackIndex, retType (*functionPtr)(argType...));

template <typename T, typename... argType>
void registerDefaultNewOperator(lua_State* ls, int tableIndex);

bool isAllocatedByLua(lua_State* ls, int userDataStackIndex);

} // namespace Typhoon::LuaBind::detail

#include "class.inl"
