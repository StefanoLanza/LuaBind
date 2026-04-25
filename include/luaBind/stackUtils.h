#pragma once

#include "typeWrapper.h"

namespace Typhoon::LuaBind {

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
	(checkArg<argTypes>(ls, stackIndices[argIndices]), ...);
}

} // namespace Typhoon::LuaBind
