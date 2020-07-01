#include "table.h"
#include <cassert>

namespace Typhoon::LuaBind {

Table::Table()
    : ls(nullptr)
    , ref(LUA_NOREF) {
}

Table::Table(lua_State* ls, StackIndex stackIndex)
    : ls { ls } {
	assert(lua_istable(ls, stackIndex.getIndex()));
	lua_pushvalue(ls, stackIndex.getIndex());
	ref = luaL_ref(ls, LUA_REGISTRYINDEX);
}

Table::Table(lua_State* ls, Reference ref_)
    : ls { ls } {
	// Duplicate reference to avoid double unreferencing
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref_.getValue());
	assert(lua_istable(ls, -1));
	ref = luaL_ref(ls, LUA_REGISTRYINDEX);
}

Table::Table(Table&& table) noexcept
    : ls { table.ls }
    , ref { table.ref } {
	table.ref = LUA_NOREF;
}

Table::~Table() {
	luaL_unref(ls, LUA_REGISTRYINDEX, ref);
}

Table& Table::operator=(Table&& table) noexcept {
	luaL_unref(ls, LUA_REGISTRYINDEX, ref);
	ls = table.ls;
	ref = table.ref;
	table.ref = LUA_NOREF;
	return *this;
}

void Table::setFunction(const char* name, lua_CFunction f) {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_pushstring(ls, name);
	lua_pushcfunction(ls, f);
	lua_settable(ls, -3);
	lua_pop(ls, 1);
	// clean stack verified
}

bool Table::getFunction(const char* functionName) {
	assert(ls);
	assert(functionName);

	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_getfield(ls, -1, functionName);
	if (! lua_isfunction(ls, -1)) {
		lua_pop(ls, 2); // function, table
		return false;
	}
	lua_pop(ls, 1); // table
	return true;
}

size_t Table::getLength() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	const size_t len = lua_rawlen(ls, -1);
	lua_pop(ls, 1);
	return len;
}

int Table::getCount() const {
	int count = 0;
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_pushnil(ls);
	while (lua_next(ls, -2) != 0) {
		// 'key' is at index -2 and 'value' at index -1
		// remove 'value'; keep 'key' for next iteration
		lua_pop(ls, 1);
		++count;
	}
	lua_pop(ls, 1); // table
	// clean stack verified
	return count;
}

bool Table::hasElement(int index) const {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_rawgeti(ls, -1, index);
	const bool res = lua_isnil(ls, -1) ? false : true;
	lua_pop(ls, 2); // element, table
	// clean stack verified
	return res;
}

Value Table::operator[](const char* key) const {
	assert(key);
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_getfield(ls, -1, key);
	Value value { ls, topStackIndex };
	lua_pop(ls, 2); // table, value
	return value;
}

Value Table::operator[](int key) const {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_pushinteger(ls, static_cast<lua_Integer>(key));
	lua_gettable(ls, -2);
	Value value { ls, topStackIndex };
	lua_pop(ls, 2); // table, value
	return value;
}

Value Table::operator[](unsigned int key) const {
	return (*this)[static_cast<int>(key)];
}

Value Table::operator[](double key) const {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_pushnumber(ls, static_cast<lua_Number>(key));
	lua_gettable(ls, -2);
	Value value { ls, topStackIndex };
	lua_pop(ls, 2); // table, value
	return value;
}

Value Table::operator[](void* key) const {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	lua_pushlightuserdata(ls, key);
	lua_gettable(ls, -2);
	Value value { ls, topStackIndex };
	lua_pop(ls, 2); // table, value
	return value;
}

TableIterator Table::begin() const {
	return TableIterator(ls, ref, 0);
}

TableIterator Table::end() const {
	return TableIterator(ls, ref, getCount());
}

Table globals(lua_State* ls) {
	return Table { ls, Reference { LUA_RIDX_GLOBALS } };
}

Table registry(lua_State* ls) {
	return Table { ls, StackIndex { LUA_REGISTRYINDEX } };
}

Table newtable(lua_State* ls) {
	lua_newtable(ls);
	Table table { ls, topStackIndex };
	lua_pop(ls, 1); // table
	// clean stack verified
	return table;
}

} // namespace Typhoon::LuaBind
