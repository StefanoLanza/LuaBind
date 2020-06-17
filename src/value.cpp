#include "value.h"
#include "auxFunc.h"
#include "stackUtils.h"
#include "table.h"
#include "typeSafefy.h"
#include <cassert>

namespace Typhoon::LUA {

Value::Value(lua_State* ls, StackIndex stackIndex)
    : ls(ls) {
	// Create and store a reference to the value on the stack
	lua_pushvalue(ls, stackIndex.GetIndex());
	ref = luaL_ref(ls, LUA_REGISTRYINDEX);
}

Value::Value(Value&& value) noexcept
    : ls(value.ls)
    , ref(value.ref) {
	value.ref = LUA_NOREF;
}

Value::~Value() {
	if (ref != LUA_NOREF) {
		luaL_unref(ls, LUA_REGISTRYINDEX, ref);
	}
}

Value& Value::operator=(Value&& value) noexcept {
	// Steal reference
	ls = value.ls;
	ref = value.ref;
	value.ref = LUA_NOREF;
	return *this;
}

int Value::getType() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int type = lua_type(ls, -1);
	lua_pop(ls, 1);
	return type;
}

bool Value::IsNil() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const bool isNil = lua_isnil(ls, -1);
	lua_pop(ls, 1);
	return isNil;
}

bool Value::Cast(int& value) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toInteger(ls, -1, value);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(double& value) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toDouble(ls, -1, value);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(float& value) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toFloat(ls, -1, value);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(std::string& str) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toString(ls, -1, str);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(const char*& str) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toString(ls, -1, str);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(bool& value) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = toBool(ls, -1, value);
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(void*& ptr) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = false;
	ptr = nullptr;
	const int luaType = lua_type(ls, -1);
	if (luaType == LUA_TLIGHTUSERDATA) {
		ptr = lua_touserdata(ls, -1);
		res = true;
	}
	else if (luaType == LUA_TUSERDATA) {
		std::memcpy(&ptr, lua_touserdata(ls, -1), sizeof ptr);
		res = true;
	}
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(Table& table) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = false;
	if (lua_istable(ls, -1)) {
		table = Table(ls, topStackIndex);
		res = true;
	}
	lua_pop(ls, 1);
	return res;
}

bool Value::Cast(void*& userData, TypeId typeId) const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool res = false;
	userData = nullptr;
	const int luaType = lua_type(ls, -1);
	if (luaType == LUA_TLIGHTUSERDATA) {
		userData = lua_touserdata(ls, -1);
	}
	else if (luaType == LUA_TUSERDATA) {
		std::memcpy(&userData, lua_touserdata(ls, -1), sizeof userData);
	}
	lua_pop(ls, 1);
	if (userData) {
		res = true;
#if LUA_TYPE_SAFE
		if (! detail::checkPointerType(userData, typeId)) {
			userData = nullptr;
			res = false;
		}
#endif
	}
	return res;
}

} // namespace Typhoon::LUA
