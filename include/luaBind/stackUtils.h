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
inline int match(lua_State* ls, int stackIndex) {
	return Wrapper<T>::match(ls, stackIndex);
}

template <class T>
inline void push(lua_State* ls, const T& object) {
	Wrapper<T>::push(ls, object);
}

template <class T>
inline void pushAsKey(lua_State* ls, const T& object) {
	Wrapper<T>::pushAsKey(ls, object);
}

template <class T>
inline auto pop(lua_State* ls, int idx) -> decltype( Wrapper<T>::pop(ls, idx) ) {
	return Wrapper<T>::pop(ls, idx);
}

template <class T>
inline constexpr int getStackSize() {
	return Wrapper<T>::stackSize;
}

} // namespace Typhoon::LuaBind
