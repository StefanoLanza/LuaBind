#pragma once

#include "config.h"

struct lua_State;

#if TY_LUABIND_TYPE_SAFE

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

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

// Check type of an object on the stack
/*! \param index stack index
 * \param className expected class name
 */
bool checkType(lua_State* ls, int index, const char* className);

// Check type of an object on the stack
/*! \param index stack index
 */
template <class T>
bool checkType(lua_State* ls, int index) {
	assert(index >= 1);
	const char* const className = typeName<T>();
	return checkType(ls, index, className);
}

} // namespace Typhoon::LuaBind::detail

#endif
