#pragma once

#include "config.h"

struct lua_State;

#if TY_LUABIND_TYPE_SAFE

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

bool tryCheckPointerType(const void* ptr, TypeId typeId);
bool checkPointerType(const void* ptr, TypeId typeInfo);
void registerPointerWithType(const void* ptr, TypeId typeInfo);
void unregisterPointer(const void* ptr);

// Helpers
template <class T>
inline void registerPointerType(const T* ptr) {
	registerPointerWithType(static_cast<const void*>(ptr), getTypeId(ptr));
}


template <class T>
inline bool checkPointerType(const void* ptr) {
	return checkPointerType(ptr, getTypeId<T>());
}

} // namespace Typhoon::LuaBind::detail

#endif
