#include "value.h"
#include "context.h"
#include "stackUtils.h"
#include "table.h"
#include <cassert>

namespace Typhoon::LuaBind {

Value::Value(lua_State* ls, StackIndex stackIndex)
    : ls { ls } {
	// Create and store a reference to the value on the stack
	lua_pushvalue(ls, stackIndex.getIndex());
	ref = luaL_ref(ls, LUA_REGISTRYINDEX);
}

Value::Value(Value&& value) noexcept
    : ls { value.ls }
    , ref { value.ref } {
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
	return getType() == LUA_TNIL;
}

std::optional<Nil> Value::asNil() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	if (! lua_isnil(ls, -1)) {
		lua_pop(ls, 1);
		return std::nullopt;
	}
	lua_pop(ls, 1);
	return Nil {};
}

std::optional<bool> Value::asBool() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	if (lua_isboolean(ls, -1)) {
		bool b = lua_toboolean(ls, -1) ? true : false;
		lua_pop(ls, 1);
		return b;
	}
	lua_pop(ls, 1);
	return std::nullopt;
}

std::optional<lua_Integer> Value::asInteger() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_Integer i = lua_tointeger(ls, -1);
	if (i != 0 || lua_isinteger(ls, -1)) { /* avoid extra test when d is not 0 */
		lua_pop(ls, 1);
		return i;
	}
	lua_pop(ls, 1);
	return std::nullopt;
}

std::optional<lua_Number> Value::asNumber() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	// Note: lua_isnumber returns true for strings that can be converted to a number
	if (lua_isnumber(ls, -1)) {
	//if (lua_type(ls, -1) == LUA_TNUMBER) {
		lua_Number d = lua_tonumber(ls, -1);
		lua_pop(ls, 1);
		return d;
	}
	lua_pop(ls, 1);
	return std::nullopt;
}

std::optional<const char*> Value::asString() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	// Note: lua_isstring returns true for numbers that can be converted to a string
	if (lua_isstring(ls, -1)) {
	//if (lua_type(ls, -1) == LUA_TSTRING) {
		const char* str = lua_tostring(ls, -1);
		lua_pop(ls, 1);
		return str;
	}
	lua_pop(ls, 1);
	return std::nullopt;
}

std::optional<void*> Value::asUserData() const {
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
	// Not a userdata
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

std::optional<void*> Value::toPtr([[maybe_unused]] TypeId typeId) const {
	void* ud = nullptr;
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const int luaType = lua_type(ls, -1);
	bool      res = false;
	if (luaType == LUA_TNIL) {
		// Nil represents a null ptr
	}
	else if (luaType == LUA_TLIGHTUSERDATA) {
		ud = lua_touserdata(ls, -1);
#if TY_LUABIND_TYPE_SAFE
		res = detail::checkPointerType(ls, ud, typeId);
#endif
	}
	else if (luaType == LUA_TUSERDATA) {
		std::memcpy(&ud, lua_touserdata(ls, -1), sizeof ud);
#if TY_LUABIND_TYPE_SAFE
		lua_getiuservalue(ls, -1, 1);
		assert(! lua_isnil(ls, -1));
		TypeId ptrTypeId;
		ptrTypeId.impl = reinterpret_cast<const void*>(lua_tointeger(ls, -1));
		res = detail::compatibleTypes(ls, ptrTypeId, typeId);
		lua_pop(ls, 1);
#else
		res = true;
#endif
	}
	else if (luaType == LUA_TTABLE) {
		// Pointer as _ptr field of a table
		lua_getfield(ls, -1, "_ptr");
		ud = lua_touserdata(ls, -1);
		lua_pop(ls, 1); // _ptr
		if (ud) {
#if TY_LUABIND_TYPE_SAFE
			lua_getfield(ls, -1, "_typeid");
			assert(lua_islightuserdata(ls, -1));
			TypeId ptrTypeId;
			ptrTypeId.impl = lua_touserdata(ls, -1);
			lua_pop(ls, 1);
			res = detail::compatibleTypes(ls, ptrTypeId, typeId);
#else
			res = true;
#endif
		}
		else {
			res = false; // table does not represent a pointer
		}
	}
	// else invalid Lua type

	lua_pop(ls, 1); // lua_rawgeti
	if (res) {
		return ud;
	}
	return std::nullopt;
}

} // namespace Typhoon::LuaBind
