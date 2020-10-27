#pragma once

#include <lua/src/lua.hpp>
#include "typeWrapper.h"
#include <string>

struct lua_State;

namespace Typhoon::LuaBind {

bool toString(lua_State* ls, int numArg, const char*& s);
bool toString(lua_State* ls, int numArg, std::string& s);
bool toBool(lua_State* ls, int numArg, bool& i);
bool toInteger(lua_State* ls, int numArg, int& i);
bool toFloat(lua_State* ls, int numArg, float& d);
bool toDouble(lua_State* ls, int numArg, double& d);

// Helpers
template <class T>
inline int match(lua_State* ls, const T& /*object*/, int stackIndex) {
	return Wrapper<T>::Match(ls, stackIndex);
}

template <class T>
inline int push(lua_State* ls, const T& object) {
	return Wrapper<T>::Push(ls, object);
}

template <class T>
inline int push(lua_State* ls, T* object) {
	return Wrapper<T*>::Push(ls, object);
}

template <class T>
inline int pushAsKey(lua_State* ls, const T& object) {
	return Wrapper<T>::PushAsKey(ls, object);
}

inline int pushAsKey(lua_State* ls, const char* str) {
	return Wrapper<const char*>::PushAsKey(ls, str);
}

inline int push(lua_State* ls, const char* str) {
	return Wrapper<const char*>::Push(ls, str);
}

template <class T>
inline T get(lua_State* ls, int idx) {
	return Wrapper<T>::Get(ls, idx);
}

template <class T>
inline constexpr int getStackSize() {
	return Wrapper<T>::stackSize;
}

} // namespace Typhoon::LuaBind
