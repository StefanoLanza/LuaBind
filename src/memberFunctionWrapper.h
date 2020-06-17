#pragma once

#include "typeWrapper.h"
#include <lua/src/lua.hpp>
#include <type_traits>

namespace Typhoon::LUA::detail {

template <typename objType, typename retType, typename... argType, std::size_t... argIndices>
int MemberFunctionWrapperHelper(lua_State* ls, std::integer_sequence<std::size_t, argIndices...> indx) {
	using func_ptr = retType (objType::*)(argType...);
	func_ptr func = *reinterpret_cast<func_ptr*>(lua_touserdata(ls, lua_upvalueindex(1)));

	// Get self
	objType* const self = Get<objType*>(ls, 1);
	if (! self) {
		luaL_argerror(ls, 1, "nil self");
	}

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	const int argStackSize[] = { GetStackSize<argType>()..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argType) + 1] = {};
	argStackIndex[0] = 2;
	for (size_t i = 1; i < sizeof...(argType); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	CheckArgs<argType...>(ls, argStackIndex, indx);

	if constexpr (std::is_void_v<retType>) {
		(self->*func)(Wrapper<argType>::Get(ls, argStackIndex[argIndices])...);
		return 0;
	}
	else {
		return Push(ls, (self->*func)(Wrapper<argType>::Get(ls, argStackIndex[argIndices])...));
	}
}

template <typename objType, typename retType, typename... argType>
int MemberFunctionWrapper(lua_State* ls) {
	return MemberFunctionWrapperHelper<objType, retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <typename objType, typename retType, typename... argType>
inline void RegisterMemberFunction(lua_State* ls, retType (objType::*func)(argType...), const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = MemberFunctionWrapper<objType, retType, argType...>;
	PushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func), Flags::none);
	lua_settable(ls, tableStackIndex);
}

// const
template <typename objType, typename retType, typename... argType>
inline void RegisterMemberFunction(lua_State* ls, retType (objType::*func)(argType...) const, const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = MemberFunctionWrapper<objType, retType, argType...>;
	PushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func), Flags::none);
	lua_settable(ls, tableStackIndex);
}

} // namespace Typhoon::LUA::detail