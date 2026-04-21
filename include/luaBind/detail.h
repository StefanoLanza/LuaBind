#pragma once

#include <lua/src/lua.hpp>

#include <core/scopedAllocator.h>
#include <core/typeId.h>
#ifdef _DEBUG
#include <core/ptrUtil.h>
#endif

#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

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

ScopedAllocator* getTemporaryAllocator(lua_State* ls);

// Helper
template <class T, class... ArgTypes>
T* allocTemporary(lua_State* ls, ArgTypes&&... args) {
	auto allocator = getTemporaryAllocator(ls);
	T*   ptr = allocator->make<T>(std::forward<ArgTypes>(args)...);
#ifdef _DEBUG
	ptr = decoratePointer(ptr, 1 + (allocator->getEpoch() % 7));
#endif
	return ptr;
}

#ifdef _DEBUG
void checkDanglingPointer(lua_State* ls, const void* ptr, int stackIndex);
#endif

#if TY_LUABIND_TYPE_SAFE

void registerBaseClass(lua_State* ls, TypeId super, TypeId base);
bool checkPointerType(lua_State* ls, const void* ptr, TypeId typeId);
void registerTemporaryPointer(lua_State* ls, const void* ptr, TypeId typeId);
bool compatibleTypes(lua_State* ls, TypeId first, TypeId second);

#endif

} // namespace Typhoon::LuaBind::detail
