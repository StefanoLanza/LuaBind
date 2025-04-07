#pragma once

#include "typeWrapper.h"

namespace Typhoon::LuaBind {

bool toString(lua_State* ls, int numArg, const char*& s);
bool toString(lua_State* ls, int numArg, std::string& s);
bool toBool(lua_State* ls, int numArg, bool& i);
bool toInteger(lua_State* ls, int numArg, int& i);
bool toFloat(lua_State* ls, int numArg, float& d);
bool toDouble(lua_State* ls, int numArg, double& d);

// Helpers

template <class T>
inline void push(lua_State* ls, const T& object) {
	Wrapper<T>::push(ls, object);
}

template <class T>
inline void pushAsKey(lua_State* ls, const T& object) {
	Wrapper<T>::pushAsKey(ls, object);
}

template <class T>
inline auto pop(lua_State* ls, int idx) -> decltype(Wrapper<T>::pop(ls, idx)) {
	return Wrapper<T>::pop(ls, idx);
}

template <typename T>
inline int checkArg(lua_State* ls, int stackIndex) {
	if (! Wrapper<T>::match(ls, stackIndex)) {
		// Note: check that you're not passing by const reference a primitive object because they are handled by pointer
		return luaL_argerror(ls, stackIndex, lua_typename(ls, lua_type(ls, stackIndex)));
	}
	return 1;
}

template <typename... argTypes, std::size_t... argIndices>
inline void checkArgs(lua_State* ls, const int stackIndices[], std::integer_sequence<std::size_t, argIndices...>) {
	// Call CheckArg for each function argument
	const int foo[] = { checkArg<argTypes>(ls, stackIndices[argIndices])..., 0 };
	(void)foo;
}

} // namespace Typhoon::LuaBind
