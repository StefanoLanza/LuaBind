#pragma once

#include "debug.h"
#include "detail.h"
#include "stackUtils.h"
#include <core/ptrUtil.h>
#include <lua/src/lua.hpp>

namespace Typhoon::LuaBind::detail {

template <typename retType, typename... argTypes, std::size_t... argIndices>
int freeFunctionWrapperImpl(lua_State* ls, std::index_sequence<argIndices...> indx) {
	// Extract function pointer from Lua user data
	using func_ptr = retType (*)(argTypes...);
	const void* const func_ud = lua_touserdata(ls, lua_upvalueindex(1));
	const auto        func = serializePOD<func_ptr>(func_ud);

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	constexpr int argStackSize[] = { Wrapper<argTypes>::stackSize..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argTypes) + 1] = {};
	argStackIndex[0] = 1;
	for (size_t i = 1; i < sizeof...(argTypes); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	checkArgs<argTypes...>(ls, argStackIndex, indx);

	// Call C++ function, popping all arguments from the stack
	if constexpr (std::is_void_v<retType>) {
		func(Wrapper<argTypes>::pop(ls, argStackIndex[argIndices])...);
		return 0;
	}
	else {
		Wrapper<retType>::push(ls, func(Wrapper<argTypes>::pop(ls, argStackIndex[argIndices])...));
		return Wrapper<retType>::stackSize;
	}
}

template <typename retType, typename... argType>
int freeFunctionWrapper(lua_State* ls) {
	return freeFunctionWrapperImpl<retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <typename retType, typename... argType>
inline void registerFunction(lua_State* ls, retType (*functionPtr)(argType...), const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = freeFunctionWrapper<retType, argType...>;
	pushFunctionAsUpvalue(ls, luaFunc, &functionPtr, sizeof(functionPtr));
	lua_settable(ls, tableStackIndex);
}

// Pick the overload with the first argument matching the bound class
template <typename T>
struct Overload {
	template <typename retType, typename... argType>
	static auto resolve(retType(func)(T self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto resolve(retType(func)(T* self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto resolve(retType(func)(const T* self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto resolve(retType(func)(T& self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto resolve(retType(func)(const T& self, argType...)) {
		return func;
	}
};

} // namespace Typhoon::LuaBind::detail
