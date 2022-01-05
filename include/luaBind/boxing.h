#pragma once

#include <cstddef>

struct lua_State;

namespace Typhoon::LuaBind::detail {

// Boxing functions for lightweight objects

template <class T>
int box(lua_State* ls);

template <class T>
int store(lua_State* ls);

template <class T>
int retrieve(lua_State* ls);

template <class T>
void pushBoxingFunctions(lua_State* ls, int tableStackIndex);

void* allocateBoxed(lua_State* ls, size_t size, size_t alignment);

// For GC
int collectBoxed(lua_State* ls);

} // namespace Typhoon::LuaBind::detail

#include "boxing.inl"
