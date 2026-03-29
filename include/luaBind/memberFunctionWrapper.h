#pragma once

#include "typeWrapper.h"
#include <core/ptrUtil.h>
#include <lua/src/lua.hpp>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

template <typename objType, typename retType, typename... argType, std::size_t... argIndices>
int memberFunctionWrapperImpl(lua_State* ls, std::index_sequence<argIndices...> indx) {
	const void* const ud = lua_touserdata(ls, lua_upvalueindex(1));

	// Extract function pointer from Lua user data
	using func_ptr = retType (objType::*)(argType...);
	const func_ptr func = serializePOD<func_ptr>(ud);

	// Get self
	objType* const self = Wrapper<objType*>::pop(ls, 1);
	if (! self) {
		return luaL_argerror(ls, 1, "nil self");
	}

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	constexpr int argStackSize[] = { Wrapper<argType>::getStackSize()..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argType) + 1] = {};
	argStackIndex[0] = 2;
	for (size_t i = 1; i < sizeof...(argType); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	checkArgs<argType...>(ls, argStackIndex, indx);

	if constexpr (std::is_void_v<retType>) {
		(self->*func)(Wrapper<argType>::pop(ls, argStackIndex[argIndices])...);
		return 0;
	}
	else {
		Wrapper<retType>::push(ls, (self->*func)(Wrapper<argType>::pop(ls, argStackIndex[argIndices])...));
		return Wrapper<retType>::getStackSize();
	}
}

template <typename objType, typename retType, typename... argType>
int memberFunctionWrapper(lua_State* ls) {
	return memberFunctionWrapperImpl<objType, retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <typename objType, typename retType, typename... argType>
inline void registerMemberFunction(lua_State* ls, retType (objType::*func)(argType...), const char* functionName, int tableStackIndex) {
#ifdef _DEBUG
	lua_pushstring(ls, functionName);
	lua_gettable(ls, tableStackIndex);
	assert(lua_isnil(ls, -1)); // function with the same already registered
	lua_pop(ls, 1);
#endif
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = memberFunctionWrapper<objType, retType, argType...>;
	pushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func));
	lua_settable(ls, tableStackIndex);
}

// const
template <typename objType, typename retType, typename... argType>
inline void registerMemberFunction(lua_State* ls, retType (objType::*func)(argType...) const, const char* functionName, int tableStackIndex) {
#ifdef _DEBUG
	lua_pushstring(ls, functionName);
	lua_gettable(ls, tableStackIndex);
	assert(lua_isnil(ls, -1)); // function with the same already registered
	lua_pop(ls, 1);
#endif
	lua_pushstring(ls, functionName);
	lua_CFunction luaFunc = memberFunctionWrapper<objType, retType, argType...>;
	pushFunctionAsUpvalue(ls, luaFunc, &func, sizeof(func));
	lua_settable(ls, tableStackIndex);
}

template <class obj_type, class ret_type>
class MemberVariableGetter {
public:
	static int closure(lua_State* ls) {
		const size_t offset = static_cast<size_t>(lua_tonumber(ls, lua_upvalueindex(1)));

		// Get self
		const obj_type* self = Wrapper<const obj_type*>::pop(ls, 1);
		if (! self) {
			return luaL_argerror(ls, 1, "nil self");
		}

		// Get pointer to member var
		const ret_type* memberVar = reinterpret_cast<const ret_type*>(reinterpret_cast<uintptr_t>(self) + offset);

		Wrapper<ret_type>::push(ls, *memberVar);
		return Wrapper<ret_type>::getStackSize();
	}
};

template <class obj_type, class ret_type>
class MemberVariableSetter {
public:
	static int closure(lua_State* ls) {
		const size_t offset = static_cast<size_t>(lua_tonumber(ls, lua_upvalueindex(1)));

		// Get self
		obj_type* self = Wrapper<obj_type*>::pop(ls, 1);
		if (! self) {
			return luaL_argerror(ls, 1, "nil self");
		}

		// Get pointer to member var
		ret_type* memberVar = reinterpret_cast<ret_type*>(reinterpret_cast<uintptr_t>(self) + offset);

		// Get value
		*memberVar = Wrapper<ret_type>::pop(ls, 2);

		return 0;
	}
};

template <typename OBJECT_TYPE, typename MEMBER_TYPE>
void registerGetter(lua_State* ls, MEMBER_TYPE OBJECT_TYPE::*field, size_t offset, const char* functionName, int tableStackIndex) {
	(void)field;
	lua_pushstring(ls, functionName);
	lua_CFunction closure = MemberVariableGetter<OBJECT_TYPE, MEMBER_TYPE>::closure;
	lua_pushnumber(ls, static_cast<lua_Number>(offset));
	lua_pushcclosure(ls, closure, 1);
	lua_settable(ls, tableStackIndex);
}

template <typename OBJECT_TYPE, typename MEMBER_TYPE>
void registerSetter(lua_State* ls, MEMBER_TYPE OBJECT_TYPE::*field, size_t offset, const char* functionName, int tableStackIndex) {
	(void)field;
	lua_pushstring(ls, functionName);
	lua_CFunction closure = MemberVariableSetter<OBJECT_TYPE, MEMBER_TYPE>::closure;
	lua_pushnumber(ls, static_cast<lua_Number>(offset));
	lua_pushcclosure(ls, closure, 1);
	lua_settable(ls, tableStackIndex);
}

} // namespace Typhoon::LuaBind::detail
