#include "value.h"
#include "context.h"
#include "stackUtils.h"
#include "table.h"
#include <cassert>

namespace Typhoon::LuaBind {

Value::Value(lua_State* ls, StackIndex stackIndex)
    : ls(ls) {
	// Create and store a reference to the value on the stack
	lua_pushvalue(ls, stackIndex.getIndex());
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

bool Value::isNil() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const bool isNil = lua_isnil(ls, -1);
	lua_pop(ls, 1);
	return isNil;
}

std::optional<bool> Value::asBool() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	bool value = false;
	bool res = toBool(ls, -1, value);
	lua_pop(ls, 1);
	if (! res) {
		return std::nullopt;
	}
	return value;
}

std::optional<int> Value::asInt() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	int  value = 0;
	bool res = toInteger(ls, -1, value);
	lua_pop(ls, 1);
	if (! res) {
		return std::nullopt;
	}
	return value;
}

std::optional<float> Value::asFloat() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	float value = 0.0f;
	bool  res = toFloat(ls, -1, value);
	lua_pop(ls, 1);
	if (! res) {
		return std::nullopt;
	}
	return value;
}

std::optional<double> Value::asDouble() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	double value = 0.0;
	bool   res = toDouble(ls, -1, value);
	lua_pop(ls, 1);
	if (! res) {
		return std::nullopt;
	}
	return value;
}

std::optional<const char*> Value::asString() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const char* str = nullptr;
	bool        res = toString(ls, -1, str);
	lua_pop(ls, 1);
	if (! res) {
		return std::nullopt;
	}
	return str;
}

std::optional<void*> Value::asPtr() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int luaType = lua_type(ls, -1);
	if (luaType == LUA_TLIGHTUSERDATA) {
		void* ptr = lua_touserdata(ls, -1);
		lua_pop(ls, 1);
		return ptr;
	}
	else if (luaType == LUA_TUSERDATA) {
		void* ptr = nullptr;
		std::memcpy(&ptr, lua_touserdata(ls, -1), sizeof ptr);
		lua_pop(ls, 1);
		return ptr;
	}
	// Not a pointer
	lua_pop(ls, 1);
	return std::nullopt;
}

std::optional<Table> Value::asTable() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	if (lua_istable(ls, -1)) {
		Table table = Table(ls, topStackIndex);
		lua_pop(ls, 1);
		return table;
	}
	// Not a table
	lua_pop(ls, 1);
	return std::nullopt;
}

bool Value::cast(void*& userData, [[maybe_unused]] TypeId typeId) const {
	userData = nullptr;
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int luaType = lua_type(ls, -1);
	if (luaType == LUA_TLIGHTUSERDATA) {
		userData = lua_touserdata(ls, -1);
#if TY_LUABIND_TYPE_SAFE
		if (! detail::checkPointerType(ls, userData, typeId)) {
			userData = nullptr;
		}
#endif
	}
	else if (luaType == LUA_TUSERDATA) {
		std::memcpy(&userData, lua_touserdata(ls, -1), sizeof userData);
#if TY_LUABIND_TYPE_SAFE
		lua_getiuservalue(ls, -1, 1);
		assert(! lua_isnil(ls, -1));
		TypeId ptrTypeId;
		ptrTypeId.impl = reinterpret_cast<const void*>(lua_tointeger(ls, -1));
		if (! detail::compatibleTypes(ls, ptrTypeId, typeId)) {
			userData = nullptr;
		}
		lua_pop(ls, 1);
#endif
	}
	else if (luaType == LUA_TTABLE) {
		// Pointer as _ptr field of a table
		lua_getfield(ls, -1, "_ptr");
		userData = lua_touserdata(ls, -1);
		lua_pop(ls, 1); // _ptr
		if (userData) {
#if TY_LUABIND_TYPE_SAFE
			lua_getfield(ls, -1, "_typeid");
			assert(lua_islightuserdata(ls, -1));
			TypeId ptrTypeId;
			ptrTypeId.impl = lua_touserdata(ls, -1);
			lua_pop(ls, 1);
			if (! detail::compatibleTypes(ls, ptrTypeId, typeId)) {
				userData = nullptr;
			}
#endif
		}
	}
	lua_pop(ls, 1);

	return (userData != nullptr);
}

} // namespace Typhoon::LuaBind
