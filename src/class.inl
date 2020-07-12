#pragma once

#include "config.h"

#include "boxing.h"
#include "detail.h"
#include "freeFunctionWrapper.h"
#include "reference.h"
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

template <class T>
Reference registerCppClass(lua_State* ls, const char* className, TypeId baseClassId) {
	const Reference ref = registerCppClass(ls, className, Typhoon::getTypeId<T>(), baseClassId);
	if constexpr (Wrapper<T>::isLightweight()) {
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
	registerNewOperator(ls, tableStackIndex, luaFunc, functionPtr, sizeof functionPtr);
}

template <typename T>
void registerDefaultNewOperator(lua_State* ls, int tableIndex) {
	registerNewOperator(ls, tableIndex, newObject<T>);
}

} // namespace Typhoon::LuaBind::detail
