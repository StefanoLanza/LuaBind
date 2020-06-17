#pragma once

#include "debug.h"
#include "private.h"
#include "typeWrapper.h"
#include <lua/src/lua.hpp>

namespace Typhoon::LUA::detail {

// inline uint32 ExtractFlags(const void* ud)
//{
//	return *reinterpret_cast<const uint32*>(static_cast<const uint8*>(ud) + sizeof(func));
//}

template <typename retType, typename... argTypes, std::size_t... argIndices>
int FunctionWrapper(lua_State* ls, std::integer_sequence<std::size_t, argIndices...> indx) {
	using func_ptr = retType (*)(argTypes...);

	void* const ud = lua_touserdata(ls, lua_upvalueindex(1));

	// Extract function pointer and flags from Lua user data
	func_ptr    func = *static_cast<func_ptr*>(ud);
	const Flags flags = *reinterpret_cast<Flags*>(static_cast<uint8_t*>(ud) + sizeof(func_ptr));

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	const int argStackSize[] = { GetStackSize<argTypes>()..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argTypes) + 1] = {};
	argStackIndex[0] = (flags == Flags::call) ? 2 : 1;
	for (size_t i = 1; i < sizeof...(argTypes); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	CheckArgs<argTypes...>(ls, argStackIndex, indx);

	// Call C++ function, popping all arguments from the stack
	if constexpr (std::is_void_v<retType>) {
		func(Wrapper<argTypes>::Get(ls, argStackIndex[argIndices])...);
		return 0;
	}
	else {
		return Push(ls, func(Wrapper<argTypes>::Get(ls, argStackIndex[argIndices])...));
	}
}

template <typename retType, typename... argType>
int FunctionWrapper(lua_State* ls) {
	return FunctionWrapper<retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <typename retType, typename... argType>
inline lua_CFunction CreateLuaCFunction(retType (*func)(argType...)) {
	(void)func;
	return FunctionWrapper<retType, argType...>;
}

template <typename F>
inline void registerFunction(lua_State* ls, F functionPtr, const char* functionName, int tableStackIndex) {
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = detail::CreateLuaCFunction(functionPtr);
	PushFunctionAsUpvalue(ls, luaFunc, &functionPtr, sizeof(functionPtr), Flags::none);
	lua_settable(ls, tableStackIndex);
}

} // namespace Typhoon::LUA::detail
