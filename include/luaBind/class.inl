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
int garbageCollect(lua_State* ls) {
	if (! lua_isuserdata(ls, 1)) {
		return 0; // object registered as table
	}
	// Extract pointer from user data
	T* obj = nullptr;
	std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
	assert(obj);
	if (lua_getiuservalue(ls, 1, 1) != LUA_TNONE) {
		assert(lua_tointeger(ls, -1) == 0);
		delete obj; // object allocated by Lua
	}
	// else object was on stack
	lua_pop(ls, 1); // pops value pushed by lua_getiuservalue
	return 0;
}

template <class T>
lua_CFunction getDestructor() {
	if constexpr (std::is_trivially_destructible_v<T>) {
		return nullptr;
	}
	else {
		return garbageCollect<T>;
	}
}

template <class T>
Reference registerCppClass(lua_State* ls, const char* className, TypeId baseClassId) {
	const Reference ref = registerCppClass(ls, className, Typhoon::getTypeId<T>(), baseClassId, getDestructor<T>());
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

template <typename T>
void registerDefaultNewOperator(lua_State* ls, int tableIndex) {
	registerNewOperator(ls, tableIndex, newObject<T>);
}

} // namespace Typhoon::LuaBind::detail
