#pragma once

#include "config.h"

#include "boxing.h"
#include "detail.h"
#include "freeFunctionWrapper.h"
#include "reference.h"
#include <core/typeId.h>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

template <class T>
int wrapDefaultDelete(lua_State* ls) {
	if (isAllocatedByLua(ls, 1)) {
		// Extract pointer from user data
		T* obj = nullptr;
		std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
		assert(obj);
		delete obj; // object allocated by Lua
	}
	// else object was allocated by C++
	return 0;
}

template <class T>
int wrapDeleter(lua_State* ls) {
	if (isAllocatedByLua(ls, 1)) {
		// Extract pointer from user data
		T* obj = nullptr;
		std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
		assert(obj);

		// Extract function pointer
		const void* const ud = lua_touserdata(ls, lua_upvalueindex(1));
		using Deleter = void (*)(T*);
		const auto deleter = serializePOD<Deleter>(ud, 0);

		deleter(obj); // object allocated by Lua
	}
	// else object was allocated by C++
	return 0;
}

template <class T>
Reference registerCppClass(lua_State* ls, const char* className, TypeId baseClassId) {
	const Reference ref = registerCppClass(ls, className, Typhoon::getTypeId<T>(), baseClassId);
	if constexpr (std::is_base_of_v<Lightweight<T>, Wrapper<T>>) {
		// Lightweight types get boxing functions
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref.getValue()); // table
		const int tableStackIndex = lua_gettop(ls);
		pushBoxingFunctions<T>(ls, tableStackIndex);
		lua_pop(ls, 1);
	}
	return ref;
}

template <typename retType, typename... argType>
inline void registerNewOperator(lua_State* ls, int tableStackIndex, retType (*functionPtr)(argType...)) {
	lua_CFunction luaFunc = freeFunctionWrapper<retType, argType...>;
	// FIXME Warning: implicit conversion between pointer-to-function and pointer-to-object is a Microsoft extension
	registerNewOperator(ls, tableStackIndex, luaFunc, reinterpret_cast<const void*>(functionPtr), sizeof functionPtr);
}

template <typename argType>
inline void registerDeleteOperator(lua_State* ls, int tableStackIndex, void (*functionPtr)(argType*)) {
	lua_CFunction luaFunc = wrapDeleter<argType>;
	// FIXME Warning: implicit conversion between pointer-to-function and pointer-to-object is a Microsoft extension
	registerDeleteOperator(ls, tableStackIndex, luaFunc, reinterpret_cast<const void*>(functionPtr), sizeof functionPtr);
}

template <typename T>
void registerDefaultNewOperator(lua_State* ls, int tableIndex) {
	registerNewOperator(ls, tableIndex, wrapNewObject<T>, wrapDefaultDelete<T>);
}

} // namespace Typhoon::LuaBind::detail
