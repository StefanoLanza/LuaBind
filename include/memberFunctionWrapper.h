#pragma once

#include "typeWrapper.h"
#include <core/ptrUtil.h>
#include <lua/src/lua.hpp>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

template <typename objType, typename retType, typename... argType, std::size_t... argIndices>
int memberFunctionWrapperImpl(lua_State* ls, std::integer_sequence<std::size_t, argIndices...> indx) {
	const void* const ud = lua_touserdata(ls, lua_upvalueindex(1));

	// Extract function pointer from Lua user data
	using func_ptr = retType (objType::*)(argType...);
	const func_ptr func = serializePOD<func_ptr>(ud, 0);

	// Get self
	objType* const self = pop<objType*>(ls, 1);
	if (! self) {
		return luaL_argerror(ls, 1, "nil self");
	}

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	const int argStackSize[] = { getStackSize<argType>()..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argType) + 1] = {};
	argStackIndex[0] = 2;
	for (size_t i = 1; i < sizeof...(argType); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	checkArgs<argType...>(ls, argStackIndex, indx);

	if constexpr (std::is_void_v<retType>) {
		(self->*func)(pop<argType>(ls, argStackIndex[argIndices])...);
		return 0;
	}
	else {
		return push(ls, (self->*func)(pop<argType>(ls, argStackIndex[argIndices])...));
	}
}

template <typename objType, typename retType, typename... argType>
int memberFunctionWrapper(lua_State* ls) {
	return memberFunctionWrapperImpl<objType, retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <typename objType, typename retType, typename... argType>
inline void registerMemberFunction(lua_State* ls, retType (objType::*func)(argType...), const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = memberFunctionWrapper<objType, retType, argType...>;
	pushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func));
	lua_settable(ls, tableStackIndex);
}

// const
template <typename objType, typename retType, typename... argType>
inline void registerMemberFunction(lua_State* ls, retType (objType::*func)(argType...) const, const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = memberFunctionWrapper<objType, retType, argType...>;
	pushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func));
	lua_settable(ls, tableStackIndex);
}

} // namespace Typhoon::LuaBind::detail
