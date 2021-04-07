#pragma once

#include "table.h"
#include "typeWrapper.h"

namespace Typhoon::LuaBind {

// Table traits
template <>
class Wrapper<Table> {
public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_istable(ls, idx);
	}
	static int pushAsKey(lua_State* ls, const Table& table) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.getReference().getValue());
		return 1;
	}
	static int push(lua_State* ls, const Table& table) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.getReference().getValue());
		return 1;
	}
	static Table pop(lua_State* ls, int idx) {
		return Table { ls, StackIndex { idx } };
	}
};

} // namespace Typhoon::LuaBind
