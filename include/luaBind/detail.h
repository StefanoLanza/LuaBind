#pragma once

#include <lua/src/lua.hpp>

#include <cassert>
#include <cstring>
#include <cstdint>
#include <utility>
#include <type_traits>
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

// Extract a raw pointer from a table.
// Returns nullptr on error
void* retrievePointerFromTable(lua_State* ls, int idx);

void pushFunctionAsUpvalue(lua_State* ls, lua_CFunction closure, const void* functionPointer, size_t functionPointerSize);

template <typename T>
inline T serializePOD(const void* ptr) {
	static_assert(std::is_trivially_copyable_v<T>);
	T obj;
	std::memcpy(&obj, ptr, sizeof obj);
	return obj;
}

long long makePointerKey(const void* ptr, TypeId typeId);

#if TY_LUABIND_TYPE_SAFE

void registerBaseClass(lua_State* ls, TypeId super, TypeId base);
bool checkPointerType(lua_State* ls, const void* ptr, TypeId typeId);
void registerTemporaryPointer(lua_State* ls, const void* ptr, TypeId typeId);
bool compatibleTypes(lua_State* ls, TypeId first, TypeId second);

#endif

} // namespace Typhoon::LuaBind::detail
