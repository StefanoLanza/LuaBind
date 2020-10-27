#pragma once

#include "reference.h"
#include "typeWrapper.h"
#include <cassert>
#include <cstdint>

namespace Typhoon::LuaBind::detail {

void* allocTemporary(size_t size, size_t alignment);

// Helper
template <class T>
inline T* allocTemporary() {
	return static_cast<T*>(allocTemporary(sizeof(T), alignof(T)));
}

// Delete an object after collection
template <class T>
int garbageCollect(lua_State* ls);

// Extract a raw pointer from a table.
// Returns nullptr on error
void* retrievePointerFromTable(lua_State* ls, int idx);

template <class obj_type, class ret_type>
class MemberVariableGetter {
public:
	static int closure(lua_State* ls) {
		const size_t offset = static_cast<size_t>(lua_tonumber(ls, lua_upvalueindex(1)));

		// Get self
		const obj_type* self = Wrapper<const obj_type*>::Get(ls, 1);
		if (! self) {
			luaL_argerror(ls, 1, "nil self");
		}

		// Get pointer to member var
		const ret_type* memberVar = reinterpret_cast<const ret_type*>(reinterpret_cast<uintptr_t>(self) + offset);

		return push(ls, *memberVar);
	}
};

template <class obj_type, class ret_type>
class MemberVariableSetter {
public:
	static int closure(lua_State* ls) {
		const size_t offset = static_cast<size_t>(lua_tonumber(ls, lua_upvalueindex(1)));

		// Get self
		obj_type* self = Wrapper<obj_type*>::Get(ls, 1);
		if (! self) {
			luaL_argerror(ls, 1, "nil self");
		}

		// Get pointer to member var
		ret_type* memberVar = reinterpret_cast<ret_type*>(reinterpret_cast<uintptr_t>(self) + offset);

		// Get value
		*memberVar = Wrapper<ret_type>::Get(ls, 2);

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

void pushFunctionAsUpvalue(lua_State* ls, lua_CFunction closure, const void* functionPointer, size_t functionPointerSize);

template <class T>
void pushObjectAsFullUserData(lua_State* ls, T* objectPtr);

void pushObjectAsFullUserData(lua_State* ls, void* objectPtr, const char* className);

template <typename T>
inline int checkArg(lua_State* ls, int stackIndex) {
	if (! Wrapper<T>::Match(ls, stackIndex)) {
		// Note: check that you're not passing by const reference a primitive object because they are handled by pointer
		luaL_argerror(ls, stackIndex, lua_typename(ls, lua_type(ls, stackIndex)));
	}
	return 1;
}

template <typename... argTypes, std::size_t... argIndices>
inline void checkArgs(lua_State* ls, const int stackIndices[], std::integer_sequence<std::size_t, argIndices...>) {
	// Call CheckArg for each function argument
	const int foo[] = { checkArg<argTypes>(ls, stackIndices[argIndices])..., 0 };
	(void)foo;
}

// Create a new object and return it to Lua as a full user data, optionally with a destructor for GC
template <typename T>
int newObject(lua_State* ls);

} // namespace Typhoon::LuaBind::detail

#include "detail.inl"
