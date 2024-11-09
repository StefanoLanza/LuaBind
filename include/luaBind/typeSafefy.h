#pragma once

#include "config.h"

struct lua_State;

#if TY_LUABIND_TYPE_SAFE

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

void registerBaseClass(lua_State* ls,  TypeId super, TypeId base);
bool tryCheckPointerType(lua_State* ls, const void* ptr, TypeId typeId);
bool checkPointerType(lua_State* ls, const void* ptr, TypeId typeId);
void registerPointer(lua_State* ls, const void* ptr, TypeId typeId);
void unregisterPointer(lua_State* ls, const void* ptr);

// Helpers
template <class T>
inline void registerPointer(lua_State* ls, const T* ptr) {
	static_assert(! std::is_void<T>(), "invalid const void* ptr");
	registerPointer(ls, ptr, getTypeId<T>());
}


template <class T>
inline bool checkPointerType(lua_State* ls, const void* ptr) {
	return checkPointerType(ls, ptr, getTypeId<T>());
}

} // namespace Typhoon::LuaBind::detail

#endif
