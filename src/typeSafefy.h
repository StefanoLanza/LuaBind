#pragma once

#include "config.h"

#if TY_LUA_TYPE_SAFE

#include <core/typeInfo.h>

namespace Typhoon::LUA::detail {

bool checkPointerType(const void* ptr, TypeId typeInfo);
void registerPointerType(const void* ptr, TypeId typeInfo);
void unregisterPointerType(const void* ptr);

// Helpers
template <class T>
inline void registerPointerType(const T* ptr) {
	registerPointerType(static_cast<const void*>(ptr), getTypeId(ptr));
}

template <class T>
inline bool checkPointerType(const void* ptr) {
	return checkPointerType(ptr, getTypeId<T>());
}

} // namespace Typhoon::LUA::detail

#endif
